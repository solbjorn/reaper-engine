#include "stdafx.h"

void CRenderTarget::RenderScreenQuad(const u32 w, const u32 h, ID3DRenderTargetView* rt, ref_selement& sh, string_unordered_map<const char*, Fvector4*>* consts)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    if (rt)
        u_setrt(cmd_list, w, h, rt, nullptr, nullptr, get_base_zb());

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Half-pixel offset (DX9 only)
    constexpr Fvector2 p0{0.0f, 0.0f}, p1{1.0f, 1.0f};

    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);

    const auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    cmd_list.set_Element(sh);

    if (consts)
    {
        for (const auto& [k, v] : *consts)
            cmd_list.set_c(k, *v);
    }

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::RenderScreenQuad(const u32 w, const u32 h, ref_rt& rt, ref_selement& sh, string_unordered_map<const char*, Fvector4*>* consts)
{
    RenderScreenQuad(w, h, rt ? rt->pRT : nullptr, sh, consts);
}
