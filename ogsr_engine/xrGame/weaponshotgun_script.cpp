#include "stdafx.h"

#include "WeaponShotgun.h"

void CWeaponShotgun::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponShotgun>("CWeaponShotgun", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponShotgun>), "factory",
                                     &xr::client_factory<CWeaponShotgun>,
#ifdef DUPLET_STATE_SWITCH
                                     "is_duplet_enabled", sol::readonly(&CWeaponShotgun::is_duplet_enabled), "switch_duplet", &CWeaponShotgun::SwitchDuplet,
#endif // !DUPLET_STATE_SWITCH
                                     sol::base_classes, xr::sol_bases<CWeaponShotgun>());
}
