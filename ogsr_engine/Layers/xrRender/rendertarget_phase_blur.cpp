#include "stdafx.h"

void CRenderTarget::phase_blur()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Get common data
    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    constexpr u32 C = color_rgba(0, 0, 0, 255);
    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    f32 w = gsl::narrow_cast<f32>(Device.dwWidth) * 0.5f;
    f32 h = gsl::narrow_cast<f32>(Device.dwHeight) * 0.5f;

    u_setrt(cmd_list, rt_blur_h_2, {}, {}, rt_blur_2_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[0]);
    cmd_list.set_c("blur_params", 1.f, 0.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_2, {}, {}, rt_blur_2_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[1]);
    cmd_list.set_c("blur_params", 0.f, 1.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur / Half res
    ///////////////////////////////////////////////////////////////////////////////////
    w = gsl::narrow_cast<f32>(Device.dwWidth) * 0.25f;
    h = gsl::narrow_cast<f32>(Device.dwHeight) * 0.25f;

    u_setrt(cmd_list, rt_blur_h_4, {}, {}, rt_blur_4_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[2]);
    cmd_list.set_c("blur_params", 1.f, 0.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_4, {}, {}, rt_blur_4_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[3]);
    cmd_list.set_c("blur_params", 0.f, 1.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    w = gsl::narrow_cast<f32>(Device.dwWidth) * 0.125f;
    h = gsl::narrow_cast<f32>(Device.dwHeight) * 0.125f;

    u_setrt(cmd_list, rt_blur_h_8, {}, {}, rt_blur_8_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[4]);
    cmd_list.set_c("blur_params", 1.f, 0.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_8, {}, {}, rt_blur_8_zb);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[5]);
    cmd_list.set_c("blur_params", 0.f, 1.f, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
}

void CRenderTarget::phase_ssfx_ssr()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    const f32 ScaleFactor = std::min(std::max(ps_ssfx_ssr.x, 1.0f), 2.0f);

    constexpr Fvector2 p0{0.f, 0.f};
    Fvector2 p1{1.f, 1.f};

    // GLOSS /////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp3, {}, {}, get_base_zb());
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ssr->E[5]);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////

    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    f32 scale_X = w / ScaleFactor;
    f32 scale_Y = h / ScaleFactor;

    // SSR ///////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx, {}, {}, get_base_zb());
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    if (ScaleFactor > 1.0f)
        cmd_list.set_viewport_size(scale_X, scale_Y);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Set pass
    cmd_list.set_Element(s_ssfx_ssr->E[0]);
    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);
    cmd_list.set_c("cam_pos", ::Random.randF(-1.0, 1.0), ::Random.randF(-1.0, 1.0), 0.f, 0.f);

    cmd_list.set_c("ssr_setup", ps_ssfx_ssr);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // COPY SSR RESULT ( ACC ) ////////////////////////////////////////////
    cmd_list.context()->CopyResource(rt_ssfx_ssr->pTexture->surface_get(), rt_ssfx->pTexture->surface_get());

    // Disable/Enable Blur if the value is <= 0
    // if (ps_ssfx_ssr.y > 0 || ps_ssfx_ssr.x > 1.0)
    {
        // BLUR PHASE 1 //////////////////////////////////////////////////////////
        u_setrt(cmd_list, rt_ssfx_temp, {}, {}, get_base_zb());
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
        verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
        verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

        Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_ssr->E[1]);
        cmd_list.set_c("blur_params", 1.f, 0.f, scale_X, scale_Y);
        cmd_list.set_c("ssr_setup", ps_ssfx_ssr);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

        // BLUR PHASE 2 //////////////////////////////////////////////////////////
        u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, get_base_zb());
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
        verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
        verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

        Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_ssr->E[2]);
        cmd_list.set_c("blur_params", 0.f, 1.f, w, h);
        cmd_list.set_c("ssr_setup", ps_ssfx_ssr);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // COMBINE //////////////////////////////////////////////////////////
    // Reset Viewport
    if (ScaleFactor > 1.0f)
        cmd_list.set_viewport_size(w, h);

    p1.set(1.0f, 1.0f);

    if (!RImplementation.o.dx10_msaa)
        u_setrt(cmd_list, rt_Generic_0, {}, {}, nullptr);
    else
        u_setrt(cmd_list, rt_Generic_0_r, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ssr->E[3]);
    cmd_list.set_c("ssr_setup", ps_ssfx_ssr);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_ssfx_volumetric_blur()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Be careful and clear the buffer ( rt_Generic_2 contain unspeakable stuff if no volumetric is written )
    if (!m_bHasActiveVolumetric)
        cmd_list.ClearRT(rt_Generic_2->pRT, {});

    if (!m_bHasActiveVolumetric_spot)
    {
        cmd_list.ClearRT(rt_ssfx_volumetric->pRT, {});
        return;
    }

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    // Volumetric always at 1/8 res
    cmd_list.set_viewport_size(w / 8, h / 8);

    ref_rt* rt_VolBlur[2] = {&rt_ssfx_volumetric_tmp, &rt_ssfx_volumetric};
    constexpr float pixelsize[4]{0.0f, 1.0f, 1.0f, 2.0f}; // half pixel + pixelsize
    constexpr float pixelscale[4]{2.0f, 0.5f, 2.0f, 0.5f};

    // BLUR ///////////////////////////////////////////////////////////////////
    for (int b = 0; b < 4; b++)
    {
        u_setrt(cmd_list, *rt_VolBlur[b % 2], {}, {}, nullptr);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
        verts[1].set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
        verts[2].set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
        verts[3].set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);

        const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_volumetric_blur->E[b % 2]);
        cmd_list.set_c("blur_setup", w / 8, h / 8, pixelsize[b], pixelscale[b]);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Restore Viewport
    cmd_list.set_viewport_size(w, h);

    // COMBINE ////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_accum, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
    verts[1].set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
    verts[2].set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
    verts[3].set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_volumetric_blur->E[5]);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.context()->CopyResource(rt_Generic_2->pTexture->surface_get(), rt_ssfx_accum->pTexture->surface_get());
}

