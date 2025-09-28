#include "stdafx.h"

#include "chimera.h"

void CChimera::script_register(sol::state_view& lua)
{
    lua.new_usertype<CChimera>("CChimera", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CChimera>), "factory", &xr::client_factory<CChimera>,
                               sol::base_classes, xr::sol_bases<CChimera>());
}
