////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "action_planner.h"
#include "script_game_object.h"

static void set_goal_world_state(CScriptActionPlanner* action_planner, CScriptActionPlanner::CState* world_state) { action_planner->set_target_state(*world_state); }

static bool get_actual(const CScriptActionPlanner* action_planner) { return (action_planner->actual()); }

static void add_operator_script(CScriptActionPlanner& self, const CScriptActionPlanner::_edge_type& operator_id,
                                std::unique_ptr<std::remove_pointer_t<CScriptActionPlanner::_operator_ptr>>& _operator)
{
    return self.add_operator(operator_id, _operator.release());
}

static void add_evaluator_script(CScriptActionPlanner& self, const CScriptActionPlanner::_condition_type& condition_id,
                                 std::unique_ptr<std::remove_pointer_t<CScriptActionPlanner::condition_evaluator_ptr>>& evaluator)
{
    return self.add_evaluator(condition_id, evaluator.release());
}

template <>
void CActionPlanner<CScriptGameObject>::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptActionPlanner>("action_planner", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptActionPlanner()>(), "object",
                                           sol::readonly(&CScriptActionPlanner::m_object), "storage", sol::readonly(&CScriptActionPlanner::m_storage), "actual", &get_actual,
                                           "setup", &CScriptActionPlanner::setup, "update", &CScriptActionPlanner::update, "add_action", &add_operator_script, "remove_action",
                                           sol::resolve<void(const CScriptActionPlanner::_edge_type&)>(&CScriptActionPlanner::remove_operator), "action",
                                           &CScriptActionPlanner::action, "add_evaluator", add_evaluator_script, "remove_evaluator",
                                           sol::resolve<void(const CScriptActionPlanner::_condition_type&)>(&CScriptActionPlanner::remove_evaluator), "evaluator",
                                           &CScriptActionPlanner::evaluator, "current_action_id", &CScriptActionPlanner::current_action_id, "current_action",
                                           &CScriptActionPlanner::current_action, "initialized", &CScriptActionPlanner::initialized, "set_goal_world_state", &set_goal_world_state,
#ifdef LOG_ACTION
                                           "show", &CScriptActionPlanner::show,
#endif
                                           sol::base_classes, xr::sol_bases<CScriptActionPlanner>());
}
