#include "stdafx.h"

#include "HairsZone.h"

void CHairsZone::script_register(sol::state_view& lua)
{
    lua.new_usertype<CHairsZone>("CHairsZone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CHairsZone>), "factory", &xr::client_factory<CHairsZone>,
                                 sol::base_classes, xr::sol_bases<CHairsZone>());
}
