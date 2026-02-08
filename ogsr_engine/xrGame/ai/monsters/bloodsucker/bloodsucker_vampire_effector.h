#pragma once

#include "../../../../xr_3da/effectorPP.h"
#include "../../../CameraEffector.h"
#include "../../../../xr_3da/cameramanager.h"

class CVampirePPEffector : public CEffectorPP
{
    RTTI_DECLARE_TYPEINFO(CVampirePPEffector, CEffectorPP);

private:
    typedef CEffectorPP inherited;

    SPPInfo state; // current state
    float m_total; // total PP time

public:
    explicit CVampirePPEffector(const SPPInfo& ppi, float life_time);
    ~CVampirePPEffector() override = default;

    virtual BOOL Process(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////

class CVampireCameraEffector : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CVampireCameraEffector, CEffectorCam);

private:
    typedef CEffectorCam inherited;

    float m_time_total;
    Fvector dangle_target;
    Fvector dangle_current;

    float m_dist;
    Fvector m_direction;

public:
    explicit CVampireCameraEffector(float time, const Fvector& src, const Fvector& tgt);
    ~CVampireCameraEffector() override = default;

    tmc::task<bool> ProcessCam(SCamEffectorInfo& info) override;
};
