#include "stdafx.h"
#include "bloodsucker.h"

void CAI_Bloodsucker::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAI_Bloodsucker>("CAI_Bloodsucker", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Bloodsucker>), "factory",
                                      &client_factory<CAI_Bloodsucker>, "force_visibility_state", &CAI_Bloodsucker::force_visibility_state, sol::base_classes,
                                      xr_sol_bases<CAI_Bloodsucker>());
}
