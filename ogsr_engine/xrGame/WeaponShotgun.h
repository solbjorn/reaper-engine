#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponShotgun : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponShotgun, CWeaponCustomPistol);

public:
    typedef CWeaponCustomPistol inherited;

    CWeaponShotgun();
    ~CWeaponShotgun() override;

    void Load(gsl::czstring section) override;

    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    void Reload() override;
    void TryReload();
    void Fire2Start() override;
    void Fire2End() override;
    void OnShot() override;
    virtual void OnShotBoth();
    void switch2_Fire() override;
    void switch2_Fire2() override;
    void StopHUDSounds() override;

    void UpdateSounds() override;
    tmc::task<void> UpdateCL() override;

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;

#ifdef DUPLET_STATE_SWITCH
    bool is_duplet_enabled = false;
    void SwitchDuplet();
#endif // !DUPLET_STATE_SWITCH

protected:
    void OnAnimationEnd(u32 state) override;
    void TriStateReload();
    void OnStateSwitch(u32 S, u32 oldState) override;

    [[nodiscard]] bool HaveCartridgeInInventory(u8 cnt);
    [[nodiscard]] virtual u8 AddCartridge(u8 cnt);
    void ReloadMagazine() override;

    HUD_SOUND sndShotBoth;
    ESoundTypes m_eSoundShotBoth;

    ESoundTypes m_eSoundOpen{};
    ESoundTypes m_eSoundAddCartridge;
    ESoundTypes m_eSoundClose;
    HUD_SOUND m_sndOpen;
    HUD_SOUND m_sndAddCartridge, m_sndAddCartridgeSecond, m_sndAddCartridgeStart, m_sndAddCartridgeEmpty;
    HUD_SOUND m_sndClose, m_sndCloseEmpty, m_sndBreech, m_sndBreechJammed;

    bool m_stop_triStateReload{};
    bool has_anm_reload_jammed{};
    bool SecondCartridge{};
    bool StartCartridge{};

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponShotgun);

add_to_type_list(CWeaponShotgun);
#undef script_type_list
#define script_type_list save_type_list(CWeaponShotgun)
