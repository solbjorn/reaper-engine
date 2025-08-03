////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster detail path manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "alife_monster_detail_path_manager.h"
#include "alife_smart_terrain_task.h"

void CALifeMonsterDetailPathManager::script_register(sol::state_view& lua)
{
    lua.new_usertype<CALifeMonsterDetailPathManager>(
        "CALifeMonsterDetailPathManager", sol::no_constructor, "target",
        sol::overload(sol::resolve<void(const GameGraph::_GRAPH_ID&, const u32&, const Fvector&)>(&CALifeMonsterDetailPathManager::target),
                      sol::resolve<void(const GameGraph::_GRAPH_ID&)>(&CALifeMonsterDetailPathManager::target),
                      sol::resolve<void(const CALifeSmartTerrainTask*)>(&CALifeMonsterDetailPathManager::target)),
        "speed",
        sol::overload(sol::resolve<void(const float&)>(&CALifeMonsterDetailPathManager::speed), sol::resolve<const float&() const>(&CALifeMonsterDetailPathManager::speed)),
        "completed", &CALifeMonsterDetailPathManager::completed, "actual", &CALifeMonsterDetailPathManager::actual, "failed", &CALifeMonsterDetailPathManager::failed);
}
