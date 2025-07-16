#include "stdafx.h"
#include "burer.h"

void CBurer::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CBurer>("CBurer", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CBurer>), sol::base_classes,
                                            xr_sol_bases<CBurer>());
}
