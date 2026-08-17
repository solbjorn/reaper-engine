#include "stdafx.h"

void CRenderTarget::phase_ssfx_ao()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    float ScaleFactor = std::min(std::max(ps_ssfx_ao.x, 1.0f), 8.0f);

    Fvector2 p0, p1;
    p0.set(0.0f, 0.0f);
    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    float scale_X = w / ScaleFactor;
    float scale_Y = h / ScaleFactor;

    // AO ///////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    cmd_list.set_viewport_size(scale_X, scale_Y);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Set pass
    cmd_list.set_Element(s_ssfx_ao->E[0]);
    cmd_list.set_c("ao_setup", ps_ssfx_ao);
    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Save AO frame
    cmd_list.context()->CopyResource(rt_ssfx_ao->pTexture->surface_get(), rt_ssfx_temp->pTexture->surface_get());

    // scale_X = w / (ScaleFactor * 2.0f);
    // scale_Y = h / (ScaleFactor * 2.0f);

    p1.set(1.0f, 1.0f);
    cmd_list.set_viewport_size(w, h);

    // BLUR PHASE 1 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp3, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[1]);
    cmd_list.set_c("blur_setup", ps_ssfx_ao.x, 0.25f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_ao);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 2 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[2]);
    cmd_list.set_c("blur_setup", 1.f, 0.5f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_ao);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 3 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp3, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[1]);
    cmd_list.set_c("blur_setup", 1.f, 0.75f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_ao);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 4 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[2]);
    cmd_list.set_c("blur_setup", 1.f, 1.f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_ao);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.set_viewport_size(w, h);
}

void CRenderTarget::phase_ssfx_il()
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // Constants
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    float ScaleFactor = std::min(std::max(ps_ssfx_il.x, 1.0f), 8.0f);

    Fvector2 p0, p1;
    p0.set(0.0f, 0.0f);
    p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);

    // Fill VB
    float scale_X = w / ScaleFactor;
    float scale_Y = h / ScaleFactor;

    // AO ///////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    cmd_list.set_viewport_size(scale_X, scale_Y);

    // Fill vertex buffer
    auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    auto Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Set pass
    cmd_list.set_Element(s_ssfx_ao->E[3]);
    cmd_list.set_c("ao_setup", ps_ssfx_il);
    cmd_list.set_c("m_current", Matrix_current);
    cmd_list.set_c("m_previous", Matrix_previous);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Save AO frame
    cmd_list.context()->CopyResource(rt_ssfx_il->pTexture->surface_get(), rt_ssfx_temp2->pTexture->surface_get());

    // scale_X = w / ScaleFactor;
    // scale_Y = h / ScaleFactor;

    // p1.set(1.0f / ScaleFactor, 1.0f / ScaleFactor);
    // cmd_list.set_viewport_size(scale_X, scale_Y);

    // BLUR PHASE 1 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp3, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[4]);
    cmd_list.set_c("blur_setup", 1.f, 0.25f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_il);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 2 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[5]);
    cmd_list.set_c("blur_setup", 1.f, 0.5f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_il);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 3 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp3, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[4]);
    cmd_list.set_c("blur_setup", 1.f, 0.75f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_il);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLUR PHASE 4 //////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_ssfx_temp2, {}, {}, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(0, h, d_Z, d_W, C, p0.x, p1.y);
    verts[1].set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    verts[2].set(w, h, d_Z, d_W, C, p1.x, p1.y);
    verts[3].set(w, 0, d_Z, d_W, C, p1.x, p0.y);

    Offset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Draw COLOR
    cmd_list.set_Element(s_ssfx_ao->E[5]);
    cmd_list.set_c("blur_setup", 1.f, 1.f, scale_X, scale_Y);
    cmd_list.set_c("ao_setup", ps_ssfx_il);

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    cmd_list.set_viewport_size(w, h);
}
