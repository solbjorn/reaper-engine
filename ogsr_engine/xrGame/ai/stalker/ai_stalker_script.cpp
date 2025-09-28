////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_script.cpp
//	Created 	: 29.09.2003
//  Modified 	: 29.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker script functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_stalker.h"
#include "../../stalker_decision_space.h"
#include "ai_stalker_space.h"
#include "../../script_game_object.h"
#include "../../stalker_planner.h"

void CAI_Stalker::script_register(sol::state_view& lua)
{
    lua.new_enum(
        "stalker_ids",
        // properties
        "property_alive", StalkerDecisionSpace::eWorldPropertyAlive, "property_dead", StalkerDecisionSpace::eWorldPropertyDead, "property_already_dead",
        StalkerDecisionSpace::eWorldPropertyAlreadyDead, "property_alife", StalkerDecisionSpace::eWorldPropertyALife, "property_puzzle_solved",
        StalkerDecisionSpace::eWorldPropertyPuzzleSolved, "property_smart_terrain_task", StalkerDecisionSpace::eWorldPropertySmartTerrainTask, "property_items",
        StalkerDecisionSpace::eWorldPropertyItems, "property_enemy", StalkerDecisionSpace::eWorldPropertyEnemy, "property_danger", StalkerDecisionSpace::eWorldPropertyDanger,
        "property_item_to_kill", StalkerDecisionSpace::eWorldPropertyItemToKill, "property_found_item_to_kill", StalkerDecisionSpace::eWorldPropertyFoundItemToKill,
        "property_item_can_kill", StalkerDecisionSpace::eWorldPropertyItemCanKill, "property_found_ammo", StalkerDecisionSpace::eWorldPropertyFoundAmmo, "property_ready_to_kill",
        StalkerDecisionSpace::eWorldPropertyReadyToKill, "property_ready_to_detour", StalkerDecisionSpace::eWorldPropertyReadyToDetour, "property_see_enemy",
        StalkerDecisionSpace::eWorldPropertySeeEnemy, "property_panic", StalkerDecisionSpace::eWorldPropertyPanic, "property_in_cover", StalkerDecisionSpace::eWorldPropertyInCover,
        "property_looked_out", StalkerDecisionSpace::eWorldPropertyLookedOut, "property_position_holded", StalkerDecisionSpace::eWorldPropertyPositionHolded,
        "property_enemy_detoured", StalkerDecisionSpace::eWorldPropertyEnemyDetoured, "property_use_suddenness", StalkerDecisionSpace::eWorldPropertyUseSuddenness,
        "property_use_crouch_to_look_out", StalkerDecisionSpace::eWorldPropertyUseCrouchToLookOut, "property_critically_wounded",
        StalkerDecisionSpace::eWorldPropertyCriticallyWounded, "property_enemy_critically_wounded", StalkerDecisionSpace::eWorldPropertyEnemyCriticallyWounded,

        "property_danger_unknown", StalkerDecisionSpace::eWorldPropertyDangerUnknown, "property_danger_in_direction", StalkerDecisionSpace::eWorldPropertyDangerInDirection,
        "property_danger_grenade", StalkerDecisionSpace::eWorldPropertyDangerGrenade, "property_danger_by_sound", StalkerDecisionSpace::eWorldPropertyDangerBySound,

        "property_cover_actual", StalkerDecisionSpace::eWorldPropertyCoverActual, "property_cover_reached", StalkerDecisionSpace::eWorldPropertyCoverReached,
        "property_looked_around", StalkerDecisionSpace::eWorldPropertyLookedAround, "property_grenade_exploded", StalkerDecisionSpace::eWorldPropertyGrenadeExploded,

        "property_anomaly", StalkerDecisionSpace::eWorldPropertyAnomaly, "property_inside_anomaly", StalkerDecisionSpace::eWorldPropertyInsideAnomaly, "property_pure_enemy",
        StalkerDecisionSpace::eWorldPropertyPureEnemy, "property_script", StalkerDecisionSpace::eWorldPropertyScript, "property_enemy_wounded",
        StalkerDecisionSpace::eWorldPropertyEnemyWounded,

        // action
        "action_dead", StalkerDecisionSpace::eWorldOperatorDead, "action_dying", StalkerDecisionSpace::eWorldOperatorDying, "action_gather_items",
        StalkerDecisionSpace::eWorldOperatorGatherItems, "action_no_alife", StalkerDecisionSpace::eWorldOperatorALifeEmulation, "action_smart_terrain_task",
        StalkerDecisionSpace::eWorldOperatorSmartTerrainTask, "action_solve_zone_puzzle", StalkerDecisionSpace::eWorldOperatorSolveZonePuzzle, "action_reach_task_location",
        StalkerDecisionSpace::eWorldOperatorReachTaskLocation, "action_accomplish_task", StalkerDecisionSpace::eWorldOperatorAccomplishTask, "action_reach_customer_location",
        StalkerDecisionSpace::eWorldOperatorReachCustomerLocation, "action_communicate_with_customer", StalkerDecisionSpace::eWorldOperatorCommunicateWithCustomer,
        "get_out_of_anomaly", StalkerDecisionSpace::eWorldOperatorGetOutOfAnomaly, "detect_anomaly", StalkerDecisionSpace::eWorldOperatorDetectAnomaly, "action_get_item_to_kill",
        StalkerDecisionSpace::eWorldOperatorGetItemToKill, "action_find_item_to_kill", StalkerDecisionSpace::eWorldOperatorFindItemToKill, "action_make_item_killing",
        StalkerDecisionSpace::eWorldOperatorMakeItemKilling, "action_find_ammo", StalkerDecisionSpace::eWorldOperatorFindAmmo, "action_aim_enemy",
        StalkerDecisionSpace::eWorldOperatorAimEnemy, "action_get_ready_to_kill", StalkerDecisionSpace::eWorldOperatorGetReadyToKill, "action_kill_enemy",
        StalkerDecisionSpace::eWorldOperatorKillEnemy, "action_retreat_from_enemy", StalkerDecisionSpace::eWorldOperatorRetreatFromEnemy, "action_take_cover",
        StalkerDecisionSpace::eWorldOperatorTakeCover, "action_look_out", StalkerDecisionSpace::eWorldOperatorLookOut, "action_hold_position",
        StalkerDecisionSpace::eWorldOperatorHoldPosition, "action_get_distance", StalkerDecisionSpace::eWorldOperatorGetDistance, "action_detour_enemy",
        StalkerDecisionSpace::eWorldOperatorDetourEnemy, "action_search_enemy", StalkerDecisionSpace::eWorldOperatorSearchEnemy, "action_sudden_attack",
        StalkerDecisionSpace::eWorldOperatorSuddenAttack, "action_kill_enemy_if_not_visible", StalkerDecisionSpace::eWorldOperatorKillEnemyIfNotVisible,
        "action_reach_wounded_enemy", StalkerDecisionSpace::eWorldOperatorReachWoundedEnemy, "action_prepare_wounded_enemy",
        StalkerDecisionSpace::eWorldOperatorPrepareWoundedEnemy, "action_kill_wounded_enemy", StalkerDecisionSpace::eWorldOperatorKillWoundedEnemy,
        "action_kill_if_player_on_the_path", StalkerDecisionSpace::eWorldOperatorKillEnemyIfPlayerOnThePath, "action_critically_wounded",
        StalkerDecisionSpace::eWorldOperatorCriticallyWounded, "action_kill_if_enemy_critically_wounded", StalkerDecisionSpace::eWorldOperatorKillEnemyIfCriticallyWounded,

        "action_danger_unknown_planner", StalkerDecisionSpace::eWorldOperatorDangerUnknownPlanner, "action_danger_in_direction_planner",
        StalkerDecisionSpace::eWorldOperatorDangerInDirectionPlanner, "action_danger_grenade_planner", StalkerDecisionSpace::eWorldOperatorDangerGrenadePlanner,
        "action_danger_by_sound_planner", StalkerDecisionSpace::eWorldOperatorDangerBySoundPlanner,

        "action_danger_unknown_take_cover", StalkerDecisionSpace::eWorldOperatorDangerUnknownTakeCover, "action_danger_unknown_look_around",
        StalkerDecisionSpace::eWorldOperatorDangerUnknownLookAround, "action_danger_unknown_search", StalkerDecisionSpace::eWorldOperatorDangerUnknownSearchEnemy,

        "action_danger_in_direction_take_cover", StalkerDecisionSpace::eWorldOperatorDangerInDirectionTakeCover, "action_danger_in_direction_look_out",
        StalkerDecisionSpace::eWorldOperatorDangerInDirectionLookOut, "action_danger_in_direction_hold_position", StalkerDecisionSpace::eWorldOperatorDangerInDirectionHoldPosition,
        "action_danger_in_direction_detour", StalkerDecisionSpace::eWorldOperatorDangerInDirectionDetourEnemy, "action_danger_in_direction_search",
        StalkerDecisionSpace::eWorldOperatorDangerInDirectionSearchEnemy,

        "action_danger_grenade_take_cover", StalkerDecisionSpace::eWorldOperatorDangerGrenadeTakeCover, "action_danger_grenade_wait_for_explosion",
        StalkerDecisionSpace::eWorldOperatorDangerGrenadeWaitForExplosion, "action_danger_grenade_take_cover_after_explosion",
        StalkerDecisionSpace::eWorldOperatorDangerGrenadeTakeCoverAfterExplosion, "action_danger_grenade_look_around", StalkerDecisionSpace::eWorldOperatorDangerGrenadeLookAround,
        "action_danger_grenade_search", StalkerDecisionSpace::eWorldOperatorDangerGrenadeSearch,

        "action_death_planner", StalkerDecisionSpace::eWorldOperatorDeathPlanner, "action_alife_planner", StalkerDecisionSpace::eWorldOperatorALifePlanner, "action_combat_planner",
        StalkerDecisionSpace::eWorldOperatorCombatPlanner, "action_anomaly_planner", StalkerDecisionSpace::eWorldOperatorAnomalyPlanner, "action_danger_planner",
        StalkerDecisionSpace::eWorldOperatorDangerPlanner, "action_post_combat_wait", StalkerDecisionSpace::eWorldOperatorPostCombatWait, "action_script",
        StalkerDecisionSpace::eWorldOperatorScript, "action_hide_from_grenade", StalkerDecisionSpace::eWorldOperatorHideFromGrenade,

        // sounds
        "sound_die", StalkerSpace::eStalkerSoundDie, "sound_die_in_anomaly", StalkerSpace::eStalkerSoundDieInAnomaly, "sound_injuring", StalkerSpace::eStalkerSoundInjuring,
        "sound_humming", StalkerSpace::eStalkerSoundHumming, "sound_alarm", StalkerSpace::eStalkerSoundAlarm, "sound_attack_no_allies", StalkerSpace::eStalkerSoundAttackNoAllies,
        "sound_attack_allies_single_enemy", StalkerSpace::eStalkerSoundAttackAlliesSingleEnemy, "sound_attack_allies_several_enemies",
        StalkerSpace::eStalkerSoundAttackAlliesSeveralEnemies, "sound_backup", StalkerSpace::eStalkerSoundBackup, "sound_detour", StalkerSpace::eStalkerSoundDetour,
        "sound_search1_no_allies", StalkerSpace::eStalkerSoundSearch1NoAllies, "sound_search1_with_allies", StalkerSpace::eStalkerSoundSearch1WithAllies,
        "sound_injuring_by_friend", StalkerSpace::eStalkerSoundInjuringByFriend, "sound_panic_human", StalkerSpace::eStalkerSoundPanicHuman, "sound_panic_monster",
        StalkerSpace::eStalkerSoundPanicMonster, "sound_tolls", StalkerSpace::eStalkerSoundTolls, "sound_grenade_alarm", StalkerSpace::eStalkerSoundGrenadeAlarm,
        "sound_friendly_grenade_alarm", StalkerSpace::eStalkerSoundFriendlyGrenadeAlarm, "sound_need_backup", StalkerSpace::eStalkerSoundNeedBackup,

        "sound_running_in_danger", StalkerSpace::eStalkerSoundRunningInDanger, "sound_kill_wounded", StalkerSpace::eStalkerSoundKillWounded, "sound_enemy_critically_wounded",
        StalkerSpace::eStalkerSoundEnemyCriticallyWounded, "sound_enemy_killed_or_wounded", StalkerSpace::eStalkerSoundEnemyKilledOrWounded,

        "sound_script", StalkerSpace::eStalkerSoundScript);

    lua.new_usertype<CAI_Stalker>("CAI_Stalker", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Stalker>), "factory",
                                  &xr::client_factory<CAI_Stalker>, sol::base_classes, xr::sol_bases<CAI_Stalker>());
}
