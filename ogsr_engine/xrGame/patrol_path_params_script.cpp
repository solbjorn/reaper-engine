////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_params_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path parameters class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "patrol_path_params.h"

static Fvector CPatrolPathParams__point(const CPatrolPathParams* self, u32 index)
{
    THROW(self);
    return (self->point(index));
}

void CPatrolPathParams::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPatrolPathParams>(
        "patrol", sol::no_constructor, sol::call_constructor,
        sol::constructors<CPatrolPathParams(LPCSTR), CPatrolPathParams(LPCSTR, const PatrolPathManager::EPatrolStartType),
                          CPatrolPathParams(LPCSTR, const PatrolPathManager::EPatrolStartType, const PatrolPathManager::EPatrolRouteType),
                          CPatrolPathParams(LPCSTR, const PatrolPathManager::EPatrolStartType, const PatrolPathManager::EPatrolRouteType, bool),
                          CPatrolPathParams(LPCSTR, const PatrolPathManager::EPatrolStartType, const PatrolPathManager::EPatrolRouteType, bool, u32)>(),

        // start
        "start", sol::var(PatrolPathManager::ePatrolStartTypeFirst), "nearest", sol::var(PatrolPathManager::ePatrolStartTypeNearest), "custom",
        sol::var(PatrolPathManager::ePatrolStartTypePoint), "next", sol::var(PatrolPathManager::ePatrolStartTypeNext),
        // route
        "stop", sol::var(PatrolPathManager::ePatrolRouteTypeStop), "continue", sol::var(PatrolPathManager::ePatrolRouteTypeContinue), "dummy",
        sol::var(PatrolPathManager::ePatrolRouteTypeDummy),

        "count", &CPatrolPathParams::count, "level_vertex_id", &CPatrolPathParams::level_vertex_id, "game_vertex_id", &CPatrolPathParams::game_vertex_id, "point",
        &CPatrolPathParams__point, "name", &CPatrolPathParams::name, "index", sol::resolve<u32(LPCSTR) const>(&CPatrolPathParams::point), "get_nearest",
        sol::resolve<u32(const Fvector&) const>(&CPatrolPathParams::point), "flag", &CPatrolPathParams::flag, "flags", &CPatrolPathParams::flags, "terminal",
        &CPatrolPathParams::terminal);
}
