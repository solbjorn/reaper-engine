#include "stdafx.h"

void CRenderTarget::phase_gasmask_dudv()
{
    XR_TRACY_ZONE_SCOPED();

    // Constants
    u32 Offset{};
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.0f, 0.0f}, p1{1.0f, 1.0f};

    //////////////////////////////////////////////////////////////////////////
    // Set MSAA/NonMSAA rendertarget
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
    u_setrt(RCache, dest_rt, {}, {}, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

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
    RCache.set_Element(s_gasmask_dudv->E[0]);

    // Set parameters
    RCache.set_c("mask_control", ps_r2_mask_control);

    // Set geometry
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.context()->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
}
