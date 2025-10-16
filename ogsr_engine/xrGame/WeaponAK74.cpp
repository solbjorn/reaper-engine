#include "stdafx.h"

#include "WeaponAK74.h"

CWeaponAK74::CWeaponAK74(ESoundTypes eSoundType) : CWeaponMagazinedWGrenade{eSoundType} {}
CWeaponAK74::~CWeaponAK74() {}

void CWeaponAK74::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponAK74>("CWeaponAK74", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponAK74>), "factory",
                                  &xr::client_factory<CWeaponAK74>, sol::base_classes, xr::sol_bases<CWeaponAK74>());
}
