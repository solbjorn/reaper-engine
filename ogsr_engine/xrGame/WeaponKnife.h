#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponKnife : public CWeapon
{
    RTTI_DECLARE_TYPEINFO(CWeaponKnife, CWeapon);

private:
    typedef CWeapon inherited;

protected:
    HUD_SOUND m_sndShot;
    HUD_SOUND sndItemOn;

    bool m_attackStart;
    bool m_attackMotionMarksAvailable;

    bool HeadLampSwitch{}, NightVisionSwitch{};

protected:
    virtual void switch2_Idle();
    virtual void switch2_Hiding();
    virtual void switch2_Hidden();
    virtual void switch2_Showing();
    void switch2_Attacking(u32 state);

    void OnMotionMark(u32 state, const motion_marks& M) override;
    void OnAnimationEnd(u32 state) override;
    void OnStateSwitch(u32 S, u32 oldState) override;

    void DeviceUpdate() override;
    tmc::task<void> UpdateCL() override;
    void PlayAnimDeviceSwitch() override;

    void state_Attacking(float dt);

    virtual void KnifeStrike(u32 state, const Fvector& pos, const Fvector& dir);

    float fWallmarkSize;
    u16 knife_material_idx;

protected:
    ALife::EHitType m_eHitType_1;
    Fvector4 fvHitPower_1;
    float fHitImpulse_1;

    ALife::EHitType m_eHitType_2;
    Fvector4 fvHitPower_2;
    float fHitImpulse_2;

    void LoadFireParams(gsl::czstring section, gsl::czstring prefix) override;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return Knife_and_other; }
    // TODO: рассчитать здесь позицию для коллизии
    [[nodiscard]] Fvector GetPositionForCollision() override { return Device.vCameraPosition; }
    [[nodiscard]] Fvector GetDirectionForCollision() override { return Device.vCameraDirection; }

public:
    CWeaponKnife();
    ~CWeaponKnife() override;

    void Load(gsl::czstring section) override;

    void Fire2Start() override;
    void FireStart() override;

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;

    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponKnife);

add_to_type_list(CWeaponKnife);
#undef script_type_list
#define script_type_list save_type_list(CWeaponKnife)
