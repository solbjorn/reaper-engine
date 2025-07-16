#include "stdafx.h"
#include "WeaponRG6.h"

void CWeaponRG6::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponRG6>("CWeaponRG6", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponRG6>), sol::base_classes,
                                                xr_sol_bases<CWeaponRG6>());
}
