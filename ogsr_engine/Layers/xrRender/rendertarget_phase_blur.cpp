#include "stdafx.h"

void CRenderTarget::phase_blur()
{
    // Get common data
    u32 Offset = 0;
    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    // Full resolution
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.5f;
    h = float(Device.dwHeight) * 0.5f;

    u_setrt(rt_blur_h_2, 0, 0, rt_blur_2_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[0]);
    RCache.set_c("blur_params", 1.f, 0.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(rt_blur_2, 0, 0, rt_blur_2_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[1]);
    RCache.set_c("blur_params", 0.f, 1.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur / Half res
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.25f;
    h = float(Device.dwHeight) * 0.25f;

    u_setrt(rt_blur_h_4, 0, 0, rt_blur_4_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[2]);
    RCache.set_c("blur_params", 1.f, 0.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(rt_blur_4, 0, 0, rt_blur_4_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[3]);
    RCache.set_c("blur_params", 0.f, 1.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.125f;
    h = float(Device.dwHeight) * 0.125f;

    u_setrt(rt_blur_h_8, 0, 0, rt_blur_8_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[4]);
    RCache.set_c("blur_params", 1.f, 0.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(rt_blur_8, 0, 0, rt_blur_8_zb->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_blur->E[5]);
    RCache.set_c("blur_params", 0.f, 1.f, w, h);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
};

void CRenderTarget::phase_ssfx_ssr()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    float ScaleFactor = std::min(std::max(ps_ssfx_ssr.x, 1.0f), 2.0f);

    constexpr Fvector2 p0{0.f, 0.f};
    Fvector2 p1{1.f, 1.f};

    // GLOSS /////////////////////////////////////////////////////////////////
    u_setrt(rt_ssfx_temp3, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ssr->E[5]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////

    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    float scale_X = w / ScaleFactor;
    float scale_Y = h / ScaleFactor;

    // SSR ///////////////////////////////////////////////////////////
    u_setrt(rt_ssfx, 0, 0, HW.pBaseZB);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    if (ScaleFactor > 1.0f)
        set_viewport_size(HW.pContext, scale_X, scale_Y);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Set pass
    RCache.set_Element(s_ssfx_ssr->E[0]);
    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);
    RCache.set_c("cam_pos", ::Random.randF(-1.0, 1.0), ::Random.randF(-1.0, 1.0), 0.f, 0.f);

    RCache.set_c("ssr_setup", ps_ssfx_ssr);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // COPY SSR RESULT ( ACC ) ////////////////////////////////////////////
    HW.pContext->CopyResource(rt_ssfx_ssr->pTexture->surface_get(), rt_ssfx->pTexture->surface_get());

    // Disable/Enable Blur if the value is <= 0
    // if (ps_ssfx_ssr.y > 0 || ps_ssfx_ssr.x > 1.0)
    {
        // BLUR PHASE 1 //////////////////////////////////////////////////////////
        u_setrt(rt_ssfx_temp, 0, 0, HW.pBaseZB);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_ssr->E[1]);
        RCache.set_c("blur_params", 1.f, 0.f, scale_X, scale_Y);
        RCache.set_c("ssr_setup", ps_ssfx_ssr);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

        // BLUR PHASE 2 //////////////////////////////////////////////////////////
        u_setrt(rt_ssfx_temp2, 0, 0, HW.pBaseZB);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_ssr->E[2]);
        RCache.set_c("blur_params", 0.f, 1.f, w, h);
        RCache.set_c("ssr_setup", ps_ssfx_ssr);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // COMBINE //////////////////////////////////////////////////////////
    // Reset Viewport
    if (ScaleFactor > 1.0f)
        set_viewport_size(HW.pContext, w, h);

    p1.set(1.0f, 1.0f);

    if (!RImplementation.o.dx10_msaa)
        u_setrt(rt_Generic_0, nullptr, nullptr, nullptr);
    else
        u_setrt(rt_Generic_0_r, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ssr->E[3]);
    RCache.set_c("ssr_setup", ps_ssfx_ssr);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
};

void CRenderTarget::phase_ssfx_volumetric_blur()
{
    // Be careful and clear the buffer ( rt_Generic_2 contain unspeakable stuff if no volumetric is written )
    if (!m_bHasActiveVolumetric)
    {
        constexpr FLOAT ColorRGBA[4] = {0.0, 0.0, 0.0, 0.0};
        HW.pContext->ClearRenderTargetView(rt_Generic_2->pRT, ColorRGBA);
    }

    if (!m_bHasActiveVolumetric_spot)
    {
        constexpr FLOAT ColorRGBA[4] = {0.0, 0.0, 0.0, 0.0};
        HW.pContext->ClearRenderTargetView(rt_ssfx_volumetric->pRT, ColorRGBA);
        return;
    }

    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    FVF::TL* pv;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    // Volumetric always at 1/8 res
    set_viewport_size(HW.pContext, w / 8, h / 8);

    ref_rt* rt_VolBlur[2] = {&rt_ssfx_volumetric_tmp, &rt_ssfx_volumetric};
    constexpr float pixelsize[4] = {0, 1, 1, 2}; // half pixel + pixelsize
    constexpr float pixelscale[4] = {2.0f, 0.5f, 2.0f, 0.5f};

    // BLUR ///////////////////////////////////////////////////////////////////
    for (int b = 0; b < 4; b++)
    {
        u_setrt(*rt_VolBlur[b % 2], 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
        pv++;
        pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
        pv++;
        pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
        pv++;
        pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_volumetric_blur->E[b % 2]);
        RCache.set_c("blur_setup", w / 8, h / 8, pixelsize[b], pixelscale[b]);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Restore Viewport
    set_viewport_size(HW.pContext, w, h);

    // COMBINE ////////////////////////////////////////////////////////////////
    u_setrt(rt_ssfx_accum, 0, 0, NULL);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
    pv++;
    pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
    pv++;
    pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
    pv++;
    pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_volumetric_blur->E[5]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    HW.pContext->CopyResource(rt_Generic_2->pTexture->surface_get(), rt_ssfx_accum->pTexture->surface_get());
};

void CRenderTarget::phase_ssfx_water_blur()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f};
    Fvector2 p1{0.5f, 0.5f};

    set_viewport_size(HW.pContext, w / 2, h / 2);

    if (ps_ssfx_water.y > 0)
    {
        // BLUR PHASE 1 //////////////////////////////////////////////////////////
        u_setrt(rt_ssfx_temp2, 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_water_blur->E[0]);
        RCache.set_c("blur_setup", 1.f, 0.f, 0.f, 2.0f / ps_ssfx_water.x);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

        // BLUR PHASE 2 //////////////////////////////////////////////////////////
        u_setrt(rt_ssfx_temp, 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_water_blur->E[1]);
        RCache.set_c("blur_setup", 0.f, 1.f, 0.f, 1.f);

        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
    else
    {
        HW.pContext->CopyResource(rt_ssfx_temp2->pTexture->surface_get(), rt_ssfx_temp->pTexture->surface_get());

        u_setrt(rt_ssfx_temp, 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_water_blur->E[2]);
        RCache.set_c("blur_setup", 0.f, 0.f, 0.f, 2.0f / ps_ssfx_water.x);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    set_viewport_size(HW.pContext, w, h);
    p1.set(1.0f, 1.0f);
};

void CRenderTarget::phase_ssfx_water_waves()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    u32 w = Device.dwWidth;
    u32 h = Device.dwHeight;

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    set_viewport_size(HW.pContext, 512, 512);

    u_setrt(rt_ssfx_water_waves, 0, 0, NULL);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_water_blur->E[5]);
    RCache.set_c("wind_setup", g_pGamePersistent->Environment().wind_anim.w, g_pGamePersistent->Environment().CurrentEnv->wind_velocity, 0.f, 0.f);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    set_viewport_size(HW.pContext, w, h);
};

void CRenderTarget::phase_ssfx_sss()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(255, 255, 255, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    u_setrt(rt_ssfx, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss->E[0]);

    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);
    RCache.set_c("ssfx_sss", ps_ssfx_sss);

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR
    u_setrt(rt_ssfx_temp, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss->E[1]);

    RCache.set_c("blur_setup", 0.f, 1.f, 0.f, 0.f);

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR
    u_setrt(rt_ssfx_temp2, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss->E[2]);

    RCache.set_c("blur_setup", 1.f, 0.f, 2.f, 0.f);

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    HW.pContext->CopyResource(rt_ssfx_sss->pTexture->surface_get(), rt_ssfx_temp2->pTexture->surface_get());
}

static constexpr ptrdiff_t __declspec(align(16)) zeros[16 / sizeof(ptrdiff_t)]{};

static ICF void memset128(void* dst, const void* src, size_t size)
{
    __m128i* cdst = reinterpret_cast<__m128i*>(dst);
    const __m128i* csrc = reinterpret_cast<const __m128i*>(src);

    do
    {
        _mm_store_si128(cdst, _mm_load_si128(csrc));
        cdst++;
        size -= 16;
    } while (size >= 16);
}

void CRenderTarget::phase_ssfx_sss_ext(light_Package& LP)
{
    constexpr const char* strLights{"lights_data"};
    static __declspec(align(16)) light* LightSlot[8];
    static u32 sss_currentframe;

    if (Device.dwPrecacheFrame > 2) [[unlikely]]
    {
        memset(LightSlot, 0, sizeof(LightSlot));
        sss_currentframe = 0;
    }

    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(255, 255, 255, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = Device.dwWidth;
    float h = Device.dwHeight;

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    u_setrt(rt_ssfx_sss_tmp, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss_ext->E[0]);

    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);
    RCache.set_c("ssfx_sss", ps_ssfx_sss);
    RCache.set_c("id_offset", 0);

    Fvector4* Lights_Array{};
    RCache.get_ConstantDirect(strLights, 4 * sizeof(Fvector4) * 2, nullptr, nullptr, reinterpret_cast<void**>(&Lights_Array));
    VERIFY(Lights_Array);

    if (Lights_Array)
    {
        memset128(Lights_Array, zeros, 8 * sizeof(*Lights_Array));

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

            for (int x = 0; x < LightsSort.size(); x++)
            {
                light* L = LightsSort[x];
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
                    for (int x = 0; x < LightsSort.size(); x++)
                    {
                        light* L = LightsSort[x];

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
                    LightSlot[slot] = NULL;
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
    }

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    HW.pContext->CopyResource(rt_ssfx_sss_ext->pTexture->surface_get(), rt_ssfx_sss_tmp->pTexture->surface_get());

    // SSS Ext 2 -------------------------------------------------------

    u_setrt(rt_ssfx_sss_tmp, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss_ext->E[1]);

    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);
    RCache.set_c("id_offset", 1);
    Lights_Array = nullptr;
    RCache.get_ConstantDirect(strLights, 4 * sizeof(Fvector4) * 2, nullptr, nullptr, reinterpret_cast<void**>(&Lights_Array));

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    HW.pContext->CopyResource(rt_ssfx_sss_ext2->pTexture->surface_get(), rt_ssfx_sss_tmp->pTexture->surface_get());

    // Combine ---------------------------------------------------------

    u_setrt(rt_ssfx_sss_tmp, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_sss_ext->E[2]);

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}
