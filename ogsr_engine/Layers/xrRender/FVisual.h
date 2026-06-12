// FVisual.h: interface for the FVisual class.
//
//////////////////////////////////////////////////////////////////////
#ifndef FVisualH
#define FVisualH

#include "fbasicvisual.h"

class Fvisual : public dxRender_Visual, public IRender_Mesh
{
    RTTI_DECLARE_TYPEINFO(Fvisual, dxRender_Visual, IRender_Mesh);

public:
    IRender_Mesh* m_fast{};

    Fvisual();
    ~Fvisual() override;

    void Render(CBackend& cmd_list, f32, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};

#endif
