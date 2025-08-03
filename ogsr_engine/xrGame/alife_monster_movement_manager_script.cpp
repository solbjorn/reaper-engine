////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_movement_manager_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster movement manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "alife_monster_movement_manager.h"
#include "alife_monster_detail_path_manager.h"
#include "alife_monster_patrol_path_manager.h"
#include "movement_manager_space.h"

static CALifeMonsterDetailPathManager* get_detail(const CALifeMonsterMovementManager* self) { return &self->detail(); }
static CALifeMonsterPatrolPathManager* get_patrol(const CALifeMonsterMovementManager* self) { return &self->patrol(); }

void CALifeMonsterMovementManager::script_register(sol::state_view& lua)
{
    lua.new_usertype<CALifeMonsterMovementManager>("CALifeMonsterMovementManager", sol::no_constructor, "detail", &get_detail, "patrol", &get_patrol, "path_type",
                                                   sol::overload(sol::resolve<void(const EPathType&)>(&CALifeMonsterMovementManager::path_type),
                                                                 sol::resolve<const EPathType&() const>(&CALifeMonsterMovementManager::path_type)),
                                                   "actual", &CALifeMonsterMovementManager::actual, "completed", &CALifeMonsterMovementManager::completed);
}
