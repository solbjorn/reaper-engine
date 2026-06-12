// FHierrarhyVisual.h: interface for the FHierrarhyVisual class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "fbasicvisual.h"

class FHierrarhyVisual : public dxRender_Visual
{
    RTTI_DECLARE_TYPEINFO(FHierrarhyVisual, dxRender_Visual);

public:
    xr_vector<dxRender_Visual*> children;
    BOOL bDontDelete;

    FHierrarhyVisual();
    ~FHierrarhyVisual() override;

    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};
