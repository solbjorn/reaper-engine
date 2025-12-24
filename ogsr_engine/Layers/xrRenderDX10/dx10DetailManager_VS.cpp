#include "stdafx.h"

#include "../xrRender/DetailManager.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

#include "../xrRenderDX10/dx10BufferUtils.h"

constexpr int quant = 16384;

static float GoToValue(float& current, float go_to)
{
    float diff = abs(current - go_to);

    float r_value = Device.fTimeDelta;

    if (diff - r_value <= 0)
    {
        current = go_to;
        return 0;
    }

    return current < go_to ? r_value : -r_value;
}

void CDetailManager::hw_Render(CBackend& cmd_list, float fade_distance, const Fvector* light_position)
{
    XR_TRACY_ZONE_SCOPED();

    // Render-prepare
    //	Update timer
    //	Can't use Device.fTimeDelta since it is smoothed! Don't know why, but smoothed value looks more choppy!
    float fDelta = Device.fTimeGlobal - m_global_time_old;
    if ((fDelta < 0) || (fDelta > 1))
        fDelta = 0.03f;
    m_global_time_old = Device.fTimeGlobal;

    m_time_rot_1 += (PI_MUL_2 * fDelta / swing_current.rot1);
    m_time_rot_2 += (PI_MUL_2 * fDelta / swing_current.rot2);
    m_time_pos += fDelta * swing_current.speed;

    float tm_rot1 = m_time_rot_1;
    float tm_rot2 = m_time_rot_2;

    Fvector4 dir1, dir2;
    dir1.set(_sin(tm_rot1), 0, _cos(tm_rot1), 0).normalize().mul(swing_current.amp1);
    dir2.set(_sin(tm_rot2), 0, _cos(tm_rot2), 0).normalize().mul(swing_current.amp2);

    // Setup geometry and DMA
    cmd_list.set_Geometry(hw_Geom);

    // Wave0
    constexpr float scale = 1.f / float(quant);
    Fvector4 wave;
    Fvector4 consts;
    consts.set(scale, scale, ps_r__Detail_l_aniso, ps_r__Detail_l_ambient);

    wave.set(1.f / 5.f, 1.f / 7.f, 1.f / 3.f, m_time_pos);

    hw_Render_dump(cmd_list, consts, wave.div(PI_MUL_2), dir1, 1, 0, fade_distance, light_position);

    // Wave1
    wave.set(1.f / 3.f, 1.f / 7.f, 1.f / 5.f, m_time_pos);

    hw_Render_dump(cmd_list, consts, wave.div(PI_MUL_2), dir2, 2, 0, fade_distance, light_position);

    // Still
    consts.set(scale, scale, scale, 1.f);

    hw_Render_dump(cmd_list, consts, wave.div(PI_MUL_2), dir2, 0, 1, fade_distance, light_position);
}

