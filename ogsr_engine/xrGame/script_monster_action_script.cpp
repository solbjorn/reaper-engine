////////////////////////////////////////////////////////////////////////////
//	Module 		: script_monster_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script monster action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_monster_action.h"
#include "script_game_object.h"

void CScriptMonsterAction::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptMonsterAction>("act", sol::no_constructor, sol::call_constructor,
                                           sol::constructors<CScriptMonsterAction(), CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction),
                                                             CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction, CScriptGameObject*)>(),

                                           // type
                                           "rest", sol::var(MonsterSpace::eGA_Rest), "eat", sol::var(MonsterSpace::eGA_Eat), "attack", sol::var(MonsterSpace::eGA_Attack), "panic",
                                           sol::var(MonsterSpace::eGA_Panic));
}
