#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_guizmo.h"

#include "embedded_editor_helper.h"

#include "Inventory.h"
#include "Weapon.h"
#include "player_hud.h"

namespace xr
{
namespace detail
{
namespace
{
ImGuizmo::OPERATION mCurrentGizmoOperation{ImGuizmo::TRANSLATE};
constexpr ImGuizmo::MODE mCurrentGizmoMode{ImGuizmo::WORLD};

bool mGizmoUseSnap{};
std::array<f32, 3> mGizmoSnap{3.0f, 3.0f, 3.0f};

bool useHudMatrix{true};
} // namespace

void ShowImGuizmo(bool& show)
{
    if (!show)
        return;

    show = false;

    if (g_player_hud == nullptr)
        return;

    auto item = g_player_hud->attached_item(0);
    if (item == nullptr)
        return;

    auto wpn = smart_cast<CWeapon*>(Actor()->inventory().ActiveItem());
    if (wpn == nullptr)
        return;

    show = true;

    const xr::detail::ImguiWnd wnd{"ImGuizmo", &show};
    if (wnd.Collapsed)
        return;

    f32* matrix;
    Fvector pos, ypr;
    Fvector* tr;
    Fvector* rot;

    Fmatrix v, p;
    auto cameraView = reinterpret_cast<f32*>(&v);
    auto cameraProjection = reinterpret_cast<f32*>(&p);

    if (useHudMatrix)
    {
        matrix = reinterpret_cast<f32*>(&item->m_attach_offset);
        tr = &item->m_measures.m_item_attach[0];
        rot = &item->m_measures.m_item_attach[1];

        v = Device.mViewHud;
        p = Device.mProjectHud;
        p.invert();
    }
    else
    {
        matrix = reinterpret_cast<f32*>(&wpn->m_Offset);

        pos = pSettings->r_fvector3(wpn->cNameSect(), "position");
        ypr = pSettings->r_fvector3(wpn->cNameSect(), "orientation");
        tr = &pos;
        rot = &ypr;

        v = Device.mView;
        p = Device.mProject;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_H))
        useHudMatrix = !useHudMatrix;

    ImGui::Checkbox("Use Hud Matrix", &useHudMatrix);

    if (ImGui::IsKeyPressed(ImGuiKey_T))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    else if (ImGui::IsKeyPressed(ImGuiKey_R))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    ImGui::SameLine();

    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;

    static f32 drag_intensity{0.01f};

    ImGui::DragFloat("intensity", &drag_intensity, 0.000001f, 0.000001f, 1.0f, "%.6f");

    std::array<f32, 3> matrixTranslation, matrixRotation, matrixScale;

    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation.data(), matrixRotation.data(), matrixScale.data());

    ImGui::DragFloat3("Tr", matrixTranslation.data(), drag_intensity, 0.0f, 0.0f, "%.6f");
    ImGui::DragFloat3("Rt", matrixRotation.data(), drag_intensity * 10.f, 0.0f, 0.0f, "%.6f");

    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation.data(), matrixRotation.data(), matrixScale.data(), matrix);

    if (ImGui::IsKeyPressed(ImGuiKey_S))
        mGizmoUseSnap = !mGizmoUseSnap;

    ImGui::Checkbox("Use Snap", &mGizmoUseSnap);

    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE: ImGui::InputFloat3("Snap", mGizmoSnap.data()); break;
    case ImGuizmo::ROTATE: ImGui::InputFloat("Angle Snap", mGizmoSnap.data()); break;
    default: NODEFAULT;
    }

    ImGui::SetNextWindowSize(ImVec2{800.0f, 400.0f}, ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2{400.0f, 20.0f}, ImGuiCond_Appearing);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ImColor{0.35f, 0.3f, 0.3f}});

    const auto& io = ImGui::GetIO();
    ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);

    std::array<f32, 16> deltaMatrix{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, deltaMatrix.data(), mGizmoUseSnap ? mGizmoSnap.data() : nullptr))
    {
        std::array<f32, 3> matrixTranslation2, matrixRotation2, matrixScale2;

        ImGuizmo::DecomposeMatrixToComponents(deltaMatrix.data(), matrixTranslation2.data(), matrixRotation2.data(), matrixScale2.data());

        if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
            tr->add(Fvector{}.set(matrixTranslation2.data()));
        else if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
            rot->add(Fvector{}.set(matrixRotation2.data()));

        if (!useHudMatrix)
        {
            wpn->m_Offset.setHPB(Fvector{ypr}.mul(PI / 180.0f));
            wpn->m_Offset.translate_over(pos);
        }
    }

    ImGui::PopStyleColor(1);
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
