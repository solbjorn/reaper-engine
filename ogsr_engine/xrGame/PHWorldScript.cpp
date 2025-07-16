#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "Physics.h"
#include "PHWorld.h"
#include "PHCommander.h"

void CPHWorld::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPHWorld>("physics_world", sol::no_constructor, "set_gravity", &CPHWorld::SetGravity, "gravity", &CPHWorld::Gravity, "add_call",
                                              &CPHWorld::AddCall);
}
