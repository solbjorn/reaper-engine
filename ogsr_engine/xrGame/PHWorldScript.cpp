#include "stdafx.h"

#include "Physics.h"
#include "PHWorld.h"
#include "PHCommander.h"

void CPHWorld::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPHWorld>("physics_world", sol::no_constructor, "set_gravity", &CPHWorld::SetGravity, "gravity", &CPHWorld::Gravity, "add_call", &CPHWorld::AddCall);
}
