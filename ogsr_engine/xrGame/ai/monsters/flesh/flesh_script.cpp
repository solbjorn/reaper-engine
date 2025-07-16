#include "stdafx.h"
#include "flesh.h"

void CAI_Flesh::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CAI_Flesh>("CAI_Flesh", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Flesh>), sol::base_classes,
                                               xr_sol_bases<CAI_Flesh>());
}
