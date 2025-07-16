#include "stdafx.h"
#include "tushkano.h"

void CTushkano::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CTushkano>("CTushkano", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CTushkano>), sol::base_classes,
                                               xr_sol_bases<CTushkano>());
}
