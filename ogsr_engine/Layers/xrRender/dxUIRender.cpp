#include "stdafx.h"

#include "dxUIRender.h"

#include "dxUIShader.h"

dxUIRender UIRenderImpl;

void dxUIRender::CreateUIGeom()
{
    hGeom_TL.create(FVF::F_TL, RImplementation.Vertex.Buffer(), nullptr);
    hGeom_LIT.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), nullptr);
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
    RCache.set_Shader(XR_ASSERT_VAL(pShader->hShader));
}

void dxUIRender::SetScissor(Irect* rect)
{
    RCache.set_Scissor(rect);
    RCache.StateManager.OverrideScissoring(rect ? true : false, TRUE);
}

void dxUIRender::GetActiveTextureResolution(Fvector2& res)
{
    CTexture* T = RCache.get_ActiveTexture(0);
    res.set(gsl::narrow_cast<f32>(T->get_Width()), gsl::narrow_cast<f32>(T->get_Height()));
}

LPCSTR dxUIRender::UpdateShaderName(LPCSTR tex_name, LPCSTR sh_name)
{
    string_path buff;
    u32 v_dev = CAP_VERSION(HW.Caps.raster_major, HW.Caps.raster_minor);
    u32 v_need = CAP_VERSION(2, 0);

    if ((v_dev >= v_need) && FS.exist(buff, "$game_textures$", tex_name, ".ogm"))
        return "hud\\movie";
    else
        return sh_name;
}

void dxUIRender::PushPoint(float x, float y, float z, u32 C, float u, float v)
{
    switch (m_PointType)
    {
    case pttLIT:
        LIT_pv->set(x, y, z, C, u, v);
        ++LIT_pv;
        break;
    case pttTL:
        TL_pv->set(x, y, C, u, v);
        ++TL_pv;
        break;
    default: xr::unreachable();
    }
}

void dxUIRender::StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType)
{
    XR_ASSERT(PrimitiveType == ptNone && m_PointType == pttNone, "", PrimitiveType, m_PointType);

    m_iMaxVerts = iMaxVerts;
    PrimitiveType = primType;
    m_PointType = pointType;

    switch (pointType)
    {
    case pttLIT:
        LIT_start_pv = (FVF::LIT*)RImplementation.Vertex.Lock(m_iMaxVerts, hGeom_LIT.stride(), vOffset);
        LIT_pv = LIT_start_pv;
        break;
    case pttTL:
        TL_start_pv = (FVF::TL*)RImplementation.Vertex.Lock(m_iMaxVerts, hGeom_TL.stride(), vOffset);
        TL_pv = TL_start_pv;
        break;
    default: xr::unreachable();
    }
}

void dxUIRender::FlushPrimitive()
{
    gsl::index p_cnt;

    switch (m_PointType)
    {
    case pttLIT:
        p_cnt = XR_ASSERT_VAL(LIT_pv - LIT_start_pv <= m_iMaxVerts);
        RImplementation.Vertex.Unlock(p_cnt, hGeom_LIT.stride());

        RCache.set_Geometry(hGeom_LIT);
        break;
    case pttTL:
        p_cnt = XR_ASSERT_VAL(TL_pv - TL_start_pv <= m_iMaxVerts);
        RImplementation.Vertex.Unlock(p_cnt, hGeom_TL.stride());

        RCache.set_Geometry(hGeom_TL);
        break;
    default: xr::unreachable();
    }

    //	Update data for primitive type
    _D3DPRIMITIVETYPE d3dPrimType;
    gsl::index primCount;

    switch (PrimitiveType)
    {
    case ptTriStrip:
        primCount = p_cnt - 2;
        d3dPrimType = D3DPT_TRIANGLESTRIP;
        break;
    case ptTriList:
        primCount = p_cnt / 3;
        d3dPrimType = D3DPT_TRIANGLELIST;
        break;
    case ptLineStrip:
        primCount = p_cnt - 1;
        d3dPrimType = D3DPT_LINESTRIP;
        break;
    case ptLineList:
        primCount = p_cnt / 2;
        d3dPrimType = D3DPT_LINELIST;
        break;
    default: xr::unreachable();
    }

    if (primCount > 0)
        RCache.Render(d3dPrimType, vOffset, primCount);

    PrimitiveType = ptNone;
    m_PointType = pttNone;
}

void dxUIRender::CacheSetXformWorld(const Fmatrix& M) { RCache.set_xform_world(M); }
void dxUIRender::CacheSetCullMode(CullMode m) { RCache.set_CullMode(CULL_NONE + m); }
