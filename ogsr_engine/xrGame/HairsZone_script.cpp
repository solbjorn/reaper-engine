#include "stdafx.h"
#include "HairsZone.h"

void CHairsZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CHairsZone>("CHairsZone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CHairsZone>), sol::base_classes,
                                                xr_sol_bases<CHairsZone>());
}
