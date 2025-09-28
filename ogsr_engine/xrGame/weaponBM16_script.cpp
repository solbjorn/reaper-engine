#include "stdafx.h"

#include "WeaponBM16.h"

void CWeaponBM16::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponBM16>("CWeaponBM16", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponBM16>), "factory",
                                  &xr::client_factory<CWeaponBM16>, sol::base_classes, xr::sol_bases<CWeaponBM16>());
}
