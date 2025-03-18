#include "stdafx.h"
#include "../../xr_3da/igame_persistent.h"
#include "../xrRender/FBasicVisual.h"
#include "../../xr_3da/customhud.h"
#include "../../xr_3da/xr_object.h"

#include "../xrRender/QueryHelper.h"

void CRender::render_menu()
{
    PIX_EVENT(render_menu);
    //	Globals
    RCache.set_CullMode(CULL_CCW);
    RCache.set_Stencil(FALSE);
    RCache.set_ColorWriteEnable();

    // Main Render
    {
        Target->u_setrt(RCache, Target->rt_Generic_0, 0, 0, Target->rt_Base_Depth); // LDR RT
        g_pGamePersistent->OnRenderPPUI_main(); // PP-UI
    }

    // Distort
    {
        constexpr Fcolor ColorRGBA = {127.0f / 255.0f, 127.0f / 255.0f, 0.0f, 127.0f / 255.0f};

        Target->u_setrt(RCache, Target->rt_Generic_1, 0, 0, Target->rt_Base_Depth); // Now RT is a distortion mask
        RCache.ClearRT(Target->rt_Generic_1, ColorRGBA);
        g_pGamePersistent->OnRenderPPUI_PP(); // PP-UI
    }

    // Actual Display
    Target->u_setrt(RCache, Device.dwWidth, Device.dwHeight, Target->get_base_rt(), NULL, NULL, Target->get_base_zb());
    RCache.set_Shader(Target->s_menu);
    RCache.set_Geometry(Target->g_menu);

    Fvector2 p0, p1;
    u32 Offset;
    constexpr u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.f;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);

    FVF::TL* pv = (FVF::TL*)Vertex.Lock(4, Target->g_menu->vb_stride, Offset);
    pv->set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    Vertex.Unlock(4, Target->g_menu->vb_stride);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRender::Render()
{
    PIX_EVENT(CRender_Render);

    auto& dsgraph = get_imm_context();
    VERIFY(dsgraph.mapDistort.empty());

    rmNormal(dsgraph.cmd_list);

    bool _menu_pp = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;
    if (_menu_pp)
    {
        render_menu();
        return;
    }

    IMainMenu* pMainMenu = g_pGamePersistent ? g_pGamePersistent->m_pMainMenu : 0;
    bool bMenu = pMainMenu ? pMainMenu->CanSkipSceneRendering() : false;

    if (!(g_pGameLevel && g_hud) || bMenu)
    {
        Target->u_setrt(RCache, Device.dwWidth, Device.dwHeight, Target->get_base_rt(), NULL, NULL, Target->get_base_zb());
        return;
    }

    if (m_bFirstFrameAfterReset)
    {
        m_bFirstFrameAfterReset = false;
        return;
    }

    // Configure
    o.distortion = FALSE; // disable distorion
    Fcolor sun_color = ((light*)Lights.sun._get())->color;
    const BOOL bSUN = ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r, sun_color.g, sun_color.b) > EPS);

    Target->phase_scene_prepare();

    //*******
    // Sync point
    // Device.Statistic->RenderDUMP_Wait_S.Begin	();
    /*if (1)
    {
        CTimer	T;							T.Start	();
        BOOL	result						= FALSE;
        HRESULT	hr							= S_FALSE;
        //while	((hr=q_sync_point[q_sync_count]->GetData	(&result,sizeof(result),D3DGETDATA_FLUSH))==S_FALSE) {
        while	((hr=GetData (q_sync_point[q_sync_count], &result,sizeof(result)))==S_FALSE)
        {
            if (!SwitchToThread())			Sleep(ps_r2_wait_sleep);
            if (T.GetElapsed_ms() > 500)	{
                result	= FALSE;
                break;
            }
        }
    }*/
    // Device.Statistic->RenderDUMP_Wait_S.End		();
    // q_sync_count								= (q_sync_count+1)%HW.Caps.iGPUNum;
    // CHK_DX										(EndQuery(q_sync_point[q_sync_count]));

    //******* Main calc - DEFERRER RENDERER
    // Main calc
    Device.Statistic->RenderCALC.Begin();
    dsgraph.r_pmask(true, false, true); // enable priority "0",+ capture wmarks
    dsgraph.build_subspace(last_sector_id, ViewBase);
    dsgraph.r_pmask(true, false); // disable priority "1"
    Device.Statistic->RenderCALC.End();

    if (g_hud->RenderActiveItemUIQuery())
        dsgraph.render_hud_ui();

    if (o.ssfx_core)
    {
        // HUD Masking rendering
        constexpr Fcolor ColorRGBA = {1.0f, 0.0f, 0.0f, 1.0f};
        RCache.ClearRT(Target->rt_ssfx_hud, ColorRGBA);

        Target->u_setrt(dsgraph.cmd_list, Target->rt_ssfx_hud, NULL, NULL, Target->get_base_zb());
        dsgraph.render_hud(true);

        // Reset Depth
        RCache.ClearZB(Target->get_base_zb(), 1.0f);
    }

    if (ps_r2_ls_flags.test(R2FLAG_TERRAIN_PREPASS))
    {
        if (o.dx10_msaa)
            Target->u_setrt(dsgraph.cmd_list, Device.dwWidth, Device.dwHeight, NULL, NULL, NULL, Target->rt_MSAADepth);
        else
            Target->u_setrt(dsgraph.cmd_list, Device.dwWidth, Device.dwHeight, NULL, NULL, NULL, Target->get_base_zb());

        dsgraph.render_landscape(0, false);
    }

    //******* Main render :: PART-0	-- first
    PIX_EVENT(DEFER_PART0_SPLIT);
    // level, SPLIT
    Target->phase_scene_begin();
    dsgraph.render_graph(0);
    Target->disable_aniso();

    //******* Occlusion testing of volume-limited light-sources
    Target->phase_occq();
    LP_normal.clear();

    if (o.dx10_msaa)
        RCache.set_ZB(Target->rt_MSAADepth);

    {
        PIX_EVENT(DEFER_TEST_LIGHT_VIS);
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
        LP_normal.vis_prepare(dsgraph.cmd_list);
    }

    //******* Main render :: PART-1 (second)
    PIX_EVENT(DEFER_PART1_SPLIT);

    // level
    Target->phase_scene_begin();
    dsgraph.render_hud();
    dsgraph.render_lods(true, true);
    if (Details)
        Details->Render(dsgraph.cmd_list, false);
    if (ps_r2_ls_flags.test(R2FLAG_TERRAIN_PREPASS))
        dsgraph.render_landscape(1, true);
    Target->phase_scene_end();

    // Wall marks
    if (Wallmarks)
    {
        PIX_EVENT(DEFER_WALLMARKS);
        Target->phase_wallmarks();
        Wallmarks->Render(); // wallmarks has priority as normal geometry
    }

    // Update incremental shadowmap-visibility solver
    {
        PIX_EVENT(DEFER_FLUSH_OCCLUSION);
        u32 it = 0;
        for (it = 0; it < Lights_LastFrame.size(); it++)
        {
            if (0 == Lights_LastFrame[it])
                continue;
            // try
            //{
            Lights_LastFrame[it]->svis[dsgraph.context_id].flushoccq();
            /*}
            catch (...)
            {
                Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
            }*/
        }
        Lights_LastFrame.clear();
    }

    // full screen pass to mark msaa-edge pixels in highest stencil bit
    if (o.dx10_msaa)
    {
        PIX_EVENT(MARK_MSAA_EDGES);
        Target->mark_msaa_edges();
    }

    //	TODO: DX10: Implement DX10 rain.
    if (ps_r2_ls_flags.test(R3FLAG_DYN_WET_SURF))
    {
        PIX_EVENT(DEFER_RAIN);
        render_rain();
    }

    {
        // Save previus and current matrices
        {
            static Fmatrix mm_saved_viewproj;

            if (!Device.m_SecondViewport.IsSVPFrame())
            {
                Fmatrix m_invview;
                m_invview.invert(Device.mView);
                Target->Matrix_previous.mul(mm_saved_viewproj, m_invview);
                Target->Matrix_current.set(Device.mProject);
                mm_saved_viewproj.set(Device.mFullTransform);
            }
        }

        if (o.ssfx_sss && !Device.m_SecondViewport.IsSVPFrame())
        {
            static bool sss_rendered, sss_extended_rendered;
            constexpr Fcolor ColorRGBA = {1, 1, 1, 1};

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
                    RCache.ClearRT(Target->rt_ssfx_sss, ColorRGBA);
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
                    RCache.ClearRT(Target->rt_ssfx_sss_tmp, ColorRGBA);
                }
            }
        }
    }

    // Directional light - fucking sun
    if (bSUN)
    {
        PIX_EVENT(DEFER_SUN);
        stats.l_visible++;
        render_sun_cascades();
        Target->accum_direct_blend();
    }

    {
        PIX_EVENT(DEFER_SELF_ILLUM);
        Target->phase_accumulator();
        // Render emissive geometry, stencil - write 0x0 at pixel pos
        RCache.set_xform_project(Device.mProject);
        RCache.set_xform_view(Device.mView);
        // Stencil - write 0x1 at pixel pos -
        if (!o.dx10_msaa)
            RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        else
            RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        RCache.set_CullMode(CULL_CCW);
        RCache.set_ColorWriteEnable();
        dsgraph.render_emissive(!o.ssfx_bloom);
    }

    if (o.ssfx_bloom)
    {
        // Render Emissive on `rt_ssfx_bloom_emissive`
        RCache.ClearRT(Target->rt_ssfx_bloom_emissive, {});

        if (o.dx10_msaa)
            Target->u_setrt(dsgraph.cmd_list, Target->rt_ssfx_bloom_emissive, NULL, NULL, Target->rt_MSAADepth);
        else
            Target->u_setrt(dsgraph.cmd_list, Target->rt_ssfx_bloom_emissive, NULL, NULL, Target->get_base_zb());

        dsgraph.render_emissive(true, true);
    }

    // Lighting, non dependant on OCCQ
    {
        PIX_EVENT(DEFER_LIGHT_NO_OCCQ);
        Target->phase_accumulator();
        LP_normal.vis_update();
        LP_normal.sort();
        render_lights(LP_normal);
    }

    // Lighting, dependant on OCCQ
    {
        PIX_EVENT(DEFER_LIGHT_OCCQ);
        // render_lights( LP_pending );
    }

    if (o.ssfx_volumetric)
        Target->phase_ssfx_volumetric_blur();

    // Postprocess
    {
        PIX_EVENT(DEFER_LIGHT_COMBINE);
        Target->phase_combine();
    }

    if (Details)
        Details->details_clear();

    VERIFY(dsgraph.mapDistort.empty());
}

void CRender::render_forward()
{
    auto& dsgraph = get_imm_context();
    VERIFY(dsgraph.mapDistort.empty());

    o.distortion = true; // enable distorion

    //******* Main render - second order geometry (the one, that doesn't support deffering)
    //.todo: should be done inside "combine" with estimation of of luminance, tone-mapping, etc.
    // level
    dsgraph.r_pmask(false, true); // enable priority "1"
    dsgraph.build_subspace(last_sector_id, ViewBase);
    //	Igor: we don't want to render old lods on next frame.
    dsgraph.mapLOD.clear();

    if (g_hud->RenderActiveItemUIQuery())
        dsgraph.render_hud_ui();

    dsgraph.render_graph(1); // normal level, secondary priority
    dsgraph.PortalTraverser.fade_render(); // faded-portals
    dsgraph.render_sorted(); // strict-sorted geoms

    o.distortion = FALSE; // disable distorion
}
