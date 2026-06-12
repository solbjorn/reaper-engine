#pragma once

#include "weaponShotgun.h"
#include "script_export_space.h"

class CWeaponBM16 : public CWeaponShotgun
{
    RTTI_DECLARE_TYPEINFO(CWeaponBM16, CWeaponShotgun);

public:
    typedef CWeaponShotgun inherited;

    HUD_SOUND m_sndReload1;

    CWeaponBM16();
    ~CWeaponBM16() override;

    void Load(gsl::czstring section) override;

protected:
    void PlayAnimShoot() override;
    void PlayAnimFakeShoot() override;
    void PlayAnimReload() override;
    void PlayAnimShow() override;
    void PlayAnimHide() override;
    void PlayAnimIdleMoving() override;
    void PlayAnimIdleMovingSlow() override;
    void PlayAnimIdleMovingCrouch() override;
    void PlayAnimIdleMovingCrouchSlow() override;
    void PlayAnimIdleSprint() override;
    void PlayAnimIdle() override;
    void PlayAnimCheckMisfire() override;
    void PlayAnimDeviceSwitch() override;
    void PlayAnimSprintStart() override;
    void PlayAnimSprintEnd() override;
    void PlayReloadSound() override;
    void OnShot() override;
    void UpdateSounds() override;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return BM_16; }

    [[nodiscard]] gsl::czstring GetAnimAimName() override;

private:
    string128 guns_bm_aim_anm;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponBM16);

add_to_type_list(CWeaponBM16);
#undef script_type_list
#define script_type_list save_type_list(CWeaponBM16)
