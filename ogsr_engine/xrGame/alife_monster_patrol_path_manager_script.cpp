////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_patrol_path_manager_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster patrol path manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "alife_monster_patrol_path_manager.h"
#include "patrol_path_manager_space.h"

static Fvector CALifeMonsterPatrolPathManager__target_position(CALifeMonsterPatrolPathManager* self)
{
    THROW(self);
    return (self->target_position());
}

void CALifeMonsterPatrolPathManager::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CALifeMonsterPatrolPathManager>(
        "CALifeMonsterPatrolPathManager", sol::no_constructor, "path", sol::resolve<void(LPCSTR)>(&CALifeMonsterPatrolPathManager::path), "start_type",
        sol::overload(sol::resolve<void(const EPatrolStartType&)>(&CALifeMonsterPatrolPathManager::start_type),
                      sol::resolve<const EPatrolStartType&() const>(&CALifeMonsterPatrolPathManager::start_type)),
        "route_type",
        sol::overload(sol::resolve<void(const EPatrolRouteType&)>(&CALifeMonsterPatrolPathManager::route_type),
                      sol::resolve<const EPatrolRouteType&() const>(&CALifeMonsterPatrolPathManager::route_type)),
        "actual", &CALifeMonsterPatrolPathManager::actual, "completed", &CALifeMonsterPatrolPathManager::completed, "start_vertex_index",
        &CALifeMonsterPatrolPathManager::start_vertex_index, "use_randomness",
        sol::overload(sol::resolve<void(const bool&)>(&CALifeMonsterPatrolPathManager::use_randomness),
                      sol::resolve<bool() const>(&CALifeMonsterPatrolPathManager::use_randomness)),
        "target_game_vertex_id", &CALifeMonsterPatrolPathManager::target_game_vertex_id, "target_level_vertex_id", &CALifeMonsterPatrolPathManager::target_level_vertex_id,
        "target_position", &CALifeMonsterPatrolPathManager__target_position);
}
