#include "stdafx.h"
#include "PhysicObject.h"

using namespace luabind;

void CPhysicObject::script_register(lua_State* L) { module(L)[class_<CPhysicObject, CGameObject>("CPhysicObject").def(constructor<>())]; }
