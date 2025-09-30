#pragma once

#include "weaponmagazined.h"
#include "script_export_space.h"

class CWeaponVintorez : public CWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CWeaponVintorez, CWeaponMagazined);

public:
    typedef CWeaponMagazined inherited;

    CWeaponVintorez();
    virtual ~CWeaponVintorez();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponVintorez);

add_to_type_list(CWeaponVintorez);
#undef script_type_list
#define script_type_list save_type_list(CWeaponVintorez)
