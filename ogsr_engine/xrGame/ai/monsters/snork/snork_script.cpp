#include "stdafx.h"
#include "snork.h"

void CSnork::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSnork>("CSnork", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSnork>), sol::base_classes,
                                            xr_sol_bases<CSnork>());
}
