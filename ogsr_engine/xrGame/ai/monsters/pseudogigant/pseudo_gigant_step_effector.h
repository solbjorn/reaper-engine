#pragma once

#include "../../../CameraEffector.h"

class CPseudogigantStepEffector : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CPseudogigantStepEffector, CEffectorCam);

private:
    typedef CEffectorCam inherited;

    float total;
    float max_amp;
    float period_number;
    float power;

public:
    explicit CPseudogigantStepEffector(float time, float amp, float periods, float power);
    ~CPseudogigantStepEffector() override = default;

    tmc::task<bool> ProcessCam(SCamEffectorInfo& info) override;
};
