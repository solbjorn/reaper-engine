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
    CWeaponRPG7(void);
    virtual ~CWeaponRPG7(void);

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void OnStateSwitch(u32 S, u32 oldState);
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual void ReloadMagazine();
    virtual void Load(LPCSTR section);
    virtual void switch2_Fire();
    virtual void on_a_hud_attach();

    virtual void FireStart();
    virtual void SwitchState(u32 S);

    void UpdateMissileVisibility();
    virtual void UnloadMagazine(bool spawn_ammo = true);
    virtual void PlayAnimReload();

protected:
    shared_str m_sGrenadeBoneName;
    shared_str m_sHudGrenadeBoneName;

    shared_str m_sRocketSection;

    virtual size_t GetWeaponTypeForCollision() const override { return RPG; }

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponRPG7)
#undef script_type_list
#define script_type_list save_type_list(CWeaponRPG7)
