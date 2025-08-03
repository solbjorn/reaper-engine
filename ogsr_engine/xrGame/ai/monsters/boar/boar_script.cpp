#include "stdafx.h"
#include "boar.h"

void CAI_Boar::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAI_Boar>("CAI_Boar", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Boar>), "factory", &client_factory<CAI_Boar>,
                               sol::base_classes, xr_sol_bases<CAI_Boar>());
}
