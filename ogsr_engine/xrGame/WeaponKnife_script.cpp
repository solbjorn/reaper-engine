#include "stdafx.h"
#include "WeaponKnife.h"

void CWeaponKnife::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CWeaponKnife>("CWeaponKnife", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CWeaponKnife>), sol::base_classes,
                                                  xr_sol_bases<CWeaponKnife>());
}
