#include "stdafx.h"
#include "WeaponHPSA.h"

void CWeaponHPSA::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponHPSA>("CWeaponHPSA", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponHPSA>), "factory", &client_factory<CWeaponHPSA>,
                                  sol::base_classes, xr_sol_bases<CWeaponHPSA>());
}
