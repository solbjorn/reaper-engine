#pragma once

#include "rocketlauncher.h"
#include "weaponShotgun.h"
#include "script_export_space.h"

class CWeaponRG6 : public CRocketLauncher, public CWeaponShotgun
{
    RTTI_DECLARE_TYPEINFO(CWeaponRG6, CRocketLauncher, CWeaponShotgun);

public:
    typedef CRocketLauncher inheritedRL;
    typedef CWeaponShotgun inheritedSG;

    CWeaponRG6() = default;
    ~CWeaponRG6() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void Load(gsl::czstring section) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

protected:
    void FireTrace(const Fvector3& P, const Fvector3& D) override;
    virtual void LaunchGrenade(const Fvector& P, const Fvector& D);
    [[nodiscard]] u8 AddCartridge(u8 cnt) override;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return RG_6; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponRG6);

add_to_type_list(CWeaponRG6);
#undef script_type_list
#define script_type_list save_type_list(CWeaponRG6)
