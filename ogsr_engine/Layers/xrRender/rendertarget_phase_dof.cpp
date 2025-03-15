#include "stdafx.h"

void CRenderTarget::phase_dof()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    //////////////////////////////////////////////////////////////////////////
    // Set MSAA/NonMSAA rendertarget
    u_setrt(rt_dof, 0, 0, HW.pBaseZB);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Set pass
    RCache.set_Element(s_dof->E[0]);

    // Set geometry
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ////////////////////////////////////////////////////////////////////////////
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
    u_setrt(dest_rt, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Set pass
    RCache.set_Element(s_dof->E[1]);

    // Set geometry
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Resolve RT
    HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
};
