#include "stdafx.h"
#include "WeaponKnife.h"

void CWeaponKnife::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponKnife>("CWeaponKnife", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponKnife>), "factory",
                                   &client_factory<CWeaponKnife>, sol::base_classes, xr_sol_bases<CWeaponKnife>());
}
