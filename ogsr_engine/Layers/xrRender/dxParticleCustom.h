//---------------------------------------------------------------------------
#ifndef ParticleCustomH
#define ParticleCustomH

#include "../../Include/xrRender/ParticleCustom.h"

#include "FBasicVisual.h"

//---------------------------------------------------------------------------

class dxParticleCustom : public dxRender_Visual, public IParticleCustom
{
    RTTI_DECLARE_TYPEINFO(dxParticleCustom, dxRender_Visual, IParticleCustom);

public:
    // geometry-format
    ref_geom geom;

    ~dxParticleCustom() override = default;

    [[nodiscard]] IParticleCustom* dcast_ParticleCustom() override { return this; }
};

//---------------------------------------------------------------------------
#endif // ParticleCustomH
