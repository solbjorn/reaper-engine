#include "stdafx.h"
#include "mincer.h"

void CMincer::script_register(sol::state_view& lua)
{
    lua.new_usertype<CMincer>("CMincer", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CMincer>), "factory", &client_factory<CMincer>,
                              sol::base_classes, xr_sol_bases<CMincer>());
}
