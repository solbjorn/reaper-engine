////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_task_script.cpp
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain task
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "alife_smart_terrain_task.h"

void CALifeSmartTerrainTask::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CALifeSmartTerrainTask>("CALifeSmartTerrainTask", sol::no_constructor, sol::call_constructor,
                                                            sol::constructors<CALifeSmartTerrainTask(LPCSTR), CALifeSmartTerrainTask(LPCSTR, u32)>(), "game_vertex_id",
                                                            &CALifeSmartTerrainTask::game_vertex_id, "level_vertex_id", &CALifeSmartTerrainTask::level_vertex_id, "point_raw",
                                                            &CALifeSmartTerrainTask::point_raw, "position", &CALifeSmartTerrainTask::position);
}
