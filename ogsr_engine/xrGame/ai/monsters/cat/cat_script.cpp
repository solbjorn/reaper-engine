#include "stdafx.h"
#include "cat.h"

void CCat::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CCat>("CCat", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CCat>), sol::base_classes, xr_sol_bases<CCat>());
}
