#include "stdafx.h"
#include "WeaponBM16.h"

void CWeaponBM16::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponBM16>("CWeaponBM16", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponBM16>), sol::base_classes,
                                                 xr_sol_bases<CWeaponBM16>());
}
