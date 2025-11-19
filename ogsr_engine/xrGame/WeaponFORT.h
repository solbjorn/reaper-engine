#ifndef __XR_WEAPON_FORT_H__
#define __XR_WEAPON_FORT_H__

#include "WeaponPistol.h"
#include "script_export_space.h"

#define SND_RIC_COUNT 5

class CWeaponFORT : public CWeaponPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponFORT, CWeaponPistol);

private:
    typedef CWeaponPistol inherited;

public:
    CWeaponFORT();
    ~CWeaponFORT() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponFORT);

add_to_type_list(CWeaponFORT);
#undef script_type_list
#define script_type_list save_type_list(CWeaponFORT)

#endif //__XR_WEAPON_FORT_H__