void CRenderTarget::phase_ssfx_water_blur()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f};
    Fvector2 p1{0.5f, 0.5f};

    cmd_list.set_viewport_size(w / 2, h / 2);

    if (ps_ssfx_water.y > 0)
    {
        // BLUR PHASE 1 //////////////////////////////////////////////////////////
        u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, nullptr);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
        verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
        verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

        auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_water_blur->E[0]);
        cmd_list.set_c("blur_setup", 1.f, 0.f, 0.f, 2.0f / ps_ssfx_water.x);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

        // BLUR PHASE 2 //////////////////////////////////////////////////////////
        u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
        verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
        verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

        Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_water_blur->E[1]);
        cmd_list.set_c("blur_setup", 0.f, 1.f, 0.f, 1.f);

        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
    else
    {
        cmd_list.context()->CopyResource(rt_ssfx_temp2->pTexture->surface_get(), rt_ssfx_temp->pTexture->surface_get());

        u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_Stencil(FALSE);

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

        verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
        verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
        verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

        const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

        // Draw COLOR
        cmd_list.set_Element(s_ssfx_water_blur->E[2]);
        cmd_list.set_c("blur_setup", 0.f, 0.f, 0.f, 2.0f / ps_ssfx_water.x);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    cmd_list.set_viewport_size(w, h);
    p1.set(1.0f, 1.0f);
}

void CRenderTarget::phase_ssfx_water_waves()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    cmd_list.set_viewport_size(512, 512);

    u_setrt(cmd_list, rt_ssfx_water_waves, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_water_blur->E[5]);
    cmd_list.set_c("wind_setup", g_pGamePersistent->Environment().wind_anim.w, g_pGamePersistent->Environment().CurrentEnv->wind_velocity, 0.f, 0.f);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.set_viewport_size(w, h);
}

void CRenderTarget::phase_ssfx_sss()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(255, 255, 255, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    u_setrt(cmd_list, rt_ssfx, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss->E[0]);

    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);
    cmd_list.set_c("ssfx_sss", ps_ssfx_sss);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR
    u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss->E[1]);
    cmd_list.set_c("blur_setup", 0.f, 1.f, 0.f, 0.f);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR
    u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss->E[2]);
    cmd_list.set_c("blur_setup", 1.f, 0.f, 2.f, 0.f);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.context()->CopyResource(rt_ssfx_sss->pTexture->surface_get(), rt_ssfx_temp2->pTexture->surface_get());
}

namespace
{
constexpr uintptr_t __declspec(align(sizeof(__m128i))) zeros[sizeof(__m128i) / sizeof(uintptr_t)]{};

ICF void memset128(void* dst, const void* src, size_t size)
{
    __m128i* cdst = static_cast<__m128i*>(std::assume_aligned<sizeof(__m128i)>(dst));
    const __m128i* csrc = static_cast<const __m128i*>(std::assume_aligned<sizeof(__m128i)>(src));

    do
    {
        _mm_store_si128(cdst, _mm_load_si128(csrc));
        cdst++;
        size -= 16;
    } while (size >= 16);
}
} // namespace

