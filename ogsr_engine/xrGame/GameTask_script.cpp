#include "stdafx.h"

#include "GameTask.h"

void CGameTask::script_register(sol::state_view& lua)
{
    lua.new_enum("task", "fail", eTaskStateFail, "in_progress", eTaskStateInProgress, "completed", eTaskStateCompleted, "task_dummy", eTaskStateDummy);

    lua.new_usertype<SGameTaskObjective>("SGameTaskObjective", sol::no_constructor, sol::call_constructor, sol::constructors<SGameTaskObjective(CGameTask*, int)>(),
                                         "set_description", &SGameTaskObjective::SetDescription_script, "get_description", &SGameTaskObjective::GetDescription_script,
                                         "set_article_id", &SGameTaskObjective::SetArticleID_script, "set_map_hint", &SGameTaskObjective::SetMapHint_script, "set_map_location",
                                         &SGameTaskObjective::SetMapLocation_script, "set_object_id", &SGameTaskObjective::SetObjectID_script, "set_icon_name",
                                         &SGameTaskObjective::SetIconName_script, "def_ml_enabled", &SGameTaskObjective::def_location_enabled,

                                         "add_complete_info", &SGameTaskObjective::AddCompleteInfo_script, "add_fail_info", &SGameTaskObjective::AddFailInfo_script,
                                         "add_on_complete_info", &SGameTaskObjective::AddOnCompleteInfo_script, "add_on_fail_info", &SGameTaskObjective::AddOnFailInfo_script,
                                         "add_complete_func", &SGameTaskObjective::AddCompleteFunc_script, "add_fail_func", &SGameTaskObjective::AddFailFunc_script,
                                         "add_on_complete_func", &SGameTaskObjective::AddOnCompleteFunc_script, "add_on_fail_func", &SGameTaskObjective::AddOnFailFunc_script,

                                         "get_idx", &SGameTaskObjective::GetIDX_script, "get_state", &SGameTaskObjective::TaskState);

    lua.new_usertype<CGameTask>("CGameTask", sol::no_constructor, sol::call_constructor, sol::constructors<CGameTask()>(), "load", &CGameTask::Load_script, "set_title",
                                &CGameTask::SetTitle_script, "get_title", &CGameTask::GetTitle_script, "set_priority", &CGameTask::SetPriority_script, "get_priority",
                                &CGameTask::GetPriority_script, "add_objective", &CGameTask::AddObjective_script, "get_id", &CGameTask::GetID_script, "set_id",
                                &CGameTask::SetID_script, "get_objective", &CGameTask::GetObjective_script, "get_objectives_cnt", &CGameTask::GetObjectiveSize_script);
}
