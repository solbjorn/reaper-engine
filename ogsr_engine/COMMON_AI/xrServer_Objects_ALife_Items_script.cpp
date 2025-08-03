////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrServer_Objects_ALife_Items.h"

void CSE_ALifeInventoryItem::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeInventoryItem>("cse_alife_inventory_item", sol::no_constructor, "item_condition", &CSE_ALifeInventoryItem::m_fCondition);
}

void CSE_ALifeItem::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItem>("cse_alife_item", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItem, LPCSTR>), "factory",
                                    &server_factory<CSE_ALifeItem>, sol::base_classes, xr_sol_bases<CSE_ALifeItem>());
}

void CSE_ALifeItemTorch::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemTorch>("cse_alife_item_torch", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemTorch, LPCSTR>),
                                         "factory", &server_factory<CSE_ALifeItemTorch>, sol::base_classes, xr_sol_bases<CSE_ALifeItemTorch>());
}

void CSE_ALifeItemAmmo::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemAmmo>("cse_alife_item_ammo", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemAmmo, LPCSTR>),
                                        sol::base_classes, xr_sol_bases<CSE_ALifeItemAmmo>());
}

void CSE_ALifeItemWeapon::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemWeapon>("cse_alife_item_weapon", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemWeapon, LPCSTR>),
                                          "factory", &server_factory<CSE_ALifeItemWeapon>, "ammo_current", &CSE_ALifeItemWeapon::a_current, "ammo_elapsed",
                                          &CSE_ALifeItemWeapon::a_elapsed, "weapon_state", &CSE_ALifeItemWeapon::wpn_state, "addon_flags", &CSE_ALifeItemWeapon::m_addon_flags,
                                          "ammo_type", &CSE_ALifeItemWeapon::ammo_type, sol::base_classes, xr_sol_bases<CSE_ALifeItemWeapon>());
}

void CSE_ALifeItemWeaponShotGun::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemWeaponShotGun>("cse_alife_item_weapon_shotgun", sol::no_constructor, sol::call_constructor,
                                                 sol::factories(std::make_unique<CSE_ALifeItemWeaponShotGun, LPCSTR>), "factory", &server_factory<CSE_ALifeItemWeaponShotGun>,
                                                 sol::base_classes, xr_sol_bases<CSE_ALifeItemWeaponShotGun>());
}

void CSE_ALifeItemDetector::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemDetector>("cse_alife_item_detector", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemDetector, LPCSTR>),
                                            sol::base_classes, xr_sol_bases<CSE_ALifeItemDetector>());
}

void CSE_ALifeItemArtefact::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeItemArtefact>("cse_alife_item_artefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemArtefact, LPCSTR>),
                                            "factory", &server_factory<CSE_ALifeItemArtefact>, sol::base_classes, xr_sol_bases<CSE_ALifeItemArtefact>());
}
