#include "stdafx.h"

#include "../xrRender/FBasicVisual.h"

#include "../../xr_3da/CustomHUD.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "../../xr_3da/xr_object.h"

tmc::task<void> CRender::main_run()
{
    XR_TRACY_ZONE_SCOPED();

    auto& dsgraph = get_imm_context();
    XR_ASSERT(dsgraph.mapDistort.empty() && dsgraph.mapHUDDistort.empty(), "", dsgraph.mapDistort, dsgraph.mapHUDDistort);

    // Check if we touch some light even trough portal
    static xr_vector<ISpatial*> spatial_lights;
    g_SpatialSpace->q_sphere(spatial_lights, 0, STYPE_LIGHTSOURCE, Device.vCameraPosition, EPS_L);

    for (auto spatial : spatial_lights)
    {
        const auto& entity_pos = spatial->spatial_sector_point();

        spatial->spatial_updatesector(dsgraph.detect_sector(entity_pos));
        if (spatial->spatial.sector_id == INVALID_SECTOR_ID)
            // disassociated from S/P structure
            continue;

        // lightsource
        XR_ASSERT(spatial->spatial.type & STYPE_LIGHTSOURCE);
        Lights.add_light(XR_ASSERT_VAL(smart_cast<light*>(spatial->dcast_Light()) != nullptr));
    }

    Device.Statistic->RenderCALC.Begin();
    dsgraph.r_pmask(true, true, true);
    co_await dsgraph.build_subspace(last_sector_id, ViewBase);
    Device.Statistic->RenderCALC.End();
}

void CRender::render_menu()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = get_imm_context().cmd_list;

    PIX_EVENT_CTX(cmd_list, render_menu);

    //	Globals
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(FALSE);
    cmd_list.set_ColorWriteEnable();

    // Main Render
    {
        Target->u_setrt(cmd_list, Target->rt_Generic_0, {}, {}, Target->rt_Base_Depth); // LDR RT
        g_pGamePersistent->OnRenderPPUI_main(); // PP-UI
    }

    // Distort
    {
        constexpr Fcolor ColorRGBA{127.0f / 255.0f, 127.0f / 255.0f, 0.0f, 127.0f / 255.0f};

        Target->u_setrt(cmd_list, Target->rt_Generic_1, {}, {}, Target->rt_Base_Depth); // Now RT is a distortion mask
        cmd_list.ClearRT(Target->rt_Generic_1, ColorRGBA);
        g_pGamePersistent->OnRenderPPUI_PP(); // PP-UI
    }

    // Actual Display
    Target->u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, Target->get_base_rt(), nullptr, nullptr, Target->get_base_zb());
    cmd_list.set_Shader(Target->s_menu);
    cmd_list.set_Geometry(Target->g_menu);

    Fvector2 p0, p1;
    constexpr u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.f;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);

    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

