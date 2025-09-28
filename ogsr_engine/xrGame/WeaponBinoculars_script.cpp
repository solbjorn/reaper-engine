#include "stdafx.h"

#include "weaponbinoculars.h"

void CWeaponBinoculars::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponBinoculars>("CWeaponBinoculars", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponBinoculars>), "factory",
                                        &xr::client_factory<CWeaponBinoculars>, sol::base_classes, xr::sol_bases<CWeaponBinoculars>());
}
