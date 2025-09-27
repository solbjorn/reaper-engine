#include "stdafx.h"

void CRenderTarget::phase_ssfx_ao()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    float ScaleFactor = std::min(std::max(ps_ssfx_ao.x, 1.0f), 8.0f);

    Fvector2 p0, p1;
    p0.set(0.0f, 0.0f);
    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    float scale_X = w / ScaleFactor;
    float scale_Y = h / ScaleFactor;

    // AO ///////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    RCache.set_viewport_size(scale_X, scale_Y);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Set pass
    RCache.set_Element(s_ssfx_ao->E[0]);
    RCache.set_c("ao_setup", ps_ssfx_ao);

    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Save AO frame
    RCache.context()->CopyResource(rt_ssfx_ao->pTexture->surface_get(), rt_ssfx_temp->pTexture->surface_get());

    // scale_X = w / (ScaleFactor * 2.0f);
    // scale_Y = h / (ScaleFactor * 2.0f);

    p1.set(1.0f, 1.0f);
    RCache.set_viewport_size(w, h);

    // BLUR PHASE 1 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp3, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[1]);
    RCache.set_c("blur_setup", ps_ssfx_ao.x, 0.25f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_ao);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 2 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[2]);
    RCache.set_c("blur_setup", 1.f, 0.5f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_ao);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 3 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp3, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[1]);
    RCache.set_c("blur_setup", 1.f, 0.75f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_ao);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 4 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[2]);
    RCache.set_c("blur_setup", 1.f, 1.f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_ao);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.set_viewport_size(w, h);
}

void CRenderTarget::phase_ssfx_il()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    float ScaleFactor = std::min(std::max(ps_ssfx_il.x, 1.0f), 8.0f);

    Fvector2 p0, p1;
    p0.set(0.0f, 0.0f);
    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    float scale_X = w / ScaleFactor;
    float scale_Y = h / ScaleFactor;

    // AO ///////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp2, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    RCache.set_viewport_size(scale_X, scale_Y);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Set pass
    RCache.set_Element(s_ssfx_ao->E[3]);
    RCache.set_c("ao_setup", ps_ssfx_il);
    RCache.set_c("m_current", Matrix_current);
    RCache.set_c("m_previous", Matrix_previous);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Save AO frame
    RCache.context()->CopyResource(rt_ssfx_il->pTexture->surface_get(), rt_ssfx_temp2->pTexture->surface_get());

    // scale_X = w / ScaleFactor;
    // scale_Y = h / ScaleFactor;

    // p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);
    // RCache.set_viewport_size(scale_X, scale_Y);

    // BLUR PHASE 1 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp3, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[4]);
    RCache.set_c("blur_setup", 1.f, 0.25f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_il);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 2 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp2, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[5]);
    RCache.set_c("blur_setup", 1.f, 0.5f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_il);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 3 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp3, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[4]);
    RCache.set_c("blur_setup", 1.f, 0.75f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_il);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 4 //////////////////////////////////////////////////////////
    u_setrt(RCache, rt_ssfx_temp2, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_ao->E[5]);
    RCache.set_c("blur_setup", 1.f, 1.f, scale_X, scale_Y);
    RCache.set_c("ao_setup", ps_ssfx_il);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.set_viewport_size(w, h);
}