tmc::task<void> CRender::Render()
{
    auto& dsgraph = get_imm_context();
    auto& cmd_list = dsgraph.cmd_list;

    PIX_EVENT_CTX(cmd_list, CRender_Render);

    rmNormal(cmd_list);

    bool _menu_pp = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;
    if (_menu_pp)
    {
        render_menu();
        co_return;
    }

    IMainMenu* pMainMenu{g_pGamePersistent ? g_pGamePersistent->m_pMainMenu : nullptr};
    bool bMenu{pMainMenu ? pMainMenu->CanSkipSceneRendering() : false};

    if (!(g_pGameLevel && g_hud) || bMenu)
    {
        Target->u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, Target->get_base_rt(), nullptr, nullptr, Target->get_base_zb());
        co_return;
    }

    XR_TRACY_ZONE_SCOPED();

    // Transfer to global space to avoid deep pointer access
    const f32 fov_factor = _sqr(90.f / Device.fFOV);
    const f32 g_fSCREEN = gsl::narrow_cast<f32>(Target->get_width(cmd_list) * Target->get_height(cmd_list)) * fov_factor * (EPS_S + ps_r__LOD);
    r_ssaDISCARD = _sqr(ps_r__ssaDISCARD) / g_fSCREEN;
    r_ssaLOD_A = _sqr(ps_r2_ssaLOD_A / 3) / g_fSCREEN;
    r_ssaLOD_B = _sqr(ps_r2_ssaLOD_B / 3) / g_fSCREEN;
    r_ssaGLOD_start = _sqr(ps_r__GLOD_ssa_start / 3) / g_fSCREEN;
    r_ssaGLOD_end = _sqr(ps_r__GLOD_ssa_end / 3) / g_fSCREEN;
    r_dtex_range = ps_r2_df_parallax_range * g_fSCREEN / (1024.0f * 768.0f);

    if (m_bFirstFrameAfterReset)
    {
        m_bFirstFrameAfterReset = false;
        co_return;
    }

    // Detect camera-sector
    if (!Device.vCameraDirectionSaved.similar(Device.vCameraPosition, EPS_L))
    {
        XR_TRACY_ZONE_SCOPED();

        const auto sector_id = dsgraph.detect_sector(Device.vCameraPosition);
        if (sector_id != INVALID_SECTOR_ID)
        {
            if (sector_id != last_sector_id)
                g_pGamePersistent->OnSectorChanged(sector_id);

            last_sector_id = sector_id;
        }
    }

    //
    Lights.Update();

    auto main = co_await tmc::fork_clang(main_run());
    auto rain = tmc::fork_group();
    auto sun = tmc::fork_group();

    const bool rain_active = rain_prepare();
    if (rain_active)
        co_await rain.fork_clang(rain_run());

    const bool sun_active = sun_prepare();
    if (sun_active)
        co_await sun.fork_clang(sun_run());

    Target->phase_scene_prepare();
    co_await std::move(main);

    if (g_hud->RenderActiveItemUIQuery())
        dsgraph.render_hud_ui();

    if (o.ssfx_core)
    {
        // HUD Masking rendering
        constexpr Fcolor ColorRGBA{1.0f, 0.0f, 0.0f, 1.0f};
        cmd_list.ClearRT(Target->rt_ssfx_hud, ColorRGBA);

        Target->u_setrt(cmd_list, Target->rt_ssfx_hud, {}, {}, Target->get_base_zb());
        dsgraph.render_hud(true);

        // Reset Depth
        cmd_list.ClearZB(Target->get_base_zb(), 1.0f);
    }

    if (ps_r2_ls_flags.test(R2FLAG_TERRAIN_PREPASS))
    {
        Target->u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, nullptr, nullptr, nullptr, Target->rt_MSAADepth);
        dsgraph.render_landscape(0, false);
    }

    //******* Main render :: PART-0	-- first
    {
        XR_TRACY_ZONE_SCOPED();
        PIX_EVENT_CTX(cmd_list, DEFER_PART0_SPLIT);

        // level, SPLIT
        Target->phase_scene_begin();
        dsgraph.render_graph(0);
        Target->disable_aniso();
    }

    //******* Occlusion testing of volume-limited light-sources
    Target->phase_occq();
    LP_normal.clear();

    if (o.dx10_msaa)
        cmd_list.set_ZB(Target->rt_MSAADepth);

    {
        PIX_EVENT_CTX(cmd_list, DEFER_TEST_LIGHT_VIS);

        // perform tests
        light_Package& LP = Lights.package;

        // stats
        stats.l_shadowed = LP.v_shadowed.size();
        stats.l_unshadowed = LP.v_point.size() + LP.v_spot.size();
        stats.l_total = stats.l_shadowed + stats.l_unshadowed;

        // perform tests
        LP_normal.v_point = LP.v_point;
        LP_normal.v_shadowed = LP.v_shadowed;
        LP_normal.v_spot = LP.v_spot;
        LP_normal.vis_prepare();
    }

    //******* Main render :: PART-1 (second)
    {
        XR_TRACY_ZONE_SCOPED();
        PIX_EVENT_CTX(cmd_list, DEFER_PART1_SPLIT);

        // level
        Target->phase_scene_begin();

        dsgraph.render_hud();
        dsgraph.render_lods();

        if (Details != nullptr)
            co_await Details->Render(cmd_list);

        if (ps_r2_ls_flags.test(R2FLAG_TERRAIN_PREPASS))
            dsgraph.render_landscape(1, true);

        Target->phase_scene_end();
    }

    // Wall marks
    if (Wallmarks)
    {
        PIX_EVENT_CTX(cmd_list, DEFER_WALLMARKS);

        Target->phase_wallmarks();
        Wallmarks->Render(); // wallmarks has priority as normal geometry
    }

    // Update incremental shadowmap-visibility solver
    {
        PIX_EVENT_CTX(cmd_list, DEFER_FLUSH_OCCLUSION);

        auto scope = co_await tmc::enter(xr::tmc_cpu_st_executor());

        for (auto light : Lights_LastFrame)
        {
            if (light == nullptr)
                continue;

            for (auto& svis : light->svis)
                svis.flushoccq();
        }

        Lights_LastFrame.clear();
        co_await scope.exit();
    }

    // full screen pass to mark msaa-edge pixels in highest stencil bit
    if (o.dx10_msaa)
    {
        PIX_EVENT_CTX(cmd_list, MARK_MSAA_EDGES);
        Target->mark_msaa_edges(cmd_list);
    }

    co_await std::move(rain);

    if (rain_active)
        rain_sync();

    {
        // Save previus and current matrices
        {
            static Fmatrix mm_saved_viewproj;

            if (!Device.m_SecondViewport.IsSVPFrame())
            {
                Target->Matrix_previous.mul(mm_saved_viewproj, Device.mInvView);
                Target->Matrix_current.set(Device.mProject);
                mm_saved_viewproj.set(Device.mFullTransform);
            }
        }

        if (o.ssfx_sss && !Device.m_SecondViewport.IsSVPFrame())
        {
            static bool sss_rendered, sss_extended_rendered;
            constexpr Fcolor ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f};

            // SSS Shadows
            if (ps_ssfx_sss_quality.z > 0)
            {
                Target->phase_ssfx_sss();
                sss_rendered = true;
            }
            else
            {
                if (sss_rendered) // Clear buffer
                {
                    sss_rendered = false;
                    cmd_list.ClearRT(Target->rt_ssfx_sss, ColorRGBA);
                }
            }

            if (ps_ssfx_sss_quality.w > 0)
            {
                // Extra lights
                Target->phase_ssfx_sss_ext(Lights.package);
                sss_extended_rendered = true;
            }
            else
            {
                if (sss_extended_rendered) // Clear buffer
                {
                    sss_extended_rendered = false;
                    cmd_list.ClearRT(Target->rt_ssfx_sss_tmp, ColorRGBA);
                }
            }
        }
    }

    // Directional light - sun
    co_await std::move(sun);

    if (sun_active)
    {
        stats.l_visible++;

        co_await sun_sync();
        Target->accum_direct_blend(cmd_list);
    }

    {
        PIX_EVENT_CTX(cmd_list, DEFER_SELF_ILLUM);

        Target->phase_accumulator(cmd_list);
        // Render emissive geometry, stencil - write 0x0 at pixel pos
        cmd_list.set_xform_project(Device.mProject);
        cmd_list.set_xform_view(Device.mView);

        // Stencil - write 0x1 at pixel pos -
        if (!o.dx10_msaa)
            cmd_list.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        else
            cmd_list.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_ColorWriteEnable();

        dsgraph.render_emissive(!o.ssfx_bloom);
    }

    if (o.ssfx_bloom)
    {
        // Render Emissive on `rt_ssfx_bloom_emissive`
        cmd_list.ClearRT(Target->rt_ssfx_bloom_emissive, {});

        Target->u_setrt(cmd_list, Target->rt_ssfx_bloom_emissive, {}, {}, Target->rt_MSAADepth);
        dsgraph.render_emissive(true, true);
    }

    // Lighting, dependant on OCCQ
    {
        PIX_EVENT_CTX(cmd_list, DEFER_LIGHT_OCCQ);

        Target->phase_accumulator(cmd_list);

        co_await tmc::spawn_clang(LP_normal.vis_update(), xr::tmc_cpu_st_executor());
        LP_normal.sort();

        co_await render_lights(LP_normal);
    }

    if (o.ssfx_volumetric)
        Target->phase_ssfx_volumetric_blur();

    // Postprocess
    {
        PIX_EVENT_CTX(cmd_list, DEFER_LIGHT_COMBINE);
        co_await Target->phase_combine();
    }

    if (Details)
        Details->details_clear();

    XR_ASSERT(dsgraph.mapDistort.empty() && dsgraph.mapHUDDistort.empty(), "", dsgraph.mapDistort, dsgraph.mapHUDDistort);
}