void CDetailManager::hw_Render_dump(CBackend& cmd_list, const Fvector4& consts, const Fvector4& wave, const Fvector4& wind, u32 var_id, u32 lod_id, float fade_distance,
                                    const Fvector* light_position)
{
    XR_TRACY_ZONE_SCOPED();

    constexpr const char* strConsts = "consts";
    constexpr const char* strWave = "wave";
    constexpr const char* strDir2D = "dir2D";
    constexpr const char* strArray = "array";
    constexpr const char* strXForm = "xform";

    constexpr const char* strPos = "benders_pos";
    constexpr const char* strGrassSetup = "benders_setup";

    constexpr const char* strExData = "exdata";
    constexpr const char* strGrassAlign = "grass_align";

    // Grass benders data

    // Add Player?
    if (ps_ssfx_grass_interactive.x > 0)
        grass_shader_data.pos[0].set(Device.vCameraPosition, -1);
    else
        grass_shader_data.pos[0].set(0, 0, 0, -1);
    grass_shader_data.dir[0].set(0.0f, -99.0f, 0.0f, 1.0f);

    Device.Statistic->RenderDUMP_DT_Count = 0;

    // Matrices and offsets
    u32 vOffset = 0;
    u32 iOffset = 0;

    vis_list& list = m_visibles[var_id];

    const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
    Fvector c_sun, c_ambient, c_hemi;
    c_sun.set(desc.sun_color.x, desc.sun_color.y, desc.sun_color.z);
    c_sun.mul(.5f);
    c_ambient.set(desc.ambient.x, desc.ambient.y, desc.ambient.z);
    c_hemi.set(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z);

    // Iterate
    for (u32 O = 0; O < objects.size(); O++)
    {
        CDetail& Object = *objects[O];
        auto& vis = list[O];
        if (!vis.empty())
        {
            for (u32 iPass = 0; iPass < Object.shader->E[lod_id]->passes.size(); ++iPass)
            {
                // Setup matrices + colors (and flush it as necessary)
                cmd_list.set_Element(Object.shader->E[lod_id], iPass);
                cmd_list.apply_lmaterial();

                //	This could be cached in the corresponding consatant buffer
                //	as it is done for DX9
                cmd_list.set_c(strConsts, consts);
                cmd_list.set_c(strWave, wave);
                cmd_list.set_c(strDir2D, wind);
                cmd_list.set_c(strXForm, Device.mFullTransform);
                cmd_list.set_c(strGrassAlign, ps_ssfx_terrain_grass_align);

                if (ps_ssfx_grass_interactive.y > 0)
                {
                    cmd_list.set_c(strGrassSetup, ps_ssfx_int_grass_params_1);

                    Fvector4* c_grass{};
                    cmd_list.get_ConstantDirect(strPos, sizeof(grass_shader_data.pos) + sizeof(grass_shader_data.dir), reinterpret_cast<void**>(&c_grass), nullptr, nullptr);
                    R_ASSERT(c_grass);

                    if (c_grass)
                        xr_memcpy(c_grass, &grass_shader_data.pos, sizeof(grass_shader_data.pos) + sizeof(grass_shader_data.dir));
                }

                Fvector4* c_ExData{};
                cmd_list.get_ConstantDirect(strExData, hw_BatchSize * sizeof(Fvector4), reinterpret_cast<void**>(&c_ExData), nullptr, nullptr);
                R_ASSERT(c_ExData);

                Fvector4* c_storage{};
                cmd_list.get_ConstantDirect(strArray, hw_BatchSize * sizeof(Fvector4) * 4, reinterpret_cast<void**>(&c_storage), nullptr, nullptr);
                R_ASSERT(c_storage);

                u32 dwBatch = 0;

                for (auto* items : vis)
                {
                    for (auto* pInstance : *items)
                    {
                        if (!pInstance)
                            continue;

                        SlotItem& Instance = *pInstance;
                        u32 base = dwBatch * 4;

                        Instance.alpha += GoToValue(Instance.alpha, Instance.alpha_target);

                        float scale = Instance.scale_calculated;

                        // Sort of fade using the scale
                        // fade_distance == fade_light use light_position to define "fade", anything else uses fade_distance
                        if (fade_distance <= fade_light)
                            scale *= 1.0f - Instance.position.distance_to_xz_sqr(*light_position) * 0.005f;
                        else if (Instance.distance > fade_distance)
                            scale *= 1.0f - abs(Instance.distance - fade_distance) * 0.005f;

                        if (scale <= 0 || Instance.alpha <= 0)
                            break;

                        // Build matrix ( 3x4 matrix, last row - color )
                        Fmatrix& M = Instance.mRotY;
                        c_storage[base + 0].set(M._11 * scale, M._21 * scale, M._31 * scale, M._41);
                        c_storage[base + 1].set(M._12 * scale, M._22 * scale, M._32 * scale, M._42);
                        c_storage[base + 2].set(M._13 * scale, M._23 * scale, M._33 * scale, M._43);

                        // Build color
                        // R2 only needs hemisphere
                        float h = Instance.c_hemi;
                        float s = Instance.c_sun;
                        c_storage[base + 3].set(s, s, s, h);

                        if (c_ExData)
                            c_ExData[dwBatch].set(Instance.normal.x, Instance.normal.y, Instance.normal.z, Instance.alpha);

                        dwBatch++;
                        if (dwBatch == hw_BatchSize)
                        {
                            // flush
                            Device.Statistic->RenderDUMP_DT_Count += dwBatch;
                            u32 dwCNT_verts = dwBatch * Object.number_vertices;
                            u32 dwCNT_prims = (dwBatch * Object.number_indices) / 3;
                            cmd_list.Render(D3DPT_TRIANGLELIST, vOffset, 0, dwCNT_verts, iOffset, dwCNT_prims);
                            cmd_list.stat.r.s_details.add(dwCNT_verts);

                            // restart
                            dwBatch = 0;

                            //	Remap constants to memory directly (just in case anything goes wrong)
                            cmd_list.get_ConstantDirect(strArray, hw_BatchSize * sizeof(Fvector4) * 4, reinterpret_cast<void**>(&c_storage), nullptr, nullptr);
                            R_ASSERT(c_storage);
                        }
                    }
                }

                // flush if necessary
                if (dwBatch)
                {
                    Device.Statistic->RenderDUMP_DT_Count += dwBatch;
                    u32 dwCNT_verts = dwBatch * Object.number_vertices;
                    u32 dwCNT_prims = (dwBatch * Object.number_indices) / 3;
                    cmd_list.Render(D3DPT_TRIANGLELIST, vOffset, 0, dwCNT_verts, iOffset, dwCNT_prims);
                    cmd_list.stat.r.s_details.add(dwCNT_verts);
                }
            }

            // Clean up
            // KD: we must not clear vis on r2 since we want details shadows
            if (ps_ssfx_grass_shadows.x <= 0)
            {
                // fade_distance == fade_scene only during PHASE_NORMAL
                bool use_fast_geo = !fsimilar(fade_distance, fade_scene);

                if (!ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) ||
                    ((ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && use_fast_geo) // phase smap with shadows
                     || (ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && !use_fast_geo && (!RImplementation.is_sun())) // phase normal with shadows without sun
                     || (!ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && !use_fast_geo))) // phase normal without shadows
                    vis.clear();
            }
        }

        vOffset += hw_BatchSize * Object.number_vertices;
        iOffset += hw_BatchSize * Object.number_indices;
    }
}
