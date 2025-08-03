////////////////////////////////////////////////////////////////////////////
//	Module 		: script_entity_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script entity action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_entity_action.h"

void CScriptEntityAction::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptEntityAction>(
        "entity_action", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptEntityAction(), CScriptEntityAction(const CScriptEntityAction*)>(), "set_action",
        sol::overload(sol::resolve<void(CScriptMovementAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptWatchAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptAnimationAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptSoundAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptParticleAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptObjectAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptActionCondition&), CScriptEntityAction>(&CScriptEntityAction::SetAction),
                      sol::resolve<void(CScriptMonsterAction&), CScriptEntityAction>(&CScriptEntityAction::SetAction)),
        "move", &CScriptEntityAction::CheckIfMovementCompleted, "look", &CScriptEntityAction::CheckIfWatchCompleted, "anim", &CScriptEntityAction::CheckIfAnimationCompleted,
        "sound", &CScriptEntityAction::CheckIfSoundCompleted, "particle", &CScriptEntityAction::CheckIfParticleCompleted, "object", &CScriptEntityAction::CheckIfObjectCompleted,
        "time", &CScriptEntityAction::CheckIfTimeOver, "all", sol::resolve<bool()>(&CScriptEntityAction::CheckIfActionCompleted), "completed",
        sol::resolve<bool()>(&CScriptEntityAction::CheckIfActionCompleted));
}
