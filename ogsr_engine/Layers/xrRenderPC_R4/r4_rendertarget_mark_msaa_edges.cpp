#include "stdafx.h"

void CRenderTarget::mark_msaa_edges()
{
    u32 Offset;
    constexpr float d_Z = EPS_S, d_W = 1.f;
    constexpr u32 C = color_rgba(255, 255, 255, 255);

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
    u_setrt(RCache, NULL, NULL, NULL, rt_MSAADepth);
    RCache.set_Element(s_mark_msaa_edges->E[0]);
    RCache.set_Geometry(g_combine_2UV);
    RCache.StateManager.SetStencil(TRUE, D3DCMP_ALWAYS, 0x80, 0xFF, 0x80, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
    RCache.StateManager.SetColorWriteEnable(0);
    RCache.StateManager.SetDepthFunc(D3DCMP_ALWAYS);
    RCache.StateManager.SetDepthEnable(FALSE);
    RCache.StateManager.SetCullMode(D3DCULL_NONE);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    RCache.StateManager.SetColorWriteEnable(D3D_COLOR_WRITE_ENABLE_ALL);
}
