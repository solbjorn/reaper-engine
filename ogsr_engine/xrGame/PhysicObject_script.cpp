#include "stdafx.h"

#include "PhysicObject.h"

void CPhysicObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPhysicObject>("CPhysicObject", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPhysicObject>), "factory",
                                    &xr::client_factory<CPhysicObject>, sol::base_classes, xr::sol_bases<CPhysicObject>());
}
