#pragma once

#include "weaponpistol.h"
#include "script_export_space.h"

class CWeaponUSP45 : public CWeaponPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponUSP45, CWeaponPistol);

    typedef CWeaponPistol inherited;

public:
    CWeaponUSP45(void);
    virtual ~CWeaponUSP45(void);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponUSP45)
#undef script_type_list
#define script_type_list save_type_list(CWeaponUSP45)
