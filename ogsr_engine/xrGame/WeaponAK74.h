#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__

#include "WeaponMagazinedWGrenade.h"
#include "script_export_space.h"

class CWeaponAK74 : public CWeaponMagazinedWGrenade
{
    RTTI_DECLARE_TYPEINFO(CWeaponAK74, CWeaponMagazinedWGrenade);

private:
    typedef CWeaponMagazinedWGrenade inherited;

public:
    explicit CWeaponAK74(ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
    ~CWeaponAK74() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponAK74);

add_to_type_list(CWeaponAK74);
#undef script_type_list
#define script_type_list save_type_list(CWeaponAK74)

#endif //__XR_WEAPON_AK74_H__
