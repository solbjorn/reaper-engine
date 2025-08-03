#include "stdafx.h"
#include "PhysicObject.h"

void CPhysicObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPhysicObject>("CPhysicObject", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPhysicObject>), "factory",
                                    &client_factory<CPhysicObject>, sol::base_classes, xr_sol_bases<CPhysicObject>());
}
