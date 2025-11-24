////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script3.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_game_object.h"

#include "alife_space.h"
#include "script_entity_space.h"
#include "movement_manager_space.h"
#include "pda_space.h"
#include "memory_space.h"
#include "cover_point.h"
#include "script_hit.h"
#include "script_binder_object.h"
#include "script_ini_file.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "script_entity_action.h"
#include "action_planner.h"
#include "PhysicsShell.h"
#include "helicopter.h"
#include "HangingLamp.h"
#include "holder_custom.h"
#include "script_zone.h"
#include "relation_registry.h"
#include "GameTask.h"
#include "car.h"
#include "sight_manager_space.h"
#include "../xr_3da/IGame_Persistent.h"

void CScriptGameObject::script_register2(CScriptGameObject::usertype& lua)
{
    xr::sol_set(
        lua, "add_sound",
        sol::overload(sol::resolve<u32(LPCSTR, u32, ESoundTypes, u32, u32, u32, LPCSTR)>(&CScriptGameObject::add_sound),
                      sol::resolve<u32(LPCSTR, u32, ESoundTypes, u32, u32, u32)>(&CScriptGameObject::add_sound)),
        "remove_sound", &CScriptGameObject::remove_sound, "set_sound_mask", &CScriptGameObject::set_sound_mask, "play_sound",
        sol::overload(sol::resolve<void(u32, u32, u32, u32, u32, u32)>(&CScriptGameObject::play_sound), sol::resolve<void(u32, u32, u32, u32, u32)>(&CScriptGameObject::play_sound),
                      sol::resolve<void(u32, u32, u32, u32)>(&CScriptGameObject::play_sound), sol::resolve<void(u32, u32, u32)>(&CScriptGameObject::play_sound),
                      sol::resolve<void(u32, u32)>(&CScriptGameObject::play_sound), sol::resolve<void(u32)>(&CScriptGameObject::play_sound)),

        "binded_object", &CScriptGameObject::binded_object, "set_previous_point", &CScriptGameObject::set_previous_point, "set_start_point", &CScriptGameObject::set_start_point,
        "get_current_point_index", &CScriptGameObject::get_current_patrol_point_index, "path_completed", &CScriptGameObject::path_completed, "patrol_path_make_inactual",
        &CScriptGameObject::patrol_path_make_inactual, "enable_memory_object", &CScriptGameObject::enable_memory_object, "remove_memory_object",
        &CScriptGameObject::remove_memory_object, "active_sound_count",
        sol::overload(sol::resolve<int()>(&CScriptGameObject::active_sound_count), sol::resolve<int(bool)>(&CScriptGameObject::active_sound_count)),

        "best_cover",
        sol::overload(sol::resolve<const CCoverPoint*(const Fvector&, const Fvector&, f32, f32, f32, sol::function)>(&CScriptGameObject::best_cover),
                      sol::resolve<const CCoverPoint*(const Fvector&, const Fvector&, f32, f32, f32)>(&CScriptGameObject::best_cover)),
        "safe_cover",
        sol::overload(sol::resolve<const CCoverPoint*(const Fvector&, f32, f32, sol::function)>(&CScriptGameObject::safe_cover),
                      sol::resolve<const CCoverPoint*(const Fvector&, f32, f32)>(&CScriptGameObject::safe_cover)),
        "ambush_cover",
        sol::overload(sol::resolve<const CCoverPoint*(const Fvector&, const Fvector&, f32, f32, sol::function)>(&CScriptGameObject::ambush_cover),
                      sol::resolve<const CCoverPoint*(const Fvector&, const Fvector&, f32, f32)>(&CScriptGameObject::ambush_cover)),
        "angle_cover",
        sol::overload(sol::resolve<const CCoverPoint*(const Fvector&, f32, const Fvector&, f32, f32, u32, sol::function)>(&CScriptGameObject::angle_cover),
                      sol::resolve<const CCoverPoint*(const Fvector&, f32, const Fvector&, f32, f32, u32)>(&CScriptGameObject::angle_cover)),

        "spawn_ini", &CScriptGameObject::spawn_ini, "memory_visible_objects", &CScriptGameObject::memory_visible_objects, "memory_sound_objects",
        &CScriptGameObject::memory_sound_objects, "memory_hit_objects", &CScriptGameObject::memory_hit_objects, "not_yet_visible_objects",
        &CScriptGameObject::not_yet_visible_objects,

        "visibility_threshold", &CScriptGameObject::visibility_threshold, "enable_vision", &CScriptGameObject::enable_vision, "vision_enabled", &CScriptGameObject::vision_enabled,
        "set_sound_threshold", &CScriptGameObject::set_sound_threshold, "restore_sound_threshold", &CScriptGameObject::restore_sound_threshold,

        // sight manager
        "set_sight",
        sol::overload(sol::resolve<void(SightManager::ESightType, const Fvector*, u32)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(SightManager::ESightType, bool, bool path)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(SightManager::ESightType, const Fvector&, bool)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(SightManager::ESightType, const Fvector*)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(CScriptGameObject*, bool, bool, bool)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(CScriptGameObject*, bool, bool)>(&CScriptGameObject::set_sight),
                      sol::resolve<void(CScriptGameObject*, bool)>(&CScriptGameObject::set_sight), sol::resolve<void(CScriptGameObject*)>(&CScriptGameObject::set_sight)),

        // object handler
        "set_item",
        sol::overload(sol::resolve<void(MonsterSpace::EObjectAction, CScriptGameObject*, u32, u32)>(&CScriptGameObject::set_item),
                      sol::resolve<void(MonsterSpace::EObjectAction, CScriptGameObject*, u32)>(&CScriptGameObject::set_item),
                      sol::resolve<void(MonsterSpace::EObjectAction, CScriptGameObject*)>(&CScriptGameObject::set_item),
                      sol::resolve<void(MonsterSpace::EObjectAction)>(&CScriptGameObject::set_item)),

        "bone_position", &CScriptGameObject::bone_position, "is_body_turning", &CScriptGameObject::is_body_turning,

        //////////////////////////////////////////////////////////////////////////
        // Space restrictions
        //////////////////////////////////////////////////////////////////////////
        "add_restrictions", &CScriptGameObject::add_restrictions, "remove_restrictions", &CScriptGameObject::remove_restrictions, "remove_all_restrictions",
        &CScriptGameObject::remove_all_restrictions, "in_restrictions", &CScriptGameObject::in_restrictions, "out_restrictions", &CScriptGameObject::out_restrictions,
        "base_in_restrictions", &CScriptGameObject::base_in_restrictions, "base_out_restrictions", &CScriptGameObject::base_out_restrictions, "accessible",
        sol::overload(&CScriptGameObject::accessible_position, &CScriptGameObject::accessible_vertex_id), "accessible_nearest", &CScriptGameObject::accessible_nearest,

        //////////////////////////////////////////////////////////////////////////
        "enable_attachable_item", &CScriptGameObject::enable_attachable_item, "attachable_item_enabled", &CScriptGameObject::attachable_item_enabled, "weapon_strapped",
        &CScriptGameObject::weapon_strapped, "weapon_unstrapped", &CScriptGameObject::weapon_unstrapped,

        //////////////////////////////////////////////////////////////////////////
        // inventory owner
        //////////////////////////////////////////////////////////////////////////

        "dialog_pda_msg", sol::var(ePdaMsgDialog), "info_pda_msg", sol::var(ePdaMsgInfo), "no_pda_msg", sol::var(ePdaMsgMax),

        "give_info_portion", &CScriptGameObject::GiveInfoPortion, "disable_info_portion", &CScriptGameObject::DisableInfoPortion, "give_game_news", &GiveGameNews, "give_phrase",
        &AddTalkMessage, "give_talk_message", &AddIconedTalkMessage, "has_info", &CScriptGameObject::HasInfo, "dont_has_info", &CScriptGameObject::DontHasInfo, "get_info_time",
        &CScriptGameObject::GetInfoTime,

        "get_task_state", &CScriptGameObject::GetGameTaskState, "set_task_state", &CScriptGameObject::SetGameTaskState, "give_task", &CScriptGameObject::GiveTaskToActor,
        "is_talking", &CScriptGameObject::IsTalking, "stop_talk", &CScriptGameObject::StopTalk, "enable_talk", &CScriptGameObject::EnableTalk, "disable_talk",
        &CScriptGameObject::DisableTalk, "is_talk_enabled", &CScriptGameObject::IsTalkEnabled, "enable_trade", &CScriptGameObject::EnableTrade, "disable_trade",
        &CScriptGameObject::DisableTrade, "is_trade_enabled", &CScriptGameObject::IsTradeEnabled,

        "inventory_for_each", &CScriptGameObject::ForEachInventoryItems, "drop_item", &CScriptGameObject::DropItem, "drop_item_and_teleport",
        &CScriptGameObject::DropItemAndTeleport, "transfer_item", &CScriptGameObject::TransferItem, "transfer_money", &CScriptGameObject::TransferMoney, "give_money",
        &CScriptGameObject::GiveMoney, "money", &CScriptGameObject::Money, "set_money", &CScriptGameObject::SetMoney,

        "switch_to_trade", &CScriptGameObject::SwitchToTrade, "switch_to_talk", &CScriptGameObject::SwitchToTalk, "run_talk_dialog", &CScriptGameObject::RunTalkDialog,
        "hide_weapon", &CScriptGameObject::HideWeapon, "restore_weapon", &CScriptGameObject::RestoreWeapon, "set_start_dialog", &CScriptGameObject::SetStartDialog,
        "get_start_dialog", &CScriptGameObject::GetStartDialog, "restore_default_start_dialog", &CScriptGameObject::RestoreDefaultStartDialog,

        //////////////////////////////////////////////////////////////////////////

        "goodwill", &CScriptGameObject::GetGoodwill, "set_goodwill", &CScriptGameObject::SetGoodwill, "change_goodwill", &CScriptGameObject::ChangeGoodwill, "general_goodwill",
        &CScriptGameObject::GetAttitude, "set_relation", &CScriptGameObject::SetRelation,

        //////////////////////////////////////////////////////////////////////////
        "profile_name", &CScriptGameObject::ProfileName, "character_name", &CScriptGameObject::CharacterName, "character_rank", &CScriptGameObject::CharacterRank,
        "set_character_rank", &CScriptGameObject::SetCharacterRank, "change_character_rank", &CScriptGameObject::ChangeCharacterRank, "character_reputation",
        &CScriptGameObject::CharacterReputation, "change_character_reputation", &CScriptGameObject::ChangeCharacterReputation, "character_community",
        &CScriptGameObject::CharacterCommunity, "set_character_community", &CScriptGameObject::SetCharacterCommunity, "get_actor_relation_flags",
        &CScriptGameObject::get_actor_relation_flags, "set_actor_relation_flags", &CScriptGameObject::set_actor_relation_flags, "sound_voice_prefix",
        &CScriptGameObject::sound_voice_prefix,

        "relation_attack", sol::var(RELATION_REGISTRY::ATTACK), "relation_fight_help_monster", sol::var(RELATION_REGISTRY::FIGHT_HELP_MONSTER), "relation_fight_help_human",
        sol::var(RELATION_REGISTRY::FIGHT_HELP_HUMAN), "relation_kill", sol::var(RELATION_REGISTRY::KILL),

        // CustomZone
        "enable_anomaly", &CScriptGameObject::EnableAnomaly, "disable_anomaly", &CScriptGameObject::DisableAnomaly, "get_anomaly_power", &CScriptGameObject::GetAnomalyPower,
        "set_anomaly_power", &CScriptGameObject::SetAnomalyPower,

        // HELICOPTER
        "get_helicopter", &CScriptGameObject::get_helicopter, "get_car", &CScriptGameObject::get_car, "get_hanging_lamp", &CScriptGameObject::get_hanging_lamp, "get_physics_shell",
        &CScriptGameObject::get_physics_shell, "get_holder_class", &CScriptGameObject::get_custom_holder, "get_current_holder", &CScriptGameObject::get_current_holder,
        // usable object
        "set_tip_text", &CScriptGameObject::SetTipText, "set_tip_text_default", &CScriptGameObject::SetTipTextDefault, "set_nonscript_usable",
        &CScriptGameObject::SetNonscriptUsable, "get_nonscript_usable", &CScriptGameObject::GetNonscriptUsable,

        // Script Zone
        "active_zone_contact", &CScriptGameObject::active_zone_contact, "inside",
        sol::overload(sol::resolve<bool(const Fvector&, float) const>(&CScriptGameObject::inside), sol::resolve<bool(const Fvector&) const>(&CScriptGameObject::inside)),

        "set_fastcall", &CScriptGameObject::set_fastcall, "set_const_force", &CScriptGameObject::set_const_force, "info_add", &CScriptGameObject::info_add, "info_clear",
        &CScriptGameObject::info_clear, "is_inv_box_empty", &CScriptGameObject::IsInvBoxEmpty,

        // monster jumper
        "jump", &CScriptGameObject::jump, "make_object_visible_somewhen", &CScriptGameObject::make_object_visible_somewhen,

        "buy_condition",
        sol::overload(sol::resolve<void(CScriptIniFile*, LPCSTR)>(&CScriptGameObject::buy_condition), sol::resolve<void(float, float)>(&CScriptGameObject::buy_condition)),
        "show_condition", &CScriptGameObject::show_condition, "sell_condition",
        sol::overload(sol::resolve<void(CScriptIniFile*, LPCSTR)>(&CScriptGameObject::sell_condition), sol::resolve<void(float, float)>(&CScriptGameObject::sell_condition)),
        "buy_supplies", &CScriptGameObject::buy_supplies,

        "sound_prefix", sol::overload(sol::resolve<LPCSTR() const>(&CScriptGameObject::sound_prefix), sol::resolve<void(LPCSTR)>(&CScriptGameObject::sound_prefix)),
        "location_on_path", &CScriptGameObject::location_on_path, "wounded",
        sol::overload(sol::resolve<bool() const>(&CScriptGameObject::wounded), sol::resolve<void(bool)>(&CScriptGameObject::wounded)),

        "iterate_inventory", &CScriptGameObject::IterateInventory, "iterate_belt", &CScriptGameObject::IterateBelt, "iterate_ruck", &CScriptGameObject::IterateRuck,
        "mark_item_dropped", &CScriptGameObject::MarkItemDropped, "marked_dropped", &CScriptGameObject::MarkedDropped, "unload_magazine", &CScriptGameObject::UnloadMagazine,

        "sight_params", &CScriptGameObject::sight_params, "movement_enabled", &CScriptGameObject::enable_movement, "movement_enabled", &CScriptGameObject::movement_enabled,
        "critically_wounded", &CScriptGameObject::critically_wounded,

        /************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
        // инвентарь
        "get_actor_max_weight", &CScriptGameObject::GetActorMaxWeight, "set_actor_max_weight", &CScriptGameObject::SetActorMaxWeight, "get_actor_max_walk_weight",
        &CScriptGameObject::GetActorMaxWalkWeight, "set_actor_max_walk_weight", &CScriptGameObject::SetActorMaxWalkWeight, "get_additional_max_weight",
        &CScriptGameObject::GetAdditionalMaxWeight, "set_additional_max_weight", &CScriptGameObject::SetAdditionalMaxWeight, "get_additional_max_walk_weight",
        &CScriptGameObject::GetAdditionalMaxWalkWeight, "set_additional_max_walk_weight", &CScriptGameObject::SetAdditionalMaxWalkWeight, "get_total_weight",
        &CScriptGameObject::GetTotalWeight, "weight", &CScriptGameObject::Weight,
        /*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/

        /*************************************************** added by Cribbledirge START ***************************************************/
        "is_actor_outdoors", &CScriptGameObject::IsActorOutdoors,
        /**************************************************** added by Cribbledirge END ****************************************************/
        // проверка что актор под каким-либо укрытием
        "is_ActorHide", [] { return g_pGamePersistent->IsActorInHideout(); },

        // KD
        // functions for CInventoryOwner class
        "item_on_belt", &CScriptGameObject::ItemOnBelt, "item_in_ruck", &CScriptGameObject::ItemInRuck, "is_on_belt", &CScriptGameObject::IsOnBelt, "is_in_ruck",
        &CScriptGameObject::IsInRuck, "is_in_slot", &CScriptGameObject::IsInSlot, "move_to_ruck", &CScriptGameObject::MoveToRuck, "move_to_belt", &CScriptGameObject::MoveToBelt,
        "move_to_slot", &CScriptGameObject::MoveToSlot, "belt_count", &CScriptGameObject::BeltSize, "ruck_count", &CScriptGameObject::RuckSize, "invalidate_inventory",
        &CScriptGameObject::InvalidateInventory, "get_slot", &CScriptGameObject::GetSlot,

        // functions for CInventoryItem class
        "set_inventory_item_flags", &CScriptGameObject::SetIIFlags, "get_inventory_item_flags", &CScriptGameObject::GetIIFlags,

        // functions for object testing
        "is_game_object", &CScriptGameObject::IsGameObject, "is_car", &CScriptGameObject::IsCar, "is_helicopter", &CScriptGameObject::IsHeli, "is_holder",
        &CScriptGameObject::IsHolderCustom, "is_entity_alive", &CScriptGameObject::IsEntityAlive, "is_inventory_item", &CScriptGameObject::IsInventoryItem, "is_inventory_owner",
        &CScriptGameObject::IsInventoryOwner, "is_actor", &CScriptGameObject::IsActor, "is_custom_monster", &CScriptGameObject::IsCustomMonster, "is_weapon",
        &CScriptGameObject::IsWeapon, "is_weapon_magazined", &CScriptGameObject::IsWeaponMagazined, "is_weapon_shotgun", &CScriptGameObject::IsWeaponShotgun, "is_weapon_gl",
        &CScriptGameObject::IsWeaponGL, "is_binoculars", &CScriptGameObject::IsWeaponBinoculars, "is_medkit", &CScriptGameObject::IsMedkit, "is_eatable_item",
        &CScriptGameObject::IsEatableItem, "is_antirad", &CScriptGameObject::IsAntirad, "is_outfit", &CScriptGameObject::IsCustomOutfit, "is_scope", &CScriptGameObject::IsScope,
        "is_silencer", &CScriptGameObject::IsSilencer, "is_grenade_launcher", &CScriptGameObject::IsGrenadeLauncher, "is_space_restrictor", &CScriptGameObject::IsSpaceRestrictor,
        "is_stalker", &CScriptGameObject::IsStalker, "is_anomaly", &CScriptGameObject::IsAnomaly, "is_monster", &CScriptGameObject::IsMonster, "is_explosive",
        &CScriptGameObject::IsExplosive, "is_script_zone", &CScriptGameObject::IsScriptZone, "is_projector", &CScriptGameObject::IsProjector, "is_lamp", &CScriptGameObject::IsLamp,
        "is_trader", &CScriptGameObject::IsTrader, "is_hud_item", &CScriptGameObject::IsHudItem, "is_food_item", &CScriptGameObject::IsFoodItem, "is_artefact",
        &CScriptGameObject::IsArtefact, "is_ammo", &CScriptGameObject::IsAmmo, "is_missile", &CScriptGameObject::IsMissile, "is_physics_shell_holder",
        &CScriptGameObject::IsPhysicsShellHolder, "is_grenade", &CScriptGameObject::IsGrenade, "is_bottle_item", &CScriptGameObject::IsBottleItem, "is_torch",
        &CScriptGameObject::IsTorch, "is_inventory_box", &CScriptGameObject::IsInventoryBox, "is_knife", &CScriptGameObject::IsKnife, "is_pistol", &CScriptGameObject::IsPistol,

        "set_camera_direction", &CScriptGameObject::SetActorCamDir, "update_condition", &CScriptGameObject::UpdateCondition, "heal_wounds", &CScriptGameObject::ChangeBleeding,
        "add_wounds", &CScriptGameObject::AddWound, "get_weight", &CScriptGameObject::GetItemWeight, "inv_box_count", &CScriptGameObject::InvBoxCount, "open_inventory_box",
        &CScriptGameObject::OpenInvBox, "object_from_inv_box", &CScriptGameObject::ObjectFromInvBox, "get_camera_fov", &CScriptGameObject::GetCamFOV, "set_camera_fov",
        &CScriptGameObject::SetCamFOV,

        "set_max_weight", &CScriptGameObject::SetMaxWeight, "set_max_walk_weight", &CScriptGameObject::SetMaxWalkWeight, "get_max_weight", &CScriptGameObject::GetMaxWeight,
        "get_max_walk_weight", &CScriptGameObject::GetMaxWalkWeight, "get_inventory_weight", &CScriptGameObject::GetInventoryWeight, "calculate_item_price",
        &CScriptGameObject::CalcItemPrice,

        "get_shape_radius", &CScriptGameObject::GetShapeRadius, "get_visual_name", &CScriptGameObject::GetVisualName, "get_visual_ini", &CScriptGameObject::GetVisIni,

        "set_bone_visible", &CScriptGameObject::SetBoneVisible, "set_hud_bone_visible", &CScriptGameObject::SetHudBoneVisible, "get_bone_visible",
        &CScriptGameObject::GetBoneVisible, "get_hud_bone_visible", &CScriptGameObject::GetHudBoneVisible, "get_bone_id", &CScriptGameObject::GetBoneID,

        "get_binoc_zoom_factor", &CScriptGameObject::GetBinocZoomFactor, "set_binoc_zoom_factor", &CScriptGameObject::SetBinocZoomFactor, "get_zoom_factor",
        &CScriptGameObject::GetZoomFactor, "get_addon_flags", &CScriptGameObject::GetAddonFlags, "set_addon_flags", &CScriptGameObject::SetAddonFlags, "get_magazine_size",
        &CScriptGameObject::GetMagazineSize, "set_magazine_size", &CScriptGameObject::SetMagazineSize, "get_grenade_launcher_status", &CScriptGameObject::GrenadeLauncherAttachable,
        "get_ammo_type", &CScriptGameObject::GetAmmoType, "get_underbarrel_ammo_type", &CScriptGameObject::GetUnderbarrelAmmoType, "get_ammo_in_magazine2",
        &CScriptGameObject::GetAmmoElapsed2, "get_gl_mode", &CScriptGameObject::GetGLMode, "get_current_ammo", &CScriptGameObject::GetCurrAmmo, "set_ammo_box_curr",
        &CScriptGameObject::SetAmmoBoxCurr, "get_ammo_box_size", &CScriptGameObject::GetAmmoBoxSize, "set_ammo_box_size", &CScriptGameObject::SetAmmoBoxSize,

        "get_holder", &CScriptGameObject::GetActorHolder, "get_camera", &CScriptGameObject::GetCarCamera, "zoom_mode", &CScriptGameObject::ZoomMode, "reset_state",
        &CScriptGameObject::ResetState,

        // для CEatableItem
        "zero_effects", &CScriptGameObject::ZeroEffects, "set_radiation_influence", &CScriptGameObject::SetRadiationInfluence,
        // для актора - иммунитеты
        "set_additional_radiation_protection", &CScriptGameObject::SetDrugRadProtection, "set_additional_telepatic_protection", &CScriptGameObject::SetDrugPsyProtection,
        // KD

        // by Real Wolf 11.07.2014
        "get_cell_item", &CScriptGameObject::GetCellItem, "get_bone_name", &CScriptGameObject::GetBoneName,

        "get_hud_item_state", &CScriptGameObject::GetHudItemState, "radius", &CScriptGameObject::GetRadius, "play_hud_motion",
        sol::overload(sol::resolve<u32(LPCSTR)>(&CScriptGameObject::play_hud_animation), sol::resolve<u32(LPCSTR, bool, u32, float)>(&CScriptGameObject::play_hud_animation)),

        "add_feel_touch",
        sol::overload(sol::resolve<void(f32, sol::object, sol::function, sol::function)>(&CScriptGameObject::addFeelTouch),
                      sol::resolve<void(f32, sol::object, sol::function)>(&CScriptGameObject::addFeelTouch)),
        "remove_feel_touch",
        sol::overload(sol::resolve<void(sol::object, sol::function, sol::function)>(&CScriptGameObject::removeFeelTouch),
                      sol::resolve<void(sol::object, sol::function)>(&CScriptGameObject::removeFeelTouch)));
}
