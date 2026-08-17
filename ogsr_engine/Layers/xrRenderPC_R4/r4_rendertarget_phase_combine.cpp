#include "stdafx.h"

#include "../xrRender/dxEnvironmentRender.h"

#include "../../xr_3da/Environment.h"
#include "../../xr_3da/IGame_Persistent.h"

tmc::task<void> CRenderTarget::phase_combine()
{
    XR_TRACY_ZONE_SCOPED();

    auto& dsgraph = RImplementation.get_imm_context();
    auto& cmd_list = dsgraph.cmd_list;

    PIX_EVENT_CTX(cmd_list, phase_combine);

    bool ssfx_PrevPos_Requiered = false;

    //	TODO: DX10: Remove half poxel offset
    bool _menu_pp = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;

    Fvector2 p0, p1;

    //*** exposure-pipeline
    u32 gpu_id = Device.dwFrame % HW.Caps.iGPUNum;

    if (Device.m_SecondViewport.IsSVPActive()) //--#SM+#-- +SecondVP+
    {
        // Фикс "мерцания" tonemapping (HDR) после выключения двойного рендера.
        // Побочный эффект - при работе двойного рендера скорость изменения tonemapping (HDR) падает в два раза
        // Мерцание связано с тем, что HDR для своей работы хранит уменьшенние копии "прошлых кадров"
        // Эти кадры относительно похожи друг на друга, однако при включЄнном двойном рендере
        // в половине кадров оказывается картинка из второго рендера, и поскольку она часто может отличатся по цвету\яркости
        // то при попытке создания "плавного" перехода между ними получается эффект мерцания
        gpu_id = (Device.dwFrame - 1) % HW.Caps.iGPUNum;
    }

    {
        t_LUM_src->surface_set(rt_LUM_pool[gpu_id * 2 + 0]->pSurface);
        t_LUM_dest->surface_set(rt_LUM_pool[gpu_id * 2 + 1]->pSurface);
    }

    // Save previus and current matrices
    Fvector2 m_blur_scale;
    {
        static Fmatrix m_saved_viewproj;

        if (!Device.m_SecondViewport.IsSVPFrame())
        {
            static Fvector3 saved_position;
            Position_previous.set(saved_position);
            saved_position.set(Device.vCameraPosition);

            Matrix_previous.mul(m_saved_viewproj, Device.mInvView);
            Matrix_current.set(Device.mProject);
            m_saved_viewproj.set(Device.mFullTransform);
        }
        float scale = ps_r2_mblur / 2.f;
        m_blur_scale.set(scale, -scale).div(12.f);
    }

    {
        // Disable when rendering SecondViewport
        if (!Device.m_SecondViewport.IsSVPFrame())
        {
            // Clear RT
            constexpr Fcolor ColorRGBA{0.0f, 0.0f, 0.0f, 1.0f};
            cmd_list.ClearRT(rt_ssfx_temp, ColorRGBA);
            cmd_list.ClearRT(rt_ssfx_temp2, ColorRGBA);

            if (RImplementation.o.ssfx_ao && ps_ssfx_ao.y > 0)
            {
                ssfx_PrevPos_Requiered = true;
                phase_ssfx_ao(); // [SSFX] - New AO Phase
            }

            if (RImplementation.o.ssfx_il && ps_ssfx_il.y > 0)
            {
                ssfx_PrevPos_Requiered = true;
                phase_ssfx_il(); // [SSFX] - New IL Phase
            }
        }
    }

    constexpr Fcolor ColorRGBA{0.0f, 0.0f, 0.0f, 0.0f};

    // low/hi RTs
    if (!RImplementation.o.dx10_msaa)
    {
        cmd_list.ClearRT(rt_Generic_0, ColorRGBA);
        cmd_list.ClearRT(rt_Generic_1, ColorRGBA);
        u_setrt(cmd_list, rt_Generic_0, rt_Generic_1, {}, get_base_zb());
    }
    else
    {
        cmd_list.ClearRT(rt_Generic_0_r, ColorRGBA);
        cmd_list.ClearRT(rt_Generic_1_r, ColorRGBA);
        u_setrt(cmd_list, rt_Generic_0_r, rt_Generic_1_r, {}, rt_MSAADepth);
    }

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // draw skybox
    g_pGamePersistent->Environment().RenderSky();

    //	Igor: Render clouds before compine without Z-test
    //	to avoid siluets. HOwever, it's a bit slower process.
    g_pGamePersistent->Environment().RenderClouds();

    //
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00); // stencil should be >= 1

    // Draw full-screen quad textured with our scene image
    if (!_menu_pp)
    {
        XR_TRACY_ZONE_SCOPED();
        PIX_EVENT_CTX(cmd_list, combine_1);

        // Compute params
        CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
        constexpr float minamb = 0.001f;
        Fvector4 ambclr{std::max(envdesc.ambient.x * 2.f, minamb), std::max(envdesc.ambient.y * 2.f, minamb), std::max(envdesc.ambient.z * 2.f, minamb), 0.f};
        ambclr.mul(ps_r2_sun_lumscale_amb);

        Fvector4 envclr;
        if (!g_pGamePersistent->Environment().USED_COP_WEATHER)
            envclr.set(envdesc.sky_color.x * 2 + EPS, envdesc.sky_color.y * 2 + EPS, envdesc.sky_color.z * 2 + EPS, envdesc.weight);
        else
            envclr.set(envdesc.hemi_color.x * 2 + EPS, envdesc.hemi_color.y * 2 + EPS, envdesc.hemi_color.z * 2 + EPS, envdesc.weight);

        Fvector4 fogclr{envdesc.fog_color.x, envdesc.fog_color.y, envdesc.fog_color.z, 0.0f};
        envclr.x *= 2 * ps_r2_sun_lumscale_hemi;
        envclr.y *= 2 * ps_r2_sun_lumscale_hemi;
        envclr.z *= 2 * ps_r2_sun_lumscale_hemi;
        Fvector4 sunclr, sundir;

        // sun-params
        {
            auto& sun = *smart_cast<const light*>(RImplementation.Lights.sun._get());
            Fvector L_dir, L_clr;
            float L_spec;
            L_clr.set(sun.color.r, sun.color.g, sun.color.b);
            L_spec = u_diffuse2s(L_clr);
            Device.mView.transform_dir(L_dir, sun.direction);
            L_dir.normalize();

            sunclr.set(L_clr.x, L_clr.y, L_clr.z, L_spec);
            sundir.set(L_dir.x, L_dir.y, L_dir.z, 0);
        }

        // Fill VB
        float scale_X = float(Device.dwWidth) / float(TEX_jitter);
        float scale_Y = float(Device.dwHeight) / float(TEX_jitter);

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(-1, 1, 0, 1, 0, 0, scale_Y);
        verts[1].set(-1, -1, 0, 0, 0, 0, 0);
        verts[2].set(1, 1, 1, 1, 0, scale_X, scale_Y);
        verts[3].set(1, -1, 1, 0, 0, scale_X, 0);

        const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw
        cmd_list.set_Element(s_combine->E[0]);
        cmd_list.set_Geometry(g_combine);

        cmd_list.set_c("m_v2w", Device.mInvView);
        cmd_list.set_c("L_ambient", ambclr);

        cmd_list.set_c("Ldynamic_color", sunclr);
        cmd_list.set_c("Ldynamic_dir", sundir);

        cmd_list.set_c("env_color", envclr);
        cmd_list.set_c("fog_color", fogclr);

        if (!RImplementation.o.dx10_msaa)
        {
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
        else
        {
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

            cmd_list.set_Element(s_combine_msaa->E[0]);
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

            cmd_list.set_Stencil(FALSE, D3DCMP_EQUAL, 0x01, 0xff, 0);
        }
    }

    auto pContext = cmd_list.context();

    // Copy previous rt
    if (!RImplementation.o.dx10_msaa)
        pContext->CopyResource(rt_Generic_temp->pTexture->surface_get(), rt_Generic_0->pTexture->surface_get());
    else
        pContext->CopyResource(rt_Generic_temp->pTexture->surface_get(), rt_Generic_0_r->pTexture->surface_get());

    if (RImplementation.o.ssfx_ssr && !Device.m_SecondViewport.IsSVPFrame())
    {
        ssfx_PrevPos_Requiered = true;
        phase_ssfx_ssr(); // [SSFX] - New SSR Phase
    }

    // [SSFX] - Water SSR rendering
    if (RImplementation.o.ssfx_water && !Device.m_SecondViewport.IsSVPFrame())
    {
        constexpr Fcolor ColorRGBA{0.0f, 0.0f, 0.0f, 1.0f};

        cmd_list.ClearRT(rt_ssfx_temp, ColorRGBA);
        cmd_list.ClearRT(rt_ssfx_temp2, ColorRGBA);

        u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);

        const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
        const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

        // Render Scale
        cmd_list.set_viewport_size(w / ps_ssfx_water.x, h / ps_ssfx_water.x);

        // Render Water SSR
        cmd_list.set_xform_world(Fidentity);
        dsgraph.render_water_ssr();

        // Restore Viewport
        cmd_list.set_viewport_size(w, h);

        // Save Frame
        pContext->CopyResource(rt_ssfx_water->pTexture->surface_get(), rt_ssfx_temp->pTexture->surface_get());

        // Water SSR Blur
        phase_ssfx_water_blur();

        // Water waves
        phase_ssfx_water_waves();
    }

    if (!RImplementation.o.dx10_msaa)
        u_setrt(cmd_list, rt_Generic_0, {}, {}, get_base_zb());
    else
        u_setrt(cmd_list, rt_Generic_0_r, {}, {}, rt_MSAADepth);

    // Final water rendering ( All the code above can be omitted if the Water module isn't installed )
    cmd_list.set_xform_world(Fidentity);
    dsgraph.render_water();

    {
        if (RImplementation.o.ssfx_rain)
        {
            phase_ssfx_rain(); // Render a small color buffer to do the refraction and more

            if (!RImplementation.o.dx10_msaa)
                u_setrt(cmd_list, rt_Generic_0, {}, {}, get_base_zb());
            else
                u_setrt(cmd_list, rt_Generic_0_r, {}, {}, rt_MSAADepth);
        }

        co_await g_pGamePersistent->Environment().RenderLast(); // rain/thunder-bolts
    }

    if (ssfx_PrevPos_Requiered)
        pContext->CopyResource(rt_ssfx_prevPos->pTexture->surface_get(), rt_Position->pTexture->surface_get());

    // Forward rendering
    {
        PIX_EVENT_CTX(cmd_list, Forward_rendering);

        if (!RImplementation.o.dx10_msaa)
            u_setrt(cmd_list, rt_Generic_0, {}, {}, get_base_zb()); // LDR RT
        else
            u_setrt(cmd_list, rt_Generic_0_r, {}, {}, rt_MSAADepth); // LDR RT

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(FALSE);
        cmd_list.set_ColorWriteEnable();
        dsgraph.render_forward();

        if (g_pGamePersistent)
            g_pGamePersistent->OnRenderPPUI_main(); // PP-UI
    }

    //	Igor: for volumetric lights
    //	combine light volume here
    if (RImplementation.o.ssfx_volumetric)
    {
        if (m_bHasActiveVolumetric || m_bHasActiveVolumetric_spot)
            phase_combine_volumetric();
    }
    else
    {
        if (m_bHasActiveVolumetric)
            phase_combine_volumetric();
    }

    // Perform blooming filter and distortion if needed
    cmd_list.set_Stencil(FALSE);

    if (RImplementation.o.dx10_msaa)
    {
        // we need to resolve rt_Generic_1 into rt_Generic_1_r
        pContext->ResolveSubresource(rt_Generic_1->pTexture->surface_get(), 0, rt_Generic_1_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
        pContext->ResolveSubresource(rt_Generic_0->pTexture->surface_get(), 0, rt_Generic_0_r->pTexture->surface_get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
    }

    // for msaa we need a resolved color buffer - Holger
    phase_bloom(); // HDR RT invalidated here

    // Distortion filter
    const bool bDistort = !dsgraph.mapDistort.empty() || !dsgraph.mapHUDDistort.empty() || _menu_pp;
    if (bDistort)
    {
        PIX_EVENT_CTX(cmd_list, render_distort_objects);

        constexpr Fcolor ColorRGBA{127.0f / 255.0f, 127.0f / 255.0f, 0.0f, 127.0f / 255.0f};

        if (!RImplementation.o.dx10_msaa)
        {
            u_setrt(cmd_list, rt_Generic_1, {}, {}, get_base_zb()); // Now RT is a distortion mask
            cmd_list.ClearRT(rt_Generic_1, ColorRGBA);
        }
        else
        {
            u_setrt(cmd_list, rt_Generic_1_r, {}, {}, rt_MSAADepth); // Now RT is a distortion mask
            cmd_list.ClearRT(rt_Generic_1_r, ColorRGBA);
        }

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(FALSE);
        cmd_list.set_ColorWriteEnable();

        dsgraph.render_distort();

        if (g_pGamePersistent)
            g_pGamePersistent->OnRenderPPUI_PP(); // PP-UI
    }

    cmd_list.set_Stencil(FALSE);

    // Screen space sunshafts
    if (!_menu_pp && (ps_r_sunshafts_mode & SS_SS_MASK) && need_to_render_sunshafts())
    {
        PIX_EVENT_CTX(cmd_list, phase_ss_ss);
        PhaseSSSS();
    }

    // Compute blur textures
    if (!Device.m_SecondViewport.IsSVPFrame()) // Temp fix for blur buffer and SVP
        phase_blur();

    // Compute bloom (new)
    if (RImplementation.o.ssfx_bloom)
    {
        if (!Device.m_SecondViewport.IsSVPFrame())
            phase_ssfx_bloom();
        else
            cmd_list.ClearRT(rt_ssfx_bloom1, ColorRGBA);
    }

    if (ps_r2_ls_flags.test(R2FLAG_DOF))
        phase_dof();

    if (ps_r2_mask_control.x > 0)
    {
        phase_gasmask_dudv();
        phase_gasmask_drops();
    }

    // Postprocess anti-aliasing
    if (ps_smaa_quality)
    {
        ProcessSMAA();
        cmd_list.set_Stencil(FALSE);
    }

    // HOLGER - HACK
    constexpr BOOL PP_Complex = TRUE;

    // Combine everything + perform AA
    if (RImplementation.o.dx10_msaa)
    {
        if (PP_Complex)
            u_setrt(cmd_list, rt_Generic, {}, {}, get_base_zb()); // LDR RT
        else
            u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, get_base_zb());
    }
    else
    {
        if (PP_Complex)
            u_setrt(cmd_list, rt_Color, {}, {}, get_base_zb()); // LDR RT
        else
            u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, get_base_zb());
    }

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    {
        XR_TRACY_ZONE_SCOPED();
        PIX_EVENT_CTX(cmd_list, combine_2);

        //
        struct alignas(8) v_aa
        {
            Fvector2 pxy, pzw;
            Fvector2 uv0;
            Fvector2 uv1;
            Fvector2 uv2;
            Fvector2 uv3;
            Fvector2 uv4;
            Fvector2 uv5xy, uv5zw;
            Fvector2 uv6xy, uv6zw;
        };
        static_assert(sizeof(struct v_aa) == CRenderTarget::aa_aa_stride);

        float _w = float(Device.dwWidth);
        float _h = float(Device.dwHeight);
        float ddw = 1.f / _w;
        float ddh = 1.f / _h;
        p0.set(.5f / _w, .5f / _h);
        p1.set((_w + .5f) / _w, (_h + .5f) / _h);

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<v_aa>(4);

        verts[0].pxy.set(EPS, float(_h + EPS));
        verts[0].pzw.set(EPS, 1.f);
        verts[0].uv0.set(p0.x, p1.y);
        verts[0].uv1.set(p0.x - ddw, p1.y - ddh);
        verts[0].uv2.set(p0.x + ddw, p1.y + ddh);
        verts[0].uv3.set(p0.x + ddw, p1.y - ddh);
        verts[0].uv4.set(p0.x - ddw, p1.y + ddh);
        verts[0].uv5xy.set(p0.x - ddw, p1.y);
        verts[0].uv5zw.set(p1.y, p0.x + ddw);
        verts[0].uv6xy.set(p0.x, p1.y - ddh);
        verts[0].uv6zw.set(p1.y + ddh, p0.x);

        verts[1].pxy.set(EPS, EPS);
        verts[1].pzw.set(EPS, 1.f);
        verts[1].uv0.set(p0.x, p0.y);
        verts[1].uv1.set(p0.x - ddw, p0.y - ddh);
        verts[1].uv2.set(p0.x + ddw, p0.y + ddh);
        verts[1].uv3.set(p0.x + ddw, p0.y - ddh);
        verts[1].uv4.set(p0.x - ddw, p0.y + ddh);
        verts[1].uv5xy.set(p0.x - ddw, p0.y);
        verts[1].uv5zw.set(p0.y, p0.x + ddw);
        verts[1].uv6xy.set(p0.x, p0.y - ddh);
        verts[1].uv6zw.set(p0.y + ddh, p0.x);

        verts[2].pxy.set(float(_w + EPS), float(_h + EPS));
        verts[2].pzw.set(EPS, 1.f);
        verts[2].uv0.set(p1.x, p1.y);
        verts[2].uv1.set(p1.x - ddw, p1.y - ddh);
        verts[2].uv2.set(p1.x + ddw, p1.y + ddh);
        verts[2].uv3.set(p1.x + ddw, p1.y - ddh);
        verts[2].uv4.set(p1.x - ddw, p1.y + ddh);
        verts[2].uv5xy.set(p1.x - ddw, p1.y);
        verts[2].uv5zw.set(p1.y, p1.x + ddw);
        verts[2].uv6xy.set(p1.x, p1.y - ddh);
        verts[2].uv6zw.set(p1.y + ddh, p1.x);

        verts[3].pxy.set(float(_w + EPS), EPS);
        verts[3].pzw.set(EPS, 1.f);
        verts[3].uv0.set(p1.x, p0.y);
        verts[3].uv1.set(p1.x - ddw, p0.y - ddh);
        verts[3].uv2.set(p1.x + ddw, p0.y + ddh);
        verts[3].uv3.set(p1.x + ddw, p0.y - ddh);
        verts[3].uv4.set(p1.x - ddw, p0.y + ddh);
        verts[3].uv5xy.set(p1.x - ddw, p0.y);
        verts[3].uv5zw.set(p0.y, p1.x + ddw);
        verts[3].uv6xy.set(p1.x, p0.y - ddh);
        verts[3].uv6zw.set(p0.y + ddh, p1.x);

        const auto Offset = cmd_list.Vertex.Unlock<v_aa>(4);

        // Draw COLOR
        if (!RImplementation.o.dx10_msaa)
            cmd_list.set_Element(s_combine->E[bDistort ? 4 : 2]); // look at blender_combine.cpp
        else
            cmd_list.set_Element(s_combine_msaa->E[bDistort ? 4 : 2]); // look at blender_combine.cpp

        cmd_list.set_c("m_current", Matrix_current);
        cmd_list.set_c("m_previous", Matrix_previous);
        cmd_list.set_c("m_blur", m_blur_scale.x, m_blur_scale.y, 0.f, 0.f);
        /////lvutner
        cmd_list.set_c("mask_control", ps_r2_mask_control);
        //////////

        cmd_list.set_Geometry(g_aa_AA);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    cmd_list.set_Stencil(FALSE);

    g_pGamePersistent->Environment().RenderFlares(); // lens-flares

    //	PP-if required
    if (PP_Complex)
    {
        PIX_EVENT_CTX(cmd_list, phase_pp);
        phase_pp();
    }

    //	Re-adapt luminance
    cmd_list.set_Stencil(FALSE);

    //*** exposure-pipeline-clear
    {
        std::swap(rt_LUM_pool[gpu_id * 2 + 0], rt_LUM_pool[gpu_id * 2 + 1]);
        t_LUM_src->surface_set(nullptr);
        t_LUM_dest->surface_set(nullptr);
    }

#ifdef DEBUG
    cmd_list.set_CullMode(CULL_CCW);
    static xr_vector<Fplane> saved_dbg_planes;

    if (bDebug)
        saved_dbg_planes = dbg_planes;
    else
        dbg_planes = saved_dbg_planes;

    for (u32 it = 0; it < dbg_planes.size(); it++)
    {
        Fplane& P = dbg_planes[it];
        Fvector zero;
        zero.mul(P.n, P.d);

        Fvector L_dir, L_up = P.n, L_right;
        L_dir.set(0, 0, 1);

        if (_abs(L_up.dotproduct(L_dir)) > .99f)
            L_dir.set(1, 0, 0);

        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
        L_dir.crossproduct(L_right, L_up);
        L_dir.normalize();

        Fvector p0, p1, p2, p3;
        constexpr float sz = 100.f;
        p0.mad(zero, L_right, sz).mad(L_dir, sz);
        p1.mad(zero, L_right, sz).mad(L_dir, -sz);
        p2.mad(zero, L_right, -sz).mad(L_dir, -sz);
        p3.mad(zero, L_right, -sz).mad(L_dir, +sz);
        cmd_list.dbg_DrawTRI(Fidentity, p0, p1, p2, 0xffffffff);
        cmd_list.dbg_DrawTRI(Fidentity, p2, p3, p0, 0xffffffff);
    }

    static xr_vector<dbg_line_t> saved_dbg_lines;

    if (bDebug)
        saved_dbg_lines = dbg_lines;
    else
        dbg_lines = saved_dbg_lines;

    for (u32 it = 0; it < dbg_lines.size(); it++)
    {
        cmd_list.dbg_DrawLINE(Fidentity, dbg_lines[it].P0, dbg_lines[it].P1, dbg_lines[it].color);
    }

    dbg_spheres.clear();
    dbg_lines.clear();
    dbg_planes.clear();
#endif
}

void CRenderTarget::phase_wallmarks()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Targets
    cmd_list.set_RT(nullptr, 2);
    cmd_list.set_RT(nullptr, 1);
    u_setrt(cmd_list, rt_Color, {}, {}, rt_MSAADepth);

    // Stencil	- draw only where stencil >= 0x1
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    PIX_EVENT_CTX(cmd_list, phase_combine_volumetric);

    //	TODO: DX10: Remove half pixel offset here

    if (!RImplementation.o.dx10_msaa)
        u_setrt(cmd_list, rt_Generic_0, rt_Generic_1, {}, get_base_zb());
    else
        u_setrt(cmd_list, rt_Generic_0_r, rt_Generic_1_r, {}, rt_MSAADepth);

    //	Sets limits to both render targets
    cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

    // Fill vertex buffer
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(-1, 1, 0, 1, 0, 0, 1);
    verts[1].set(-1, -1, 0, 0, 0, 0, 0);
    verts[2].set(1, 1, 1, 1, 0, 1, 1);
    verts[3].set(1, -1, 1, 0, 0, 1, 0);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw
    cmd_list.set_Element(s_combine_volumetric->E[0]);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.set_ColorWriteEnable();
}
