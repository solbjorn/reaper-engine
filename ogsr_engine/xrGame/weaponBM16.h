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

    virtual void Load(LPCSTR section);

protected:
    virtual void PlayAnimShoot();
    virtual void PlayAnimFakeShoot() override;
    virtual void PlayAnimReload();
    virtual void PlayAnimShow();
    virtual void PlayAnimHide();
    virtual void PlayAnimIdleMoving();
    virtual void PlayAnimIdleMovingSlow() override;
    virtual void PlayAnimIdleMovingCrouch() override;
    virtual void PlayAnimIdleMovingCrouchSlow() override;
    virtual void PlayAnimIdleSprint();
    virtual void PlayAnimIdle();
    virtual void PlayAnimCheckMisfire() override;
    virtual void PlayAnimDeviceSwitch() override;
    virtual void PlayAnimSprintStart() override;
    virtual void PlayAnimSprintEnd() override;
    virtual void PlayReloadSound();
    virtual void OnShot();
    virtual void UpdateSounds() override;

    virtual size_t GetWeaponTypeForCollision() const override { return BM_16; }

    const char* GetAnimAimName() override;

private:
    string128 guns_bm_aim_anm;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponBM16);

add_to_type_list(CWeaponBM16);
#undef script_type_list
#define script_type_list save_type_list(CWeaponBM16)
