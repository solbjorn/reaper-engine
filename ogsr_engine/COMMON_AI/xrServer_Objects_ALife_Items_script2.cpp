////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrServer_Objects_ALife_Items.h"

void CSE_ALifeItemPDA::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemPDA>("cse_alife_item_pda", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemPDA, LPCSTR>),
                                       sol::base_classes, xr_sol_bases<CSE_ALifeItemPDA>());
}

void CSE_ALifeItemDocument::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemDocument>("cse_alife_item_document", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemDocument, LPCSTR>),
                                            sol::base_classes, xr_sol_bases<CSE_ALifeItemDocument>());
}

void CSE_ALifeItemGrenade::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemGrenade>("cse_alife_item_grenade", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemGrenade, LPCSTR>),
                                           sol::base_classes, xr_sol_bases<CSE_ALifeItemGrenade>());
}

void CSE_ALifeItemExplosive::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemExplosive>("cse_alife_item_explosive", sol::no_constructor, sol::call_constructor,
                                             sol::factories(std::make_unique<CSE_ALifeItemExplosive, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeItemExplosive>());
}

void CSE_ALifeItemBolt::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemBolt>("cse_alife_item_bolt", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemBolt, LPCSTR>),
                                        sol::base_classes, xr_sol_bases<CSE_ALifeItemBolt>());
}

void CSE_ALifeItemCustomOutfit::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemCustomOutfit>("cse_alife_item_custom_outfit", sol::no_constructor, sol::call_constructor,
                                                sol::factories(std::make_unique<CSE_ALifeItemCustomOutfit, LPCSTR>), "factory", &server_factory<CSE_ALifeItemCustomOutfit>,
                                                sol::base_classes, xr_sol_bases<CSE_ALifeItemCustomOutfit>());
}

void CSE_ALifeItemWeaponMagazined::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemWeaponMagazined>("cse_alife_item_weapon_magazined", sol::no_constructor, sol::call_constructor,
                                                   sol::factories(std::make_unique<CSE_ALifeItemWeaponMagazined, LPCSTR>), "factory", &server_factory<CSE_ALifeItemWeaponMagazined>,
                                                   "current_fire_mode", &CSE_ALifeItemWeaponMagazined::m_u8CurFireMode, sol::base_classes,
                                                   xr_sol_bases<CSE_ALifeItemWeaponMagazined>());
}
