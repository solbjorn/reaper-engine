#include "stdafx.h"
#include "weaponbinoculars.h"

void CWeaponBinoculars::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponBinoculars>("CWeaponBinoculars", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponBinoculars>),
                                                       sol::base_classes, xr_sol_bases<CWeaponBinoculars>());
}
