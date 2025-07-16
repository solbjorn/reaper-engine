////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script3.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife_Monsters.h"

void CSE_ALifeCreatureActor::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCreatureActor>("cse_alife_creature_actor", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifeCreatureActor, LPCSTR>), sol::base_classes,
                                                            xr_sol_bases<CSE_ALifeCreatureActor>());
}

void CSE_ALifeTorridZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeTorridZone>("cse_torrid_zone", sol::no_constructor, sol::call_constructor,
                                                         sol::factories(std::make_unique<CSE_ALifeTorridZone, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeTorridZone>());
}

void CSE_ALifeZoneVisual::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeZoneVisual>("cse_zone_visual", sol::no_constructor, sol::call_constructor,
                                                         sol::factories(std::make_unique<CSE_ALifeZoneVisual, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeZoneVisual>());
}

void CSE_ALifeCreaturePhantom::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCreaturePhantom>("cse_alife_creature_phantom", sol::no_constructor, sol::call_constructor,
                                                              sol::factories(std::make_unique<CSE_ALifeCreaturePhantom, LPCSTR>), sol::base_classes,
                                                              xr_sol_bases<CSE_ALifeCreaturePhantom>());
}

void CSE_ALifeCreatureAbstract::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCreatureAbstract>("cse_alife_creature_abstract", sol::no_constructor, sol::call_constructor,
                                                               sol::factories(std::make_unique<CSE_ALifeCreatureAbstract, LPCSTR>), "health", &CSE_ALifeCreatureAbstract::g_Health,
                                                               "set_health", &CSE_ALifeCreatureAbstract::s_Health, "alive", &CSE_ALifeCreatureAbstract::g_Alive, "team",
                                                               &CSE_ALifeCreatureAbstract::s_team, "squad", &CSE_ALifeCreatureAbstract::s_squad, "group",
                                                               &CSE_ALifeCreatureAbstract::s_group, sol::base_classes, xr_sol_bases<CSE_ALifeCreatureAbstract>());
}

void CSE_ALifeOnlineOfflineGroup::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeOnlineOfflineGroup>("cse_alife_online_offline_group", sol::no_constructor, sol::call_constructor,
                                                                 sol::factories(std::make_unique<CSE_ALifeOnlineOfflineGroup, LPCSTR>), sol::base_classes,
                                                                 xr_sol_bases<CSE_ALifeOnlineOfflineGroup>());
}
