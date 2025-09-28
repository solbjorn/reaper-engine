#include "stdafx.h"

#include "tushkano.h"

void CTushkano::script_register(sol::state_view& lua)
{
    lua.new_usertype<CTushkano>("CTushkano", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CTushkano>), "factory", &xr::client_factory<CTushkano>,
                                sol::base_classes, xr::sol_bases<CTushkano>());
}
