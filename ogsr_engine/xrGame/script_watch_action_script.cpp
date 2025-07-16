////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_watch_action.h"
#include "script_game_object.h"
#include "sight_manager_space.h"

void CScriptWatchAction::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptWatchAction>(
        "look", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptWatchAction(), CScriptWatchAction(SightManager::ESightType), CScriptWatchAction(SightManager::ESightType, Fvector&),
                          CScriptWatchAction(SightManager::ESightType, CScriptGameObject*), CScriptWatchAction(SightManager::ESightType, CScriptGameObject*, LPCSTR),
                          // searchlight
                          CScriptWatchAction(const Fvector&, float, float), CScriptWatchAction(CScriptGameObject*, float, float)>(),

        // look
        "path_dir", sol::var(SightManager::eSightTypePathDirection), "search", sol::var(SightManager::eSightTypeSearch), "danger", sol::var(SightManager::eSightTypeCover), "point",
        sol::var(SightManager::eSightTypePosition), "fire_point", sol::var(SightManager::eSightTypeFirePosition), "cur_dir", sol::var(SightManager::eSightTypeCurrentDirection),
        "direction", sol::var(SightManager::eSightTypeDirection),

        // time
        "object", &CScriptWatchAction::SetWatchObject, "direct", &CScriptWatchAction::SetWatchDirection, "type", &CScriptWatchAction::SetWatchType, "bone",
        &CScriptWatchAction::SetWatchBone, "completed", sol::resolve<bool()>(&CScriptWatchAction::completed));
}
