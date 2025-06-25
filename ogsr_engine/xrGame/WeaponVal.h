#pragma once

#include "weaponmagazined.h"
#include "script_export_space.h"

class CWeaponVal : public CWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CWeaponVal, CWeaponMagazined);

public:
    typedef CWeaponMagazined inherited;

    CWeaponVal(void);
    virtual ~CWeaponVal(void);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponVal)
#undef script_type_list
#define script_type_list save_type_list(CWeaponVal)
