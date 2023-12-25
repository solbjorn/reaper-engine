#ifndef dxFontRender_included
#define dxFontRender_included
#pragma once

#include "..\..\Include\xrRender\FontRender.h"

class dxFontRender : public IFontRender
{
public:
    dxFontRender();
    virtual ~dxFontRender();

    virtual void Initialize(LPCSTR cShader, LPCSTR cTexture);
    virtual void OnRender(CGameFont& owner);

private:
    ref_shader pShader;
    ref_geom pGeom;

    void RenderFragment(CGameFont& owner, u32& i, u32 color_override, float dX, float dY, u32 length, u32 last);
};

#endif //	FontRender_included