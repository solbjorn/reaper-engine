#include "stdafx.h"
#include "poltergeist.h"

void CPoltergeist::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPoltergeist>("CPoltergeist", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPoltergeist>), sol::base_classes,
                                                  xr_sol_bases<CPoltergeist>());
}
