#include "stdafx.h"
#include "snork.h"

void CSnork::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSnork>("CSnork", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSnork>), "factory", &client_factory<CSnork>, sol::base_classes,
                             xr_sol_bases<CSnork>());
}
