#include "stdafx.h"
#include "WeaponShotgun.h"

void CWeaponShotgun::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponShotgun>("CWeaponShotgun", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponShotgun>),
#ifdef DUPLET_STATE_SWITCH
                                                    "is_duplet_enabled", sol::readonly(&CWeaponShotgun::is_duplet_enabled), "switch_duplet", &CWeaponShotgun::SwitchDuplet,
#endif // !DUPLET_STATE_SWITCH
                                                    sol::base_classes, xr_sol_bases<CWeaponShotgun>());
}
