#include "stdafx.h"

void CRenderTarget::phase_rain(CBackend& cmd_list)
{
    u_setrt(cmd_list, rt_Color, {}, {}, rt_MSAADepth);
    RImplementation.rmNormal(cmd_list);
}

void CRenderTarget::phase_ssfx_rain()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    cmd_list.set_viewport_size(w / 8.0f, h / 8.0f);

    u_setrt(cmd_list, rt_ssfx_rain, {}, {}, nullptr);
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
    cmd_list.set_Element(!RImplementation.o.dx10_msaa ? s_ssfx_rain->E[0] : s_ssfx_rain->E[1]);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.set_viewport_size(w, h);
}
