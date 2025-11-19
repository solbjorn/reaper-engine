#ifndef __XR_WEAPON_HPSA_H__
#define __XR_WEAPON_HPSA_H__

#include "WeaponPistol.h"
#include "script_export_space.h"

class CWeaponHPSA : public CWeaponPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponHPSA, CWeaponPistol);

private:
    typedef CWeaponPistol inherited;

public:
    CWeaponHPSA();
    ~CWeaponHPSA() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponHPSA);

add_to_type_list(CWeaponHPSA);
#undef script_type_list
#define script_type_list save_type_list(CWeaponHPSA)

#endif //__XR_WEAPON_HPSA_H__
