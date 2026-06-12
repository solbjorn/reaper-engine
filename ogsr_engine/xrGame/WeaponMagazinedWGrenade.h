#pragma once

#include "weaponmagazined.h"
#include "rocketlauncher.h"

class CWeaponFakeGrenade;

class CWeaponMagazinedWGrenade : public CWeaponMagazined, public CRocketLauncher
{
    RTTI_DECLARE_TYPEINFO(CWeaponMagazinedWGrenade, CWeaponMagazined, CRocketLauncher);

public:
    typedef CWeaponMagazined inherited;

    explicit CWeaponMagazinedWGrenade(ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
    ~CWeaponMagazinedWGrenade() override;

    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    void OnDrawUI() override;
    void net_Relcase(CObject* object) override;

    void OnH_B_Independent(bool just_before_destroy) override;

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    [[nodiscard]] bool Attach(PIItem pIItem, bool b_send_event) override;
    [[nodiscard]] bool Detach(gsl::czstring item_section_name, bool b_spawn_item) override;
    [[nodiscard]] bool CanAttach(PIItem pIItem) override;
    [[nodiscard]] bool CanDetach(gsl::czstring item_section_name) override;
    void InitAddons() override;
    [[nodiscard]] bool UseScopeTexture() override;
    [[nodiscard]] f32 CurrentZoomFactor() override;

    void OnStateSwitch(u32 S, u32 oldState) override;

    void switch2_Idle() override;
    void switch2_Reload() override;
    void state_Fire(f32 dt) override;
    void OnShot() override;
    void SwitchState(u32 S) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void ReloadMagazine() override;

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;

    void UpdateSounds() override;
    void StopHUDSounds() override;

    // переключение в режим подствольника
    [[nodiscard]] bool SwitchMode() override;
    void PerformSwitchGL();
    void OnAnimationEnd(u32 state) override;

    [[nodiscard]] bool IsNecessaryItem(const shared_str& item_sect) override;

    // виртуальные функции для проигрывания анимации HUD
    void PlayAnimShow() override;
    void PlayAnimHide() override;
    void PlayAnimReload() override;
    void PlayAnimIdle() override;
    void PlayAnimShoot() override;
    virtual void PlayAnimModeSwitch();

    HUD_SOUND sndShotG;
    HUD_SOUND sndReloadG;
    HUD_SOUND sndSwitch;

    // дополнительные параметры патронов
    // для подствольника
    CWeaponAmmo* m_pAmmo2{};
    shared_str m_ammoSect2;
    xr_vector<shared_str> m_ammoTypes2;
    u32 m_ammoType2;
    int iMagazineSize2{};
    xr_vector<CCartridge> m_magazine2;
    bool m_bGrenadeMode{};

    CCartridge m_DefaultCartridge2;

    int iAmmoElapsed2{};

    virtual void UpdateGrenadeVisibility(bool visibility);

    // название косточки для гранаты подствольника в HUD
    shared_str grenade_bone_name;

    [[nodiscard]] s32 GetAmmoElapsed2() const override { return iAmmoElapsed2; }
    [[nodiscard]] bool IsGrenadeMode() const override { return m_bGrenadeMode; }
    [[nodiscard]] f32 Weight() const override;

    [[nodiscard]] bool IsPartlyReloading() const override
    {
        if (m_bGrenadeMode)
            return m_set_next_ammoType_on_reload == u32(-1) && iAmmoElapsed2 > 0 && !IsMisfire();
        else
            return inherited::IsPartlyReloading();
    }
};
XR_SOL_BASE_CLASSES(CWeaponMagazinedWGrenade);
