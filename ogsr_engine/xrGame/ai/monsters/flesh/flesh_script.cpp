#include "stdafx.h"
#include "flesh.h"

void CAI_Flesh::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAI_Flesh>("CAI_Flesh", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Flesh>), "factory", &client_factory<CAI_Flesh>,
                                sol::base_classes, xr_sol_bases<CAI_Flesh>());
}
