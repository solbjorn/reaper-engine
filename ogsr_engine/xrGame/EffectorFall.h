#pragma once

#include "../xr_3da/Effector.h"

// приседание после падения
class CEffectorFall : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CEffectorFall, CEffectorCam);

public:
    float fPower;
    float fPhase;

    explicit CEffectorFall(float power, float life_time = 1.0f);
    ~CEffectorFall() override = default;

    tmc::task<bool> ProcessCam(SCamEffectorInfo& info) override;
};
