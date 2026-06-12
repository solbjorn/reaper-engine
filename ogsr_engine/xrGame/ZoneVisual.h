#pragma once

#include "CustomZone.h"

#include "..\Include/xrRender/KinematicsAnimated.h"

class CVisualZone : public CCustomZone
{
    RTTI_DECLARE_TYPEINFO(CVisualZone, CCustomZone);

public:
    typedef CCustomZone inherited;
    MotionID m_idle_animation;
    MotionID m_attack_animation;
    u32 m_dwAttackAnimaionStart;
    u32 m_dwAttackAnimaionEnd;

    CVisualZone();
    ~CVisualZone() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void AffectObjects() override;
    void SwitchZoneState(EZoneState new_state) override;
    void Load(gsl::czstring section) override;
    virtual void UpdateBlowout();
};
