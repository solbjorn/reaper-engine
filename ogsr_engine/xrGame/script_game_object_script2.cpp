////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script2.cpp
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
#include "script_zone.h"
#include "relation_registry.h"
#include "danger_object.h"
#include "Torch.h"
#include "searchlight.h"
#include "game_object_space.h"
#include "detail_path_manager_space.h"
#include "patrol_path_manager_space.h"

extern CScriptActionPlanner* script_action_planner(CScriptGameObject* obj);

void CScriptGameObject::script_register1(CScriptGameObject::usertype& lua)
{
    xr_sol_set(
        lua,
        // relation
        "friend", sol::var(ALife::eRelationTypeFriend), "neutral", sol::var(ALife::eRelationTypeNeutral), "enemy", sol::var(ALife::eRelationTypeEnemy), "worst_enemy",
        sol::var(ALife::eRelationTypeWorstEnemy), "dummy", sol::var(ALife::eRelationTypeDummy),
        // action_types
        "movement", sol::var(ScriptEntity::eActionTypeMovement), "watch", sol::var(ScriptEntity::eActionTypeWatch), "animation", sol::var(ScriptEntity::eActionTypeAnimation),
        "sound", sol::var(ScriptEntity::eActionTypeSound), "particle", sol::var(ScriptEntity::eActionTypeParticle), "object", sol::var(ScriptEntity::eActionTypeObject),
        "action_type_count", sol::var(ScriptEntity::eActionTypeCount),
        // EPathType
        "game_path", sol::var(MovementManager::ePathTypeGamePath), "level_path", sol::var(MovementManager::ePathTypeLevelPath), "patrol_path",
        sol::var(MovementManager::ePathTypePatrolPath), "no_path", sol::var(MovementManager::ePathTypeNoPath),

        "health", sol::property(&CScriptGameObject::GetHealth, &CScriptGameObject::SetHealth), "psy_health",
        sol::property(&CScriptGameObject::GetPsyHealth, &CScriptGameObject::SetPsyHealth), "power", sol::property(&CScriptGameObject::GetPower, &CScriptGameObject::SetPower),
        "satiety", sol::property(&CScriptGameObject::GetSatiety, &CScriptGameObject::SetSatiety), "radiation",
        sol::property(&CScriptGameObject::GetRadiation, &CScriptGameObject::SetRadiation), "morale", sol::property(&CScriptGameObject::GetMorale, &CScriptGameObject::SetMorale),
        // KD
        "alcohol", sol::property(&CScriptGameObject::GetAlcohol, &CScriptGameObject::SetAlcohol), "thirst",
        sol::property(&CScriptGameObject::GetThirst, &CScriptGameObject::SetThirst), "max_power", sol::property(&CScriptGameObject::GetMaxPower, &CScriptGameObject::SetMaxPower),
        // Actor State
        "exo_factor", sol::property(&CScriptGameObject::GetActorExoFactor, &CScriptGameObject::SetActorExoFactor),

        // Other
        "get_bleeding", &CScriptGameObject::GetBleeding, "center", &CScriptGameObject::Center, "position", &CScriptGameObject::Position, "direction", &CScriptGameObject::Direction,
        "clsid", &CScriptGameObject::clsid, "id", &CScriptGameObject::ID, "story_id", &CScriptGameObject::story_id, "section", &CScriptGameObject::Section, "name",
        &CScriptGameObject::Name, "parent", &CScriptGameObject::Parent, "mass", &CScriptGameObject::Mass, "cost", &CScriptGameObject::Cost, "condition",
        &CScriptGameObject::GetCondition, "set_condition", &CScriptGameObject::SetCondition, "death_time", &CScriptGameObject::DeathTime, "max_health",
        &CScriptGameObject::MaxHealth, "accuracy", &CScriptGameObject::Accuracy, "alive", &CScriptGameObject::Alive, "team", &CScriptGameObject::Team, "squad",
        &CScriptGameObject::Squad, "group", &CScriptGameObject::Group, "change_team", sol::resolve<void(u8, u8, u8)>(&CScriptGameObject::ChangeTeam), "kill",
        &CScriptGameObject::Kill, "hit", &CScriptGameObject::Hit, "play_cycle",
        sol::overload(sol::resolve<void(LPCSTR)>(&CScriptGameObject::play_cycle), sol::resolve<void(LPCSTR, bool)>(&CScriptGameObject::play_cycle)),

        "fov", &CScriptGameObject::GetFOV, "range", &CScriptGameObject::GetRange, "relation", &CScriptGameObject::GetRelationType, "is_relation_enemy",
        &CScriptGameObject::IsRelationEnemy, "script", &CScriptGameObject::SetScriptControl, "get_script", &CScriptGameObject::GetScriptControl, "get_script_name",
        &CScriptGameObject::GetScriptControlName, "reset_action_queue", &CScriptGameObject::ResetActionQueue, "see",
        sol::overload(&CScriptGameObject::CheckObjectVisibility, &CScriptGameObject::CheckTypeVisibility), "see_right_now", &CScriptGameObject::CheckObjectVisibilityNow,
        "who_hit_name", &CScriptGameObject::WhoHitName, "who_hit_section_name", &CScriptGameObject::WhoHitSectionName,

        "rank", &CScriptGameObject::GetRank, "command", &CScriptGameObject::AddAction, "action", &CScriptGameObject::GetCurrentAction, "object_count",
        &CScriptGameObject::GetInventoryObjectCount, "object",
        sol::overload(sol::resolve<CScriptGameObject*(LPCSTR) const>(&CScriptGameObject::GetObjectByName),
                      sol::resolve<CScriptGameObject*(int) const>(&CScriptGameObject::GetObjectByIndex)),
        "active_item", &CScriptGameObject::GetActiveItem,

        "set_callback",
        sol::overload(sol::resolve<void(GameObject::ECallbackType, sol::function, sol::object)>(&CScriptGameObject::SetCallback),
                      sol::resolve<void(GameObject::ECallbackType, sol::function)>(&CScriptGameObject::SetCallback),
                      sol::resolve<void(GameObject::ECallbackType)>(&CScriptGameObject::SetCallback)),
        "set_patrol_extrapolate_callback",
        sol::overload(sol::resolve<void()>(&CScriptGameObject::set_patrol_extrapolate_callback),
                      sol::resolve<void(const luabind::functor<bool>&)>(&CScriptGameObject::set_patrol_extrapolate_callback),
                      sol::resolve<void(const luabind::functor<bool>&, const luabind::object&)>(&CScriptGameObject::set_patrol_extrapolate_callback)),
        "set_enemy_callback",
        sol::overload(sol::resolve<void(sol::function, sol::object)>(&CScriptGameObject::set_enemy_callback),
                      sol::resolve<void(sol::function)>(&CScriptGameObject::set_enemy_callback), sol::resolve<void()>(&CScriptGameObject::set_enemy_callback)),
        "patrol", &CScriptGameObject::GetPatrolPathName,

        "get_ammo_in_magazine", &CScriptGameObject::GetAmmoElapsed, "get_ammo_total", &CScriptGameObject::GetAmmoCurrent, "set_ammo_elapsed", &CScriptGameObject::SetAmmoElapsed,
        "set_queue_size", &CScriptGameObject::SetQueueSize,

        "best_danger", &CScriptGameObject::GetBestDanger, "best_enemy", &CScriptGameObject::GetBestEnemy, "best_item", &CScriptGameObject::GetBestItem, "action_count",
        &CScriptGameObject::GetActionCount, "action_by_index", &CScriptGameObject::GetActionByIndex,

        /************************* Add by Zander *******************************/
        "dump_visual_to_log", &CScriptGameObject::GetModelDump, "set_show_model_mesh", &CScriptGameObject::ShowModelMesh, "get_show_model_mesh", &CScriptGameObject::GetShowMesh,
        "get_mesh_count", &CScriptGameObject::GetMeshCount, "set_show_model_mesh_hud", &CScriptGameObject::ShowModelMeshHUD, "get_show_model_mesh_hud",
        &CScriptGameObject::GetShowMeshHUD, "get_mesh_count_hud", &CScriptGameObject::GetMeshCountHUD,
        /************************* End Add *************************************/

        "memory_time", &CScriptGameObject::memory_time, "memory_position", &CScriptGameObject::memory_position, "best_weapon", &CScriptGameObject::best_weapon, "explode",
        &CScriptGameObject::explode, "explode_initiator", &CScriptGameObject::explode_initiator, "is_exploded", &CScriptGameObject::is_exploded, "is_ready_to_explode",
        &CScriptGameObject::is_ready_to_explode, "get_enemy", &CScriptGameObject::GetEnemy, "get_corpse", &CScriptGameObject::GetCorpse, "get_enemy_strength",
        &CScriptGameObject::GetEnemyStrength, "get_sound_info", &CScriptGameObject::GetSoundInfo, "get_monster_hit_info", &CScriptGameObject::GetMonsterHitInfo, "bind_object",
        &CScriptGameObject::bind_object, "motivation_action_manager", &script_action_planner,

        // bloodsucker
        "set_invisible", &CScriptGameObject::set_invisible, "set_manual_invisibility", &CScriptGameObject::set_manual_invisibility, "set_alien_control",
        &CScriptGameObject::set_alien_control,

        // zombie
        "fake_death_fall_down", &CScriptGameObject::fake_death_fall_down, "fake_death_stand_up", &CScriptGameObject::fake_death_stand_up,

        // base monster
        "skip_transfer_enemy", &CScriptGameObject::skip_transfer_enemy,

        "set_home",
        sol::overload(
            sol::resolve<void(LPCSTR, float, float, bool, float)>(&CScriptGameObject::set_home), sol::resolve<void(LPCSTR, float, float, bool)>(&CScriptGameObject::set_home),
            sol::resolve<void(LPCSTR, float, float)>(&CScriptGameObject::set_home), sol::resolve<void(CPatrolPath*, float, float, bool, float)>(&CScriptGameObject::set_home),
            sol::resolve<void(CPatrolPath*, float, float, bool)>(&CScriptGameObject::set_home), sol::resolve<void(CPatrolPath*, float, float)>(&CScriptGameObject::set_home),
            sol::resolve<void(u32, float, float, bool, float)>(&CScriptGameObject::set_home), sol::resolve<void(u32, float, float, bool)>(&CScriptGameObject::set_home),
            sol::resolve<void(u32, float, float)>(&CScriptGameObject::set_home)),
        "at_home", sol::overload(sol::resolve<bool()>(&CScriptGameObject::at_home), sol::resolve<bool(Fvector)>(&CScriptGameObject::at_home)), "remove_home",
        &CScriptGameObject::remove_home,

        "berserk", &CScriptGameObject::berserk, "can_script_capture", &CScriptGameObject::can_script_capture, "set_custom_panic_threshold",
        &CScriptGameObject::set_custom_panic_threshold, "set_default_panic_threshold", &CScriptGameObject::set_default_panic_threshold,

        // inventory owner
        "get_current_outfit", &CScriptGameObject::GetCurrentOutfit, "get_current_outfit_protection", &CScriptGameObject::GetCurrentOutfitProtection,

        // searchlight
        "get_current_direction", &CScriptGameObject::GetCurrentDirection, "get_projector", &CScriptGameObject::GetProjector, "projector_switch",
        &CScriptGameObject::SwitchProjector,

        // movement manager
        "set_body_state", &CScriptGameObject::set_body_state, "set_movement_type", &CScriptGameObject::set_movement_type, "set_mental_state", &CScriptGameObject::set_mental_state,
        "set_path_type", &CScriptGameObject::set_path_type, "set_detail_path_type", &CScriptGameObject::set_detail_path_type,

        "body_state", &CScriptGameObject::body_state, "target_body_state", &CScriptGameObject::target_body_state, "movement_type", &CScriptGameObject::movement_type,
        "target_movement_type", &CScriptGameObject::target_movement_type, "mental_state", &CScriptGameObject::mental_state, "target_mental_state",
        &CScriptGameObject::target_mental_state, "path_type", &CScriptGameObject::path_type, "detail_path_type", &CScriptGameObject::detail_path_type,

        //
        "set_desired_position",
        sol::overload(sol::resolve<void()>(&CScriptGameObject::set_desired_position), sol::resolve<void(const Fvector*)>(&CScriptGameObject::set_desired_position)),
        "set_desired_direction",
        sol::overload(sol::resolve<void()>(&CScriptGameObject::set_desired_direction), sol::resolve<void(const Fvector*)>(&CScriptGameObject::set_desired_direction)),
        "set_patrol_path", &CScriptGameObject::set_patrol_path,

        "set_dest_level_vertex_id", &CScriptGameObject::set_dest_level_vertex_id, "level_vertex_id", &CScriptGameObject::level_vertex_id, "level_vertex_light",
        &CScriptGameObject::level_vertex_light, "game_vertex_id", &CScriptGameObject::game_vertex_id,

        "add_animation", sol::resolve<void(LPCSTR, bool, bool)>(&CScriptGameObject::add_animation), "clear_animations", &CScriptGameObject::clear_animations, "animation_count",
        &CScriptGameObject::animation_count, "animation_slot", &CScriptGameObject::animation_slot,

        "ignore_monster_threshold", sol::overload(&CScriptGameObject::ignore_monster_threshold, &CScriptGameObject::set_ignore_monster_threshold),
        "restore_ignore_monster_threshold", &CScriptGameObject::restore_ignore_monster_threshold, "max_ignore_monster_distance",
        sol::overload(&CScriptGameObject::max_ignore_monster_distance, &CScriptGameObject::set_max_ignore_monster_distance), "restore_max_ignore_monster_distance",
        &CScriptGameObject::restore_max_ignore_monster_distance,

        "eat", &CScriptGameObject::eat, "extrapolate_length",
        sol::overload(sol::resolve<float() const>(&CScriptGameObject::extrapolate_length), sol::resolve<void(float)>(&CScriptGameObject::extrapolate_length)),

        "set_fov", &CScriptGameObject::set_fov, "set_range", &CScriptGameObject::set_range, "head_orientation", &CScriptGameObject::head_orientation, "set_actor_position",
        &CScriptGameObject::SetActorPosition, "set_actor_direction", &CScriptGameObject::SetActorDirection, "set_npc_position", &CScriptGameObject::SetNpcPosition,
        "vertex_in_direction", &CScriptGameObject::vertex_in_direction,

        "item_in_slot", &CScriptGameObject::item_in_slot, "active_slot", &CScriptGameObject::active_slot, "activate_slot", &CScriptGameObject::activate_slot, "switch_torch",
        &CScriptGameObject::SwitchTorch,

        "get_xform", &CScriptGameObject::GetXForm,

#ifdef DEBUG
        "debug_planner", &CScriptGameObject::debug_planner,
#endif // DEBUG

        "invulnerable", sol::overload(sol::resolve<bool() const>(&CScriptGameObject::invulnerable), sol::resolve<void(bool)>(&CScriptGameObject::invulnerable)));
}
