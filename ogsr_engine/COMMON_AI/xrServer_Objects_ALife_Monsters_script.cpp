////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife_Monsters.h"

static LPCSTR profile_name_script(CSE_ALifeTraderAbstract* ta) { return *ta->character_profile(); }
static LPCSTR character_name_script(CSE_ALifeTraderAbstract* ta) { return ta->m_character_name.c_str(); }
static void set_character_name_script(CSE_ALifeTraderAbstract* ta, LPCSTR name) { ta->m_character_name = name; }

void CSE_ALifeTraderAbstract::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeTraderAbstract>("cse_alife_trader_abstract", sol::no_constructor,
#ifdef XRGAME_EXPORTS
                                                             "community", &CSE_ALifeTraderAbstract::CommunityName, "profile_name", &profile_name_script, "rank",
                                                             &CSE_ALifeTraderAbstract::Rank, "reputation", &CSE_ALifeTraderAbstract::Reputation,
#endif // XRGAME_EXPORTS
                                                             "money", &CSE_ALifeTraderAbstract::m_dwMoney, "character_name",
                                                             sol::property(&character_name_script, &set_character_name_script));
}

static ALife::_OBJECT_ID CSE_AlifeTrader__smart_terrain_id(CSE_ALifeTrader* trader)
{
    THROW(trader);
    return 0xffff;
}

void CSE_ALifeTrader::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeTrader>("cse_alife_trader", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeTrader, LPCSTR>),
                                                     "smart_terrain_id", &CSE_AlifeTrader__smart_terrain_id, sol::base_classes, xr_sol_bases<CSE_ALifeTrader>());
}

void CSE_ALifeCustomZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCustomZone>("cse_custom_zone", sol::no_constructor, sol::call_constructor,
                                                         sol::factories(std::make_unique<CSE_ALifeCustomZone, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeCustomZone>());
}

void CSE_ALifeAnomalousZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeAnomalousZone>("cse_anomalous_zone", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifeAnomalousZone, LPCSTR>), "spawn_artefacts",
                                                            &CSE_ALifeAnomalousZone::spawn_artefacts, sol::base_classes, xr_sol_bases<CSE_ALifeAnomalousZone>());
}
