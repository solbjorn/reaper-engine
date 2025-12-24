#include "stdafx.h"

void CRenderTarget::mark_msaa_edges()
{
    XR_TRACY_ZONE_SCOPED();

    u32 Offset{};
    constexpr f32 d_Z{EPS_S}, d_W{1.0f};
    constexpr u32 C{color_rgba(255, 255, 255, 255)};

    // Fill vertex buffer
    FVF::TL2uv* pv = (FVF::TL2uv*)RImplementation.Vertex.Lock(4, g_combine_2UV->vb_stride, Offset);
    pv->set(-1, -1, 0, d_W, C, 0, 1, 0, 0);
    pv++;
    pv->set(-1, 1, d_Z, d_W, C, 0, 0, 0, 0);
    pv++;
    pv->set(1, -1, d_Z, d_W, C, 1, 1, 0, 0);
    pv++;
    pv->set(1, 1, d_Z, d_W, C, 1, 0, 0, 0);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine_2UV->vb_stride);

    u_setrt(RCache, {}, {}, {}, rt_MSAADepth);
    RCache.set_Element(s_mark_msaa_edges->E[0]);
    RCache.set_Geometry(g_combine_2UV);
    RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x80, 0xFF, 0x80, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
    RCache.set_ColorWriteEnable(false);
    RCache.set_ZFunc(D3DCMP_ALWAYS);
    RCache.set_Z(false);
    RCache.set_CullMode(D3DCULL_NONE);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    RCache.set_ColorWriteEnable(D3D_COLOR_WRITE_ENABLE_ALL);
}
