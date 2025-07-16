#include "stdafx.h"
#include "boar.h"

void CAI_Boar::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CAI_Boar>("CAI_Boar", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Boar>), sol::base_classes,
                                              xr_sol_bases<CAI_Boar>());
}
