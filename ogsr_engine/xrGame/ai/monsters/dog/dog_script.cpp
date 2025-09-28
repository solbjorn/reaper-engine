#include "stdafx.h"

#include "dog.h"

void CAI_Dog::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAI_Dog>("CAI_Dog", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Dog>), "factory", &xr::client_factory<CAI_Dog>,
                              sol::base_classes, xr::sol_bases<CAI_Dog>());
}
