#include "stdafx.h"

void CRenderTarget::phase_gasmask_drops()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C{color_rgba(0, 0, 0, 255)};

    constexpr f32 d_Z{EPS_S};
    constexpr f32 d_W{1.0f};
    const f32 w = gsl::narrow_cast<f32>(Device.dwWidth);
    const f32 h = gsl::narrow_cast<f32>(Device.dwHeight);

    constexpr Fvector2 p0{0.f, 0.f}, p1{1.f, 1.f};

    //////////////////////////////////////////////////////////////////////////
    // Set MSAA/NonMSAA rendertarget
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
    u_setrt(cmd_list, dest_rt, {}, {}, nullptr);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Set pass
    cmd_list.set_Element(s_gasmask_drops->E[0]);

    // Set parameters
    cmd_list.set_c("drops_control", ps_r2_drops_control.x, ps_r2_drops_control.y, ps_r2_drops_control.z, 0.f);
    cmd_list.set_c("mask_control", ps_r2_mask_control);

    // Set geometry
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.context()->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
}
