#include "stdafx.h"
#include "WeaponAK74.h"

CWeaponAK74::CWeaponAK74(LPCSTR name, ESoundTypes eSoundType) : CWeaponMagazinedWGrenade(name, eSoundType) {}

CWeaponAK74::~CWeaponAK74() {}

void CWeaponAK74::script_register(sol::state_view& lua)
{
    lua.new_usertype<CWeaponAK74>("CWeaponAK74", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponAK74>), "factory", &client_factory<CWeaponAK74>,
                                  sol::base_classes, xr_sol_bases<CWeaponAK74>());
}
