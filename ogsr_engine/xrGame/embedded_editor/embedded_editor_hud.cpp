#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_hud.h"

#include "embedded_editor_helper.h"

#include "CustomDetector.h"
#include "GamePersistent.h"
#include "HUDManager.h"
#include "Inventory.h"
#include "Weapon.h"
#include "debug_renderer.h"
#include "player_hud.h"

namespace xr
{
namespace detail
{
namespace
{
[[nodiscard]] xr_string toUtf8(gsl::czstring s)
{
    static xr_vector<wchar_t> buf;

    auto n = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
    buf.resize(gsl::narrow<size_t>(n));
    MultiByteToWideChar(CP_ACP, 0, s, -1, buf.data(), gsl::narrow_cast<s32>(std::ssize(buf)));

    xr_string result;
    n = WideCharToMultiByte(CP_UTF8, 0, buf.data(), gsl::narrow_cast<s32>(std::ssize(buf)), nullptr, 0, nullptr, nullptr);

    result.resize(gsl::narrow<size_t>(n));
    n = WideCharToMultiByte(CP_UTF8, 0, buf.data(), gsl::narrow_cast<s32>(std::ssize(buf)), result.data(), gsl::narrow_cast<s32>(std::ssize(result)), nullptr, nullptr);

    return result;
}

void write_fvector3(CInifile* file, gsl::czstring S, gsl::czstring L, Fvector V)
{
    if (V.similar(Fvector{}))
        return;

    if (!pSettings->line_exist(S, L) || !pSettings->r_fvector3(S, L).similar(V))
        file->w_fvector3(S, L, V);
}

void write_hud_params_base(CInifile* file, const attachable_hud_item* hud)
{
    gsl::czstring sect_name = hud->m_sect_name.c_str();

    write_fvector3(file, sect_name, "hands_position", hud->m_measures.m_hands_attach[0]);
    write_fvector3(file, sect_name, "hands_orientation", hud->m_measures.m_hands_attach[1]);

    write_fvector3(file, sect_name, "item_position", hud->m_measures.m_item_attach[0]);
    write_fvector3(file, sect_name, "item_orientation", hud->m_measures.m_item_attach[1]);

    write_fvector3(file, sect_name, "fire_point", hud->m_measures.m_fire_point_offset);
    write_fvector3(file, sect_name, "fire_point2", hud->m_measures.m_fire_point2_offset);
    write_fvector3(file, sect_name, "shell_point", hud->m_measures.m_shell_point_offset);
}

void write_hud_aim_params(CInifile* file, const attachable_hud_item* hud)
{
    write_hud_params_base(file, hud);

    gsl::czstring sect_name = hud->m_sect_name.c_str();
    const auto& m_hands_offset = hud->m_measures.m_hands_offset;

    write_fvector3(file, sect_name, "aim_hud_offset_pos", m_hands_offset[0][hud_item_measures::m_hands_offset_type_aim]);
    write_fvector3(file, sect_name, "aim_hud_offset_rot", m_hands_offset[1][hud_item_measures::m_hands_offset_type_aim]);
    write_fvector3(file, sect_name, "gl_hud_offset_pos", m_hands_offset[0][hud_item_measures::m_hands_offset_type_gl]);
    write_fvector3(file, sect_name, "gl_hud_offset_rot", m_hands_offset[1][hud_item_measures::m_hands_offset_type_gl]);
}

void write_item_params(CInifile* config, const CWeapon* item)
{
    gsl::czstring section_nm = item->cNameSect().c_str();

    write_fvector3(config, section_nm, "laserdot_attach_offset", item->laserdot_attach_offset);
    write_fvector3(config, section_nm, "torch_attach_offset", item->flashlight_attach_offset);
    write_fvector3(config, section_nm, "torch_omni_attach_offset", item->flashlight_omni_attach_offset);
}

bool g_bHudAdjustState{};

void DrawLightCone(Fvector position, Fvector direction, f32 range, f32 angle, u32 color)
{
    Fmatrix cone_transform;
    cone_transform.identity();
    cone_transform.c = position;

    Fvector up{0.0f, 1.0f, 0.0f};
    if (_abs(direction.y) > 0.9f)
        up.set(1.0f, 0.0f, 0.0f);

    Fvector right;
    right.crossproduct(direction, up).normalize();
    up.crossproduct(right, direction).normalize();

    cone_transform.i = right;
    cone_transform.j = up;
    cone_transform.k = direction;

    Level().debug_renderer().draw_cone(cone_transform, range, angle, color, true);
}

void DrawPointText(Fvector position, gsl::czstring text, u32 color = color_rgba(255, 255, 255, 255), f32 height_offset = 0.02f)
{
    Fvector text_pos{position};
    text_pos.y += height_offset;

    Fmatrix FP;
    FP.build_projection(deg2rad(psHUD_FOV <= 1.0f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                        g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Fmatrix mFullTransform;
    mFullTransform.mul(FP, Device.mView);

    Fvector4 v_res;
    mFullTransform.transform(v_res, text_pos);

    if (v_res.z < 0.0f || v_res.w < 0.0f)
        return;

    if (v_res.x < -1.0f || v_res.x > 1.0f || v_res.y < -1.0f || v_res.y > 1.0f)
        return;

    const auto x = (1.0f + v_res.x) / 2.0f * gsl::narrow_cast<f32>(Device.dwWidth);
    const auto y = (1.0f - v_res.y) / 2.0f * gsl::narrow_cast<f32>(Device.dwHeight);
    auto& font = *HUD().Font().pFontMedium;

    font.SetAligment(CGameFont::alCenter);
    font.SetColor(color);
    font.OutSet(x, y);
    font.OutNext("%s", text);
}
} // namespace

void ShowHudEditor(bool& show)
{
    if (!show)
        return;

    if (g_player_hud == nullptr)
    {
        show = false;
        return;
    }

    const xr::detail::ImguiWnd wnd{"HUD Editor", &show};
    if (wnd.Collapsed)
        return;

    static f32 drag_intensity{0.0001f};

    std::ignore = xr::detail::ImGui_HoverDragFloat("intensity", drag_intensity, 0.00001f, 0.000001f, 1.0f, "%.6f");

    bool check = g_bHudAdjustState;
    ImGui::Checkbox("hud_adjust_state", &check);
    g_bHudAdjustState = check;

    std::unique_ptr<CInifile> config;
    const auto create_if_needed = [&config] {
        if (config)
            return;

        string_path fname;
        std::ignore = FS.update_path(fname, "$game_config$", "weapons\\weapons_hud_adjust.ltx");
        config = std::make_unique<CInifile>(fname, false, true, true);
    };

    auto item = g_player_hud->attached_item(0);
    if (item != nullptr)
    {
        ImGui::Separator();

        auto wpn = smart_cast<CWeapon*>(Actor()->inventory().ActiveItem());
        ImGui::TextUnformatted(wpn ? toUtf8(wpn->Name()).c_str() : toUtf8(("st_hud_editor_item_1")).c_str());

        std::ignore = xr::detail::ImGui_HoverDragFloat3("hands_position", item->m_measures.m_hands_attach[0].arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("hands_orientation", item->m_measures.m_hands_attach[1].arr, drag_intensity * 5.0f, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("item_position", item->m_measures.m_item_attach[0].arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("item_orientation", item->m_measures.m_item_attach[1].arr, drag_intensity * 5.0f, 0.0f, 0.0f, "%.6f");

        ImGui::Separator();

        std::ignore = xr::detail::ImGui_HoverDragFloat3("aim_hud_offset_pos", item->m_measures.m_hands_offset[0][hud_item_measures::m_hands_offset_type_aim].arr, drag_intensity,
                                                        0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("aim_hud_offset_rot", item->m_measures.m_hands_offset[1][hud_item_measures::m_hands_offset_type_aim].arr, drag_intensity,
                                                        0.0f, 0.0f, "%.6f");

        ImGui::Separator();

        std::ignore = xr::detail::ImGui_HoverDragFloat3("gl_hud_offset_pos", item->m_measures.m_hands_offset[0][hud_item_measures::m_hands_offset_type_gl].arr, drag_intensity,
                                                        0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("gl_hud_offset_rot", item->m_measures.m_hands_offset[1][hud_item_measures::m_hands_offset_type_gl].arr, drag_intensity,
                                                        0.0f, 0.0f, "%.6f");

        ImGui::Separator();

        std::ignore = xr::detail::ImGui_HoverDragFloat3("fire_point", item->m_measures.m_fire_point_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("shoot_point", item->m_measures.m_shoot_point_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("shell_point", item->m_measures.m_shell_point_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("fire_point2", item->m_measures.m_fire_point2_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");

        if (ImGui::Button("hud save"))
        {
            create_if_needed();
            write_hud_aim_params(config.get(), item);
            std::ignore = config->save_as();
        }

        firedeps fd;
        item->setup_firedeps(fd);
        auto& render = Level().debug_renderer();

        DrawPointText(fd.vLastFP, "fire_point", color_rgba(255, 255, 0, 255));
        render.draw_aabb(fd.vLastFP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(255, 0, 0), true);
        DrawPointText(fd.vLastShootPoint, "shoot_point", color_rgba(255, 255, 0, 255));
        render.draw_aabb(fd.vLastShootPoint, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(5, 107, 0), true);
        DrawPointText(fd.vLastFP2, "fire_point2", color_rgba(255, 255, 0, 255));
        render.draw_aabb(fd.vLastFP2, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(0, 0, 255), true);

        if (wpn != nullptr)
        {
            bool can_save{};

            if (pSettings->line_exist(wpn->cNameSect(), "laser_light_section") || pSettings->line_exist(wpn->cNameSect(), "flashlight_section"))
            {
                ImGui::Separator();

                // Laser light offsets
                if (pSettings->line_exist(wpn->cNameSect(), "laser_light_section"))
                {
                    std::ignore = xr::detail::ImGui_HoverDragFloat3("laserdot_attach_offset", wpn->laserdot_attach_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");

                    can_save = true;

                    if (wpn->IsLaserOn())
                    {
                        Fvector laser_pos = wpn->get_LastFP(), laser_dir = wpn->get_LastFD();
                        wpn->GetBoneOffsetPosDir(wpn->laserdot_attach_bone, laser_pos, laser_dir, wpn->laserdot_attach_offset);
                        wpn->CorrectDirFromWorldToHud(laser_dir);

                        DrawPointText(laser_pos, "laserdot_attach_offset", color_rgba(255, 255, 0, 255));
                        Level().debug_renderer().draw_aabb(laser_pos, 0.002f, 0.002f, 0.002f, color_rgba(0, 255, 0, 255), true);
                        DrawLightCone(laser_pos, laser_dir, 0.5f, 0.002f, color_rgba(255, 0, 0, 255));
                    }
                }

                // Torch light offsets
                if (pSettings->line_exist(wpn->cNameSect(), "flashlight_section"))
                {
                    std::ignore = xr::detail::ImGui_HoverDragFloat3("torch_attach_offset", wpn->flashlight_attach_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");
                    std::ignore = xr::detail::ImGui_HoverDragFloat3("torch_omni_attach_offset", wpn->flashlight_omni_attach_offset.arr, drag_intensity, 0.0f, 0.0f, "%.6f");

                    can_save = true;

                    if (wpn->IsFlashlightOn())
                    {
                        Fvector flashlight_pos = wpn->get_LastFP(), flashlight_dir = wpn->get_LastFD();
                        wpn->GetBoneOffsetPosDir(wpn->flashlight_attach_bone, flashlight_pos, flashlight_dir, wpn->flashlight_attach_offset);
                        wpn->CorrectDirFromWorldToHud(flashlight_dir);

                        DrawPointText(flashlight_pos, "torch_attach_offset", color_rgba(255, 255, 0, 255));
                        Level().debug_renderer().draw_aabb(flashlight_pos, 0.002f, 0.002f, 0.002f, color_rgba(0, 255, 0, 255), true);
                        DrawLightCone(flashlight_pos, flashlight_dir, 0.5f, 0.1f, color_rgba(255, 255, 255, 255));
                    }
                }
            }

            if (can_save && ImGui::Button("item save"))
            {
                create_if_needed();
                write_item_params(config.get(), wpn);
                std::ignore = config->save_as();
            }
        }
    }

    item = g_player_hud->attached_item(1);
    if (item != nullptr)
    {
        ImGui::Separator();

        const auto det = smart_cast<CCustomDetector*>(Actor()->inventory().ItemFromSlot(DETECTOR_SLOT));
        ImGui::TextUnformatted(det ? toUtf8(det->Name()).c_str() : toUtf8(("st_hud_editor_item_2")).c_str());

        std::ignore = xr::detail::ImGui_HoverDragFloat3("[R] hands_position", item->m_measures.m_hands_attach[0].arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("[R] hands_orientation", item->m_measures.m_hands_attach[1].arr, drag_intensity * 5.0f, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("[R] item_position", item->m_measures.m_item_attach[0].arr, drag_intensity, 0.0f, 0.0f, "%.6f");
        std::ignore = xr::detail::ImGui_HoverDragFloat3("[R] item_orientation", item->m_measures.m_item_attach[1].arr, drag_intensity * 5.0f, 0.0f, 0.0f, "%.6f");

        if (ImGui::Button("[R] hud save"))
        {
            create_if_needed();
            write_hud_params_base(config.get(), item);
            std::ignore = config->save_as();
        }
    }
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
