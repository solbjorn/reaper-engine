#include "stdafx.h"
#include "mincer.h"

void CMincer::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CMincer>("CMincer", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CMincer>), sol::base_classes,
                                             xr_sol_bases<CMincer>());
}
