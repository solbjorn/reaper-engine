////////////////////////////////////////////////////////////////////////////
//	Module 		: client_spawn_manager_script.cpp
//	Created 	: 08.10.2004
//  Modified 	: 08.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Client spawn manager script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "client_spawn_manager.h"

void CClientSpawnManager::script_register(sol::state_view& lua)
{
    lua.new_usertype<CClientSpawnManager>(
        "client_spawn_manager", sol::no_constructor, "add",
        sol::overload(sol::resolve<void(ALife::_OBJECT_ID, ALife::_OBJECT_ID, const luabind::functor<void>&, const luabind::object&)>(&CClientSpawnManager::add),
                      sol::resolve<void(ALife::_OBJECT_ID, ALife::_OBJECT_ID, const luabind::functor<void>&)>(&CClientSpawnManager::add)),
        "remove", sol::resolve<void(ALife::_OBJECT_ID, ALife::_OBJECT_ID)>(&CClientSpawnManager::remove));
}
