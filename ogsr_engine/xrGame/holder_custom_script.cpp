#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "holder_custom.h"

void CHolderCustom::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CHolderCustom>("holder", sol::no_constructor, "engaged", &CHolderCustom::Engaged, "Action", &CHolderCustom::Action, "SetParam",
                                                   sol::resolve<void(int, Fvector)>(&CHolderCustom::SetParam));
}
