#pragma once

#include "weaponpistol.h"
#include "script_export_space.h"

class CWeaponWalther : public CWeaponPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponWalther, CWeaponPistol);

public:
    typedef CWeaponPistol inherited;

    CWeaponWalther(void);
    virtual ~CWeaponWalther(void);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponWalther);

add_to_type_list(CWeaponWalther);
#undef script_type_list
#define script_type_list save_type_list(CWeaponWalther)
