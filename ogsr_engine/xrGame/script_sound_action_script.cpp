////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "script_sound_action.h"

void CScriptSoundAction::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptSoundAction>(
        "sound", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptSoundAction(), CScriptSoundAction(LPCSTR, LPCSTR), CScriptSoundAction(LPCSTR, LPCSTR, const Fvector&),
                          CScriptSoundAction(LPCSTR, LPCSTR, const Fvector&, const Fvector&), CScriptSoundAction(LPCSTR, LPCSTR, const Fvector&, const Fvector&, bool),
                          CScriptSoundAction(LPCSTR, const Fvector&), CScriptSoundAction(LPCSTR, const Fvector&, const Fvector&),
                          CScriptSoundAction(LPCSTR, const Fvector&, const Fvector&, bool), CScriptSoundAction(CScriptSound&, LPCSTR, const Fvector&),
                          CScriptSoundAction(CScriptSound&, LPCSTR, const Fvector&, const Fvector&),
                          CScriptSoundAction(CScriptSound&, LPCSTR, const Fvector&, const Fvector&, bool), CScriptSoundAction(CScriptSound&, const Fvector&),
                          CScriptSoundAction(CScriptSound&, const Fvector&, const Fvector&), CScriptSoundAction(CScriptSound&, const Fvector&, const Fvector&, bool),
                          // monster specific
                          CScriptSoundAction(MonsterSound::EType), CScriptSoundAction(MonsterSound::EType, int),
                          // trader specific
                          CScriptSoundAction(LPCSTR, LPCSTR, MonsterSpace::EMonsterHeadAnimType)>(),

        // type
        "idle", sol::var(MonsterSound::eMonsterSoundIdle), "eat", sol::var(MonsterSound::eMonsterSoundEat), "attack", sol::var(MonsterSound::eMonsterSoundAggressive), "attack_hit",
        sol::var(MonsterSound::eMonsterSoundAttackHit), "take_damage", sol::var(MonsterSound::eMonsterSoundTakeDamage), "die", sol::var(MonsterSound::eMonsterSoundDie), "threaten",
        sol::var(MonsterSound::eMonsterSoundThreaten), "steal", sol::var(MonsterSound::eMonsterSoundSteal), "panic", sol::var(MonsterSound::eMonsterSoundPanic),

        "set_sound", sol::overload(sol::resolve<void(LPCSTR)>(&CScriptSoundAction::SetSound), sol::resolve<void(const CScriptSound&)>(&CScriptSoundAction::SetSound)),
        "set_sound_type", &CScriptSoundAction::SetSoundType, "set_bone", &CScriptSoundAction::SetBone, "set_position", &CScriptSoundAction::SetPosition, "set_angles",
        &CScriptSoundAction::SetAngles, "completed", sol::resolve<bool()>(&CScriptSoundAction::completed));
}
