#include "stdafx.h"

void CRenderTarget::ProcessSMAA()
{
    u32 Offset;
    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    const float _w = float(Device.dwWidth);
    const float _h = float(Device.dwHeight);

    // Half-pixel offset (DX9 only)
    constexpr Fvector2 p0{0.0f, 0.0f}, p1{1.0f, 1.0f};

    // Phase 0: edge detection ////////////////////////////////////////////////
    u_setrt(RCache, rt_smaa_edgetex, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    for (u32 i = 0; i < 3; i++)
        RCache.ClearRT(RCache.get_RT(i), {});

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(EPS, _h + EPS, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(_w + EPS, _h + EPS, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(_w + EPS, EPS, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_pp_antialiasing->E[2]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Phase 1: blend weights calculation ////////////////////////////////////
    u_setrt(RCache, rt_smaa_blendtex, nullptr, nullptr, nullptr);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(TRUE, D3DCMP_EQUAL, 0x1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    for (u32 i = 0; i < 3; i++)
        RCache.ClearRT(RCache.get_RT(i), {});

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(EPS, _h + EPS, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(_w + EPS, _h + EPS, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(_w + EPS, EPS, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_pp_antialiasing->E[3]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Phase 2: neighbour blend //////////////////////////////////////////////
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
    u_setrt(RCache, dest_rt, nullptr, nullptr, nullptr);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(EPS, _h + EPS, d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(_w + EPS, _h + EPS, d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(_w + EPS, EPS, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_pp_antialiasing->E[4]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    RCache.context()->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
}
