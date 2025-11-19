#pragma once

#include "dx103DFluidData.h"
#include "../../xrRender/FBasicVisual.h"

class dx103DFluidVolume : public dxRender_Visual
{
    RTTI_DECLARE_TYPEINFO(dx103DFluidVolume, dxRender_Visual);

public:
    dx103DFluidVolume();
    ~dx103DFluidVolume() override;

    void Load(LPCSTR, IReader* data, u32) override;
    void Render(CBackend& cmd_list, float, bool) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;

    [[nodiscard]] shared_str getProfileName() { return m_FluidData.dbg_name; }

private:
    //	For debug purpose only
    ref_geom m_Geom;

    dx103DFluidData m_FluidData;
};
