#pragma once

#include "EffectorShot.h"

// Этот класс в ЗП удалён и по видимому не используется.
class CCameraShotEffectorX : public CCameraShotEffector
{
    RTTI_DECLARE_TYPEINFO(CCameraShotEffectorX, CCameraShotEffector);

public:
    typedef CCameraShotEffector inherited;

    explicit CCameraShotEffectorX(float max_angle, float relax_time, float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
    ~CCameraShotEffectorX() override;

    tmc::task<bool> ProcessCam(SCamEffectorInfo&) override;
    void GetDeltaAngle(Fvector3& delta_angle) override;
    void Shot(f32 angle) override;
    void Clear() override;

protected:
    virtual void UpdateActorCamera(float dPitch, float dYaw);
};
