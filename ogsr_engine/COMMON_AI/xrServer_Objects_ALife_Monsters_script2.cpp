////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife_Monsters.h"

void CSE_ALifeCreatureCrow::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCreatureCrow>("cse_alife_creature_crow", sol::no_constructor, sol::call_constructor,
                                                           sol::factories(std::make_unique<CSE_ALifeCreatureCrow, LPCSTR>), sol::base_classes,
                                                           xr_sol_bases<CSE_ALifeCreatureCrow>());
}

void CSE_ALifeMonsterZombie::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeMonsterZombie>("cse_alife_monster_zombie", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifeMonsterZombie, LPCSTR>), sol::base_classes,
                                                            xr_sol_bases<CSE_ALifeMonsterZombie>());
}

void CSE_ALifeMonsterBase::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeMonsterBase>("cse_alife_monster_base", sol::no_constructor, sol::call_constructor,
                                                          sol::factories(std::make_unique<CSE_ALifeMonsterBase, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeMonsterBase>());
}

void CSE_ALifeHumanStalker::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeHumanStalker>("cse_alife_human_stalker", sol::no_constructor, sol::call_constructor,
                                                           sol::factories(std::make_unique<CSE_ALifeHumanStalker, LPCSTR>), sol::base_classes,
                                                           xr_sol_bases<CSE_ALifeHumanStalker>());
}
