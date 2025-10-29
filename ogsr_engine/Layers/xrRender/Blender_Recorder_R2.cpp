#include "stdafx.h"

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

void CBlender_Compile::r_Pass(LPCSTR _vs, LPCSTR _ps, bool bFog, BOOL bZtest, BOOL bZwrite, BOOL bABlend, D3DBLEND abSRC, D3DBLEND abDST, BOOL aTest, u32 aRef)
{
    RS.Invalidate();
    ctable.clear();
    passTextures.clear();
    dwStage = 0;

    // Setup FF-units (Z-buffer, blender)
    PassSET_ZB(bZtest, bZwrite);
    PassSET_Blend(bABlend, abSRC, abDST, aTest, aRef);
    PassSET_LightFog(FALSE, bFog);

    // Create shaders
    auto& res = *RImplementation.Resources;
    dest.ps = res._CreatePS(_ps);
    ctable.merge(&dest.ps->constants);

    dest.vs = res._CreateVS(_vs);
    ctable.merge(&dest.vs->constants);

    dest.gs = res._CreateGS("null");
    dest.hs = res._CreateHS("null");
    dest.ds = res._CreateDS("null");
    dest.cs = res._CreateCS("null");

    // Last Stage - disable
    if (std::is_eq(xr::strcasecmp(_ps, "null")))
    {
        RS.SetTSS(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RS.SetTSS(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
}

void CBlender_Compile::r_Constant(LPCSTR name, R_constant_setup* s)
{
    R_ASSERT(s);
    ref_constant C = ctable.get(name);
    if (C)
        C->handler = s;
}

void CBlender_Compile::r_ColorWriteEnable(bool cR, bool cG, bool cB, bool cA)
{
    BYTE Mask = 0;
    Mask |= cR ? D3DCOLORWRITEENABLE_RED : 0;
    Mask |= cG ? D3DCOLORWRITEENABLE_GREEN : 0;
    Mask |= cB ? D3DCOLORWRITEENABLE_BLUE : 0;
    Mask |= cA ? D3DCOLORWRITEENABLE_ALPHA : 0;

    RS.SetRS(D3DRS_COLORWRITEENABLE, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE1, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE2, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE3, Mask);
}
