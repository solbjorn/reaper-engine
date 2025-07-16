////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_condition_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action condition class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "script_action_condition.h"

void CScriptActionCondition::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptActionCondition>(
        "cond", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptActionCondition(), CScriptActionCondition(u32), CScriptActionCondition(u32, double)>(),

        // cond
        "move_end", sol::var(CScriptActionCondition::MOVEMENT_FLAG), "look_end", sol::var(CScriptActionCondition::WATCH_FLAG), "anim_end",
        sol::var(CScriptActionCondition::ANIMATION_FLAG), "sound_end", sol::var(CScriptActionCondition::SOUND_FLAG), "object_end", sol::var(CScriptActionCondition::OBJECT_FLAG),
        "time_end", sol::var(CScriptActionCondition::TIME_FLAG), "act_end", sol::var(CScriptActionCondition::ACT_FLAG));
}
