#include "stdafx.h"
#include "PhysicObject.h"

void CPhysicObject::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPhysicObject>("CPhysicObject", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPhysicObject>), sol::base_classes,
                                                   xr_sol_bases<CPhysicObject>());
}
