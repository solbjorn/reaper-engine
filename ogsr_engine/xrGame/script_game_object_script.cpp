////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "script_ini_file.h"
#include "sight_manager_space.h"
#include "searchlight.h"
#include "hit_immunity.h"
#include "EntityCondition.h"

void CScriptGameObject::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CSightParams>(
        "CSightParams", sol::no_constructor, sol::call_constructor, sol::constructors<CSightParams()>(),
        // sight_type
        "eSightTypeCurrentDirection", sol::var(SightManager::eSightTypeCurrentDirection), "eSightTypePathDirection", sol::var(SightManager::eSightTypePathDirection),
        "eSightTypeDirection", sol::var(SightManager::eSightTypeDirection), "eSightTypePosition", sol::var(SightManager::eSightTypePosition), "eSightTypeObject",
        sol::var(SightManager::eSightTypeObject), "eSightTypeCover", sol::var(SightManager::eSightTypeCover), "eSightTypeSearch", sol::var(SightManager::eSightTypeSearch),
        "eSightTypeLookOver", sol::var(SightManager::eSightTypeLookOver), "eSightTypeCoverLookOver", sol::var(SightManager::eSightTypeCoverLookOver), "eSightTypeFireObject",
        sol::var(SightManager::eSightTypeFireObject), "eSightTypeFirePosition", sol::var(SightManager::eSightTypeFirePosition), "eSightTypeDummy",
        sol::var(SightManager::eSightTypeDummy),

        "m_object", sol::readonly(&CSightParams::m_object), "m_vector", sol::readonly(&CSightParams::m_vector), "m_sight_type", sol::readonly(&CSightParams::m_sight_type));

    auto obj = lua.new_usertype<CScriptGameObject>("game_object", sol::no_constructor);
    static_assert(std::is_same_v<decltype(obj), CScriptGameObject::usertype>);

    script_register_trader(obj);
    script_register1(obj);
    script_register2(obj);
    script_register3(obj);

    lua.new_enum(
        "callback", "trade_start", GameObject::eTradeStart, "trade_stop", GameObject::eTradeStop, "trade_sell_buy_item", GameObject::eTradeSellBuyItem, "trade_perform_operation",
        GameObject::eTradePerformTradeOperation, "trader_global_anim_request", GameObject::eTraderGlobalAnimationRequest, "trader_head_anim_request",
        GameObject::eTraderHeadAnimationRequest, "trader_sound_end", GameObject::eTraderSoundEnd, "zone_enter", GameObject::eZoneEnter, "zone_exit", GameObject::eZoneExit,
        "level_border_exit", GameObject::eExitLevelBorder, "level_border_enter", GameObject::eEnterLevelBorder, "death", GameObject::eDeath, "patrol_path_in_point",
        GameObject::ePatrolPathInPoint, "inventory_info", GameObject::eInventoryInfo, "article_info", GameObject::eArticleInfo, "use_object", GameObject::eUseObject, "hit",
        GameObject::eHit, "sound", GameObject::eSound, "action_movement", GameObject::eActionTypeMovement, "action_watch", GameObject::eActionTypeWatch, "action_animation",
        GameObject::eActionTypeAnimation, "action_sound", GameObject::eActionTypeSound, "action_particle", GameObject::eActionTypeParticle, "action_object",
        GameObject::eActionTypeObject, "helicopter_on_point", GameObject::eHelicopterOnPoint, "helicopter_on_hit", GameObject::eHelicopterOnHit, "on_item_take",
        GameObject::eOnItemTake, "on_item_drop", GameObject::eOnItemDrop, "script_animation", GameObject::eScriptAnimation, "task_state", GameObject::eTaskStateChange,
        "take_item_from_box", GameObject::eInvBoxItemTake, "place_item_to_box", GameObject::eInvBoxItemPlace, "map_location_added", GameObject::eMapLocationAdded, "on_key_press",
        GameObject::eOnKeyPress, "on_key_release", GameObject::eOnKeyRelease, "on_key_hold", GameObject::eOnKeyHold, "on_mouse_wheel", GameObject::eOnMouseWheel, "on_mouse_move",
        GameObject::eOnMouseMove, "on_belt", GameObject::eOnItemToBelt, "on_ruck", GameObject::eOnItemToRuck, "on_slot", GameObject::eOnItemToSlot, "on_before_use_item",
        GameObject::eOnBeforeUseItem, "entity_alive_before_hit", GameObject::entity_alive_before_hit, "update_addons_visibility", GameObject::eOnUpdateAddonsVisibiility,
        "update_hud_addons_visibility", GameObject::eOnUpdateHUDAddonsVisibiility, "on_addon_init", GameObject::eOnAddonInit,

        "on_cell_item_focus", GameObject::eCellItemFocus, "on_cell_item_select", GameObject::eCellItemSelect, "on_cell_item_focus_lost", GameObject::eCellItemFocusLost,
        "on_cell_item_mouse", GameObject::eOnCellItemMouse, "on_before_save", GameObject::eBeforeSave, "on_after_save", GameObject::ePostSave, "on_level_map_click",
        GameObject::eUIMapClick, "on_pickup_item_showing", GameObject::eUIPickUpItemShowing, "on_group_items", GameObject::eUIGroupItems, "on_weapon_shell_drop",
        GameObject::eOnWpnShellDrop, "on_throw_grenade", GameObject::eOnThrowGrenade, "on_goodwill_change", GameObject::eOnGoodwillChange, "update_artefacts_on_belt",
        GameObject::eUpdateArtefactsOnBelt, "level_changer_action", GameObject::eLevelChangerAction,

        "on_attach_vehicle", GameObject::eAttachVehicle, "on_detach_vehicle", GameObject::eDetachVehicle, "on_use_vehicle", GameObject::eUseVehicle, "on_inv_box_item_take",
        GameObject::eOnInvBoxItemTake, "on_inv_box_item_drop", GameObject::eOnInvBoxItemDrop, "on_inv_box_open", GameObject::eOnInvBoxOpen, "select_pda_contact",
        GameObject::eSelectPdaContact, "on_footstep", GameObject::eOnActorFootStep, "on_actor_land", GameObject::eOnActorLand, "on_actor_jump", GameObject::eOnActorJump);

    lua.set_function("buy_condition", sol::overload(sol::resolve<void(CScriptIniFile*, LPCSTR)>(&::buy_condition), sol::resolve<void(float, float)>(&::buy_condition)));
    lua.set_function("sell_condition", sol::overload(sol::resolve<void(CScriptIniFile*, LPCSTR)>(&::sell_condition), sol::resolve<void(float, float)>(&::sell_condition)));
    lua.set_function("show_condition", &::show_condition);

    script_register4(lua);
    CHitImmunity::script_register(L);
    CEntityCondition::script_register(L);
}
