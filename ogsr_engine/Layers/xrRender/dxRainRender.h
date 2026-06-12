#pragma once

#include "..\..\Include\xrRender\RainRender.h"

class dxRainRender : public IRainRender
{
    RTTI_DECLARE_TYPEINFO(dxRainRender, IRainRender);

public:
    dxRainRender();
    ~dxRainRender() override;

    void Render(CEffect_Rain& owner) override;
    void Calculate(CEffect_Rain& owner) override;

    [[nodiscard]] const Fsphere& GetDropBounds() const override;

private:
    // Visualization	(rain)
    ref_shader SH_Rain;
    ref_geom hGeom_Rain;

    // Visualization	(drops)
    IRender_DetailModel* DM_Drop;
    ref_geom hGeom_Drops;
    ref_shader SH_Splash;
    std::unique_ptr<CPerlinNoise1D> RainPerlin;
};
