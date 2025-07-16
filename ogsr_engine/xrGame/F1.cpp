#include "stdafx.h"
#include "f1.h"

CF1::CF1(void) {}

CF1::~CF1(void) {}

void CF1::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CF1>("CF1", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CF1>), sol::base_classes, xr_sol_bases<CF1>());
}
