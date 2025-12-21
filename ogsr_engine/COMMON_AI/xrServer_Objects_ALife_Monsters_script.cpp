////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Monsters_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server monsters for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrServer_Objects_ALife_Monsters.h"

namespace
{
[[nodiscard]] gsl::czstring profile_name_script(CSE_ALifeTraderAbstract* ta) { return ta->character_profile().c_str(); }
} // namespace

void CSE_ALifeTraderAbstract::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeTraderAbstract>("cse_alife_trader_abstract", sol::no_constructor,
#ifdef XRGAME_EXPORTS
                                              "community", &CSE_ALifeTraderAbstract::CommunityName, "profile_name", &profile_name_script, "rank", &CSE_ALifeTraderAbstract::Rank,
                                              "reputation", &CSE_ALifeTraderAbstract::Reputation,
#endif // XRGAME_EXPORTS
                                              "money", &CSE_ALifeTraderAbstract::m_dwMoney, "character_name", &CSE_ALifeTraderAbstract::m_character_name);
}

namespace
{
[[nodiscard]] bool CSE_AlifeTrader_alive(CSE_ALifeTrader&) { return true; }

[[nodiscard]] ALife::_OBJECT_ID CSE_AlifeTrader__smart_terrain_id(CSE_ALifeTrader* trader)
{
    THROW(trader);
    return std::numeric_limits<ALife::_OBJECT_ID>::max();
}
} // namespace

void CSE_ALifeTrader::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeTrader>("cse_alife_trader", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeTrader, LPCSTR>), "factory",
                                      &xr::server_factory<CSE_ALifeTrader>, "alive", &CSE_AlifeTrader_alive, "smart_terrain_id", &CSE_AlifeTrader__smart_terrain_id,
                                      sol::base_classes, xr::sol_bases<CSE_ALifeTrader>());
}

void CSE_ALifeCustomZone::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeCustomZone>("cse_custom_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeCustomZone, LPCSTR>),
                                          sol::base_classes, xr::sol_bases<CSE_ALifeCustomZone>());
}

void CSE_ALifeAnomalousZone::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeAnomalousZone>("cse_anomalous_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeAnomalousZone, LPCSTR>),
                                             "factory", &xr::server_factory<CSE_ALifeAnomalousZone>, "spawn_artefacts", &CSE_ALifeAnomalousZone::spawn_artefacts, sol::base_classes,
                                             xr::sol_bases<CSE_ALifeAnomalousZone>());
}
