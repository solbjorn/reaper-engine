#include "stdafx.h"
#include "burer.h"

void CBurer::script_register(sol::state_view& lua)
{
    lua.new_usertype<CBurer>("CBurer", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CBurer>), "factory", &client_factory<CBurer>, sol::base_classes,
                             xr_sol_bases<CBurer>());
}
