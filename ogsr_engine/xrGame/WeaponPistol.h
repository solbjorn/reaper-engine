#pragma once

#include "weaponcustompistol.h"

class CWeaponPistol : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponPistol, CWeaponCustomPistol);

public:
    typedef CWeaponCustomPistol inherited;

    CWeaponPistol();
    ~CWeaponPistol() override;

    void Load(gsl::czstring section) override;

    tmc::task<void> net_Destroy() override;
    void OnH_B_Chield() override;

    void OnAnimationEnd(u32 state) override;

    // анимации
    void PlayAnimShow() override;
    void PlayAnimIdleSprint() override;
    void PlayAnimIdleMoving() override;
    void PlayAnimIdleMovingSlow() override;
    void PlayAnimIdleMovingCrouch() override;
    void PlayAnimIdleMovingCrouchSlow() override;
    void PlayAnimIdle() override;
    void PlayAnimAim() override;
    void PlayAnimHide() override;
    void PlayAnimReload() override;
    void PlayAnimShoot() override;

    void UpdateSounds() override;

protected:
    [[nodiscard]] bool AllowFireWhileWorking() override { return true; }
    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return Pistol; }

    HUD_SOUND sndClose;
    ESoundTypes m_eSoundClose;

    bool m_opened;
};
