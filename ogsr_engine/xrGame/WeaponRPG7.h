#pragma once

#include "weaponpistol.h"
#include "rocketlauncher.h"
#include "script_export_space.h"

class CWeaponRPG7 : public CWeaponCustomPistol, public CRocketLauncher
{
    RTTI_DECLARE_TYPEINFO(CWeaponRPG7, CWeaponCustomPistol, CRocketLauncher);

private:
    typedef CWeaponCustomPistol inherited;

public:
    CWeaponRPG7();
    ~CWeaponRPG7() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void OnStateSwitch(u32 S, u32 oldState) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void ReloadMagazine() override;
    void Load(gsl::czstring section) override;
    void switch2_Fire() override;
    void on_a_hud_attach() override;

    void FireStart() override;
    void SwitchState(u32 S) override;

    void UpdateMissileVisibility();
    void UnloadMagazine(bool spawn_ammo = true) override;
    void PlayAnimReload() override;

protected:
    shared_str m_sGrenadeBoneName;
    shared_str m_sHudGrenadeBoneName;

    shared_str m_sRocketSection;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return RPG; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponRPG7);

add_to_type_list(CWeaponRPG7);
#undef script_type_list
#define script_type_list save_type_list(CWeaponRPG7)
