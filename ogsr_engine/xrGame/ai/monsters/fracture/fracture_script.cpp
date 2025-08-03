#include "stdafx.h"
#include "fracture.h"

void CFracture::script_register(sol::state_view& lua)
{
    lua.new_usertype<CFracture>("CFracture", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CFracture>), "factory", &client_factory<CFracture>,
                                sol::base_classes, xr_sol_bases<CFracture>());
}
