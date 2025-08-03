#include "stdafx.h"
#include "WeaponRPG7.h"

void CWeaponRPG7::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponRPG7>("CWeaponRPG7", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponRPG7>), "factory", &client_factory<CWeaponRPG7>,
                                  sol::base_classes, xr_sol_bases<CWeaponRPG7>());
}
