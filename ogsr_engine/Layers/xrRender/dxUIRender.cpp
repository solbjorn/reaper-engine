#include "stdafx.h"

#include "dxUIRender.h"

#include "dxUIShader.h"

dxUIRender UIRenderImpl;

void dxUIRender::CreateUIGeom()
{
    hGeom_TL.create(FVF::F_TL, SGeometry::default_vb(), nullptr);
    XR_ASSERT(hGeom_TL.stride() == sizeof(FVF::TL));

    hGeom_LIT.create(FVF::F_LIT, SGeometry::default_vb(), nullptr);
    XR_ASSERT(hGeom_LIT.stride() == sizeof(FVF::LIT));
}

void dxUIRender::DestroyUIGeom()
{
    for (auto& it : g_UIShadersCache)
        it.second.destroy();

    g_UIShadersCache.clear();

    hGeom_TL._set(nullptr);
    hGeom_LIT._set(nullptr);
}

void dxUIRender::SetShader(IUIShader& shader)
{
    auto pShader = XR_ASSERT_VAL(smart_cast<dxUIShader*>(&shader) != nullptr);
    RImplementation.get_imm_context().cmd_list.set_Shader(XR_ASSERT_VAL(pShader->hShader));
}

void dxUIRender::SetScissor(Irect* rect)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    cmd_list.set_Scissor(rect);
    cmd_list.StateManager.OverrideScissoring(rect ? true : false, TRUE);
}

void dxUIRender::GetActiveTextureResolution(Fvector2& res)
{
    const auto T = RImplementation.get_imm_context().cmd_list.get_ActiveTexture(0);
    res.set(gsl::narrow_cast<f32>(T->get_Width()), gsl::narrow_cast<f32>(T->get_Height()));
}

void dxUIRender::PushPoint(float x, float y, float z, u32 C, float u, float v)
{
    switch (m_PointType)
    {
    case pttLIT:
        lit_verts[lit_written].set(x, y, z, C, u, v);
        ++lit_written;
        break;
    case pttTL:
        tl_verts[tl_written].set(x, y, C, u, v);
        ++tl_written;
        break;
    default: xr::unreachable();
    }
}

void dxUIRender::StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType)
{
    XR_ASSERT(PrimitiveType == ptNone && m_PointType == pttNone, "", PrimitiveType, m_PointType);

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    m_iMaxVerts = iMaxVerts;
    PrimitiveType = primType;
    m_PointType = pointType;

    switch (pointType)
    {
    case pttLIT:
        lit_verts = cmd_list.Vertex.Lock<FVF::LIT>(m_iMaxVerts);
        lit_written = 0;
        break;
    case pttTL:
        tl_verts = cmd_list.Vertex.Lock<FVF::TL>(m_iMaxVerts);
        tl_written = 0;
        break;
    default: xr::unreachable();
    }
}

void dxUIRender::FlushPrimitive()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    std::size_t vOffset;
    std::size_t p_cnt;

    switch (m_PointType)
    {
    case pttLIT:
        p_cnt = XR_ASSERT_VAL(lit_written <= m_iMaxVerts);
        vOffset = cmd_list.Vertex.Unlock<FVF::LIT>(p_cnt);

        cmd_list.set_Geometry(hGeom_LIT);
        break;
    case pttTL:
        p_cnt = XR_ASSERT_VAL(tl_written <= m_iMaxVerts);
        vOffset = cmd_list.Vertex.Unlock<FVF::TL>(p_cnt);

        cmd_list.set_Geometry(hGeom_TL);
        break;
    default: xr::unreachable();
    }

    //	Update data for primitive type
    _D3DPRIMITIVETYPE d3dPrimType;
    gsl::index primCount;

    switch (PrimitiveType)
    {
    case ptTriStrip:
        primCount = gsl::narrow_cast<gsl::index>(p_cnt) - 2;
        d3dPrimType = D3DPT_TRIANGLESTRIP;
        break;
    case ptTriList:
        primCount = gsl::narrow_cast<gsl::index>(p_cnt) / 3;
        d3dPrimType = D3DPT_TRIANGLELIST;
        break;
    case ptLineStrip:
        primCount = gsl::narrow_cast<gsl::index>(p_cnt) - 1;
        d3dPrimType = D3DPT_LINESTRIP;
        break;
    case ptLineList:
        primCount = gsl::narrow_cast<gsl::index>(p_cnt) / 2;
        d3dPrimType = D3DPT_LINELIST;
        break;
    default: xr::unreachable();
    }

    if (primCount > 0)
        cmd_list.Render(d3dPrimType, vOffset, gsl::narrow_cast<std::size_t>(primCount));

    PrimitiveType = ptNone;
    m_PointType = pttNone;
}

void dxUIRender::CacheSetXformWorld(const Fmatrix& M) { RImplementation.get_imm_context().cmd_list.set_xform_world(M); }
void dxUIRender::CacheSetCullMode(CullMode m) { RImplementation.get_imm_context().cmd_list.set_CullMode(CULL_NONE + m); }
