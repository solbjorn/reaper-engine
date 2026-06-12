#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponSVD : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponSVD, CWeaponCustomPistol);

public:
    typedef CWeaponCustomPistol inherited;

protected:
    void switch2_Fire() override;
    void OnAnimationEnd(u32 state) override;
    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return SniperRifle; }

public:
    CWeaponSVD();
    ~CWeaponSVD() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponSVD);

add_to_type_list(CWeaponSVD);
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVD)
