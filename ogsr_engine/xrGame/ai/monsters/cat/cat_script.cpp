#include "stdafx.h"
#include "cat.h"

void CCat::script_register(sol::state_view& lua)
{
    lua.new_usertype<CCat>("CCat", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CCat>), "factory", &client_factory<CCat>, sol::base_classes,
                           xr_sol_bases<CCat>());
}
