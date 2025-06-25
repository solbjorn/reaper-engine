#pragma once

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
    virtual ~CVisualZone();
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void AffectObjects();
    virtual void SwitchZoneState(EZoneState new_state);
    virtual void Load(LPCSTR section);
    virtual void UpdateBlowout();
};
