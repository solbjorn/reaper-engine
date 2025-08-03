////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster brain class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "alife_monster_brain.h"
#include "alife_monster_movement_manager.h"

static CALifeMonsterMovementManager* get_movement(const CALifeMonsterBrain* brain) { return &brain->movement(); }

void CALifeMonsterBrain::script_register(sol::state_view& lua)
{
    lua.new_usertype<CALifeMonsterBrain>(
        "CALifeMonsterBrain", sol::no_constructor, "movement", &get_movement, "update", &CALifeMonsterBrain::update_script, "can_choose_alife_tasks",
        sol::overload(sol::resolve<void(bool)>(&CALifeMonsterBrain::can_choose_alife_tasks), sol::resolve<bool() const>(&CALifeMonsterBrain::can_choose_alife_tasks)),
        "m_time_interval", &CALifeMonsterBrain::m_time_interval, "m_last_search_time", &CALifeMonsterBrain::m_last_search_time);
}
