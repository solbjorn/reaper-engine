////////////////////////////////////////////////////////////////////////////
//	Module 		: script_animation_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script animation action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_animation_action.h"

void CScriptAnimationAction::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptAnimationAction>(
        "anim", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptAnimationAction(), CScriptAnimationAction(LPCSTR), CScriptAnimationAction(LPCSTR, bool), CScriptAnimationAction(MonsterSpace::EMentalState),
                          // Monster specific
                          CScriptAnimationAction(MonsterSpace::EScriptMonsterAnimAction, int)>(),

        // type
        "free", sol::var(MonsterSpace::eMentalStateFree), "danger", sol::var(MonsterSpace::eMentalStateDanger), "panic", sol::var(MonsterSpace::eMentalStatePanic),
        // monster
        "stand_idle", sol::var(MonsterSpace::eAA_StandIdle), "sit_idle", sol::var(MonsterSpace::eAA_SitIdle), "lie_idle", sol::var(MonsterSpace::eAA_LieIdle), "eat",
        sol::var(MonsterSpace::eAA_Eat), "sleep", sol::var(MonsterSpace::eAA_Sleep), "rest", sol::var(MonsterSpace::eAA_Rest), "attack", sol::var(MonsterSpace::eAA_Attack),
        "look_around", sol::var(MonsterSpace::eAA_LookAround), "turn", sol::var(MonsterSpace::eAA_Turn),

        "anim", &CScriptAnimationAction::SetAnimation, "type", &CScriptAnimationAction::SetMentalState, "completed", sol::resolve<bool()>(&CScriptAnimationAction::completed));
}
