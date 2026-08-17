#include "stdafx.h"

void CRenderTarget::mark_msaa_edges(CBackend& cmd_list)
{
    XR_TRACY_ZONE_SCOPED();

    constexpr f32 d_Z{EPS_S}, d_W{1.0f};
    constexpr u32 C{color_rgba(255, 255, 255, 255)};

    // Fill vertex buffer
    const auto verts = cmd_list.Vertex.Lock<FVF::TL2uv>(4);

    verts[0].set(-1, -1, 0, d_W, C, 0, 1, 0, 0);
    verts[1].set(-1, 1, d_Z, d_W, C, 0, 0, 0, 0);
    verts[2].set(1, -1, d_Z, d_W, C, 1, 1, 0, 0);
    verts[3].set(1, 1, d_Z, d_W, C, 1, 0, 0, 0);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL2uv>(4);

    u_setrt(cmd_list, {}, {}, {}, rt_MSAADepth);
    cmd_list.set_Element(s_mark_msaa_edges->E[0]);
    cmd_list.set_Geometry(g_combine_2UV);
    cmd_list.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x80, 0xFF, 0x80, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
    cmd_list.set_ColorWriteEnable(false);
    cmd_list.set_ZFunc(D3DCMP_ALWAYS);
    cmd_list.set_Z(false);
    cmd_list.set_CullMode(D3DCULL_NONE);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    cmd_list.set_ColorWriteEnable(D3D_COLOR_WRITE_ENABLE_ALL);
}
