#pragma once

#include "weaponcustompistol.h"
#include "script_export_space.h"

class CWeaponSVD : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponSVD, CWeaponCustomPistol);

public:
    typedef CWeaponCustomPistol inherited;

protected:
    virtual void switch2_Fire();
    virtual void OnAnimationEnd(u32 state);
    virtual size_t GetWeaponTypeForCollision() const override { return SniperRifle; }

public:
    CWeaponSVD(void);
    virtual ~CWeaponSVD(void);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponSVD)
#undef script_type_list
#define script_type_list save_type_list(CWeaponSVD)
