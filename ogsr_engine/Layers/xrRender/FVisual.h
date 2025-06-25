// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////
#ifndef FVisualH
#define FVisualH
#pragma once

#include "fbasicvisual.h"

class Fvisual : public dxRender_Visual, public IRender_Mesh
{
    RTTI_DECLARE_TYPEINFO(Fvisual, dxRender_Visual, IRender_Mesh);

public:
    IRender_Mesh* m_fast;

    virtual void Render(CBackend& cmd_list, float, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

    Fvisual();
    virtual ~Fvisual();
};

#endif
