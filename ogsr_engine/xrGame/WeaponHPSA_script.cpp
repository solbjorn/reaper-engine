#include "stdafx.h"
#include "WeaponHPSA.h"

void CWeaponHPSA::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponHPSA>("CWeaponHPSA", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponHPSA>), sol::base_classes,
                                                 xr_sol_bases<CWeaponHPSA>());
}