void CRenderTarget::phase_ssfx_sss_ext(light_Package& LP)
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    static constexpr const char* strLights{"lights_data"};
    static __declspec(align(sizeof(__m128i))) light* LightSlot[8];
    static u32 sss_currentframe;

    if (Device.dwPrecacheFrame > 2) [[unlikely]]
    {
        memset(LightSlot, 0, sizeof(LightSlot));
        sss_currentframe = 0;
    }

    // Constants
    constexpr u32 C{color_rgba(255, 255, 255, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    u_setrt(cmd_list, rt_ssfx_sss_tmp, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss_ext->E[0]);

    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);
    cmd_list.set_c("ssfx_sss", ps_ssfx_sss);
    cmd_list.set_c("id_offset", 0);

    Fvector4* Lights_Array{nullptr};
    cmd_list.get_ConstantDirect(strLights, 4 * sizeof(Fvector4) * 2, nullptr, nullptr, reinterpret_cast<void**>(&Lights_Array));

    memset128(XR_ASSERT_VAL(Lights_Array != nullptr), zeros, 8 * sizeof(*Lights_Array));

    xr_vector<light*> LightsSort;
    bool CheckPackage = true;

    if (Device.dwFrame > sss_currentframe)
    {
        sss_currentframe = Device.dwFrame + 2;

        xr_vector<light*>& source = LP.v_shadowed;
        for (u32 it = 0; it < source.size(); it++)
        {
            light* L = source[it];

            if (L->omnipart_num == 0 && L->range > 1.5f)
            {
                if (L->distance < 800 && L->flags.bActive)
                {
                    L->distance_lpos = Device.vCameraPosition.distance_to(L->position);

                    if (L->distance_lpos <= L->range)
                        L->sss_priority = 0;
                    else
                        L->sss_priority = 1;

                    LightsSort.push_back(L);
                }
            }

            // Refresh hierarchy ( Look for a better way? )
            if (L->sss_refresh)
            {
                L->sss_refresh = false;
                int done = 0;

                for (u32 lit = 0; lit < source.size(); lit++)
                {
                    light* L2 = source[lit];
                    if (L2->omipart_parent == L->omipart_parent)
                    {
                        L2->sss_id = L->sss_id;
                        done++;
                        if (done >= 6)
                            break; // Update done.
                    }
                }
            }
        }

        // Sort Distance
        std::ranges::sort(LightsSort, [](const light* i, const light* j) { return (i->distance < j->distance && i->sss_priority < j->sss_priority); });

        for (auto L : LightsSort)
        {
            bool Add = true;
            int FreeSlot = -1;

            for (int slot = 0; slot < 8; slot++)
            {
                if (LightSlot[slot])
                {
                    if (LightSlot[slot] == L)
                    {
                        Add = false;
                        break;
                    }
                }
                else
                    FreeSlot = slot;
            }

            if (Add && FreeSlot > -1)
            {
                LightSlot[FreeSlot] = L;

                L->sss_id = FreeSlot;

                if (L->flags.type == IRender_Light::OMNIPART)
                    L->sss_refresh = true;
            }
        }
    }
    else
    {
        // Don't check the sorted package when the frame is skipped
        CheckPackage = false;
    }

    for (int slot = 0; slot < 8; slot++)
    {
        if (LightSlot[slot])
        {
            // Check if the light still exist on the sorted Light Package
            bool Remove = true;

            if (CheckPackage)
            {
                for (auto L : LightsSort)
                {
                    if (L == LightSlot[slot])
                        Remove = false;
                }
            }
            else
            {
                // The distance calc was skipped, check here instead
                LightSlot[slot]->distance_lpos = Device.vCameraPosition.distance_to(LightSlot[slot]->position);
                Remove = false;
            }

            float Dist = LightSlot[slot]->distance_lpos;

            if (Dist > (LightSlot[slot]->range * 2.0f))
                Remove = true;

            // Remove Light
            if (!LightSlot[slot]->flags.bActive || Remove)
            {
                if (LightSlot[slot]->flags.type == IRender_Light::OMNIPART)
                    LightSlot[slot]->sss_refresh = true;

                LightSlot[slot]->sss_id = -1;
                LightSlot[slot] = nullptr;
            }
            else
            {
                // Update Light
                Fvector L_pos;

                Device.mView.transform_tiny(L_pos, LightSlot[slot]->position);

                // Distance Atte ( Use MaxAtte if the light range is bigger than the max sort range )
                float MaxAtte = 1.0f - (clampr((LightSlot[slot]->distance - 780) / -100, 0.f, 1.f));
                float Atte = 1.0f - (clampr((Dist - LightSlot[slot]->range * 1.9f) / -(LightSlot[slot]->range / 2.0f), 0.f, 1.f));

                // ( Reminder ) The value is inverted ( 1.0 = Fadeout ~ 0.0 = Full Visible )
                Lights_Array[slot].set(L_pos, std::max(MaxAtte, Atte));
            }
        }
    }

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    auto pContext = cmd_list.context();
    pContext->CopyResource(rt_ssfx_sss_ext->pTexture->surface_get(), rt_ssfx_sss_tmp->pTexture->surface_get());

    // SSS Ext 2 -------------------------------------------------------

    u_setrt(cmd_list, rt_ssfx_sss_tmp, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss_ext->E[1]);

    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);
    cmd_list.set_c("id_offset", 1);

    // Invalidate constant cache just in case
    Lights_Array = nullptr;
    cmd_list.get_ConstantDirect(strLights, 4 * sizeof(Fvector4) * 2, nullptr, nullptr, reinterpret_cast<void**>(&Lights_Array));

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    pContext->CopyResource(rt_ssfx_sss_ext2->pTexture->surface_get(), rt_ssfx_sss_tmp->pTexture->surface_get());

    // Combine ---------------------------------------------------------

    u_setrt(cmd_list, rt_ssfx_sss_tmp, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_sss_ext->E[2]);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}
