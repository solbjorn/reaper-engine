#pragma once

#include "EffectorShot.h"

// Этот класс в ЗП удалён и по видимому не используется.
class CCameraShotEffectorX : public CCameraShotEffector
{
    RTTI_DECLARE_TYPEINFO(CCameraShotEffectorX, CCameraShotEffector);

public:
    typedef CCameraShotEffector inherited;

    CCameraShotEffectorX(float max_angle, float relax_time, float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
    virtual ~CCameraShotEffectorX();

    [[nodiscard]] BOOL ProcessCam(SCamEffectorInfo&) override;
    virtual void GetDeltaAngle(Fvector& delta_angle);
    virtual void Shot(float angle);
    virtual void Clear();

protected:
    virtual void UpdateActorCamera(float dPitch, float dYaw);
};
