#pragma once

#include "weaponmagazinedwgrenade.h"
#include "script_export_space.h"

class CWeaponGroza : public CWeaponMagazinedWGrenade
{
    RTTI_DECLARE_TYPEINFO(CWeaponGroza, CWeaponMagazinedWGrenade);

public:
    typedef CWeaponMagazinedWGrenade inherited;

    CWeaponGroza(void);
    virtual ~CWeaponGroza(void);

protected:
    virtual size_t GetWeaponTypeForCollision() const override { return Groza; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponGroza);

add_to_type_list(CWeaponGroza);
#undef script_type_list
#define script_type_list save_type_list(CWeaponGroza)
