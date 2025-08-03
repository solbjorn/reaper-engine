#include "stdafx.h"
#include "WeaponRG6.h"

void CWeaponRG6::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponRG6>("CWeaponRG6", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponRG6>), "factory", &client_factory<CWeaponRG6>,
                                 sol::base_classes, xr_sol_bases<CWeaponRG6>());
}
