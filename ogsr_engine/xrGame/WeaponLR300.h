#ifndef __XR_WEAPON_LR300_H__
#define __XR_WEAPON_LR300_H__

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponLR300 : public CWeaponMagazined
{
    RTTI_DECLARE_TYPEINFO(CWeaponLR300, CWeaponMagazined);

private:
    typedef CWeaponMagazined inherited;

public:
    CWeaponLR300();
    ~CWeaponLR300() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponLR300);

add_to_type_list(CWeaponLR300);
#undef script_type_list
#define script_type_list save_type_list(CWeaponLR300)

#endif //__XR_WEAPON_LR300_H__
