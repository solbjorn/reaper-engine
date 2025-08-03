////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "action_planner_action.h"
#include "script_game_object.h"

template <>
void CActionPlannerAction<CScriptGameObject>::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptActionPlannerAction>(
        "planner_action", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptActionPlannerAction(), CScriptActionPlannerAction(CScriptGameObject*), CScriptActionPlannerAction(CScriptGameObject*, LPCSTR)>(), "setup",
        &CScriptActionPlannerAction::setup, "initialize", &CScriptActionPlannerAction::initialize, "execute", &CScriptActionPlannerAction::execute, "finalize",
        &CScriptActionPlannerAction::finalize,
#ifdef LOG_ACTION
        "show", &CScriptActionPlannerAction::show,
#endif
        "weight", &CScriptActionPlannerAction::weight, sol::base_classes, xr_sol_bases<CScriptActionPlannerAction>());
}
