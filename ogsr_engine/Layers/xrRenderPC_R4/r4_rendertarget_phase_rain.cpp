#include "stdafx.h"

void CRenderTarget::phase_rain()
{
    u_setrt(RCache, rt_Color, nullptr, nullptr, rt_MSAADepth);
    RImplementation.rmNormal(RCache);
}

void CRenderTarget::phase_ssfx_rain()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    RCache.set_viewport_size(w / 8.0f, h / 8.0f);

    u_setrt(RCache, rt_ssfx_rain, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
    pv++;
    pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
    pv++;
    pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
    pv++;
    pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(!RImplementation.o.dx10_msaa ? s_ssfx_rain->E[0] : s_ssfx_rain->E[1]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.set_viewport_size(w, h);
}
