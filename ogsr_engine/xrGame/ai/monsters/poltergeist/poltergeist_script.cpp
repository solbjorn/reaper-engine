#include "stdafx.h"
#include "poltergeist.h"

void CPoltergeist::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPoltergeist>("CPoltergeist", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPoltergeist>), "factory",
                                   &client_factory<CPoltergeist>, sol::base_classes, xr_sol_bases<CPoltergeist>());
}
