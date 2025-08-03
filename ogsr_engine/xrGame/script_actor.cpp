////////////////////////////////////////////////////////////////////////////
//	Module 		: script_actor.cpp
//	Created 	: 12.08.2014
//  Modified 	: 23.08.2014
//	Author		: Alexander Petrov
//	Description : Script Actor (params)
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "base_client_classes.h"
#include "script_game_object.h"
#include "CharacterPhysicsSupport.h"
#include "script_actor.h"
#include "PHSimpleCharacter.h"
#include "Inventory.h"
#include "Wound.h"

static CPHMovementControl* get_movement(CActor* pActor) { return pActor->character_physics_support()->movement(); }

typedef CScriptActor::SConditionChangeV SConditionChangeV;
typedef float SConditionChangeV::* SConditionChangeVField;

template <SConditionChangeVField field>
static float get_change_v(CActorCondition* C)
{
    return CScriptActor::sccv(C).*field;
}

template <SConditionChangeVField field>
static void set_change_v(CActorCondition* C, float v)
{
    CScriptActor::sccv(C).*field = v;
}

static void set_health(CActorCondition* C, float h) { C->health() = h; }
static void set_max_health(CActorCondition* C, float h) { C->max_health() = h; }

static float get_wound_size(CActorCondition* C, u32 bone, u32 hit_type)
{
    if (C->wounds().size() <= bone)
        return 0;
    return C->wounds().at(bone)->TypeSize((ALife::EHitType)hit_type);
}

static float get_wound_total_size(CActorCondition* C, u32 bone)
{
    if (C->wounds().size() <= bone)
        return 0;
    return C->wounds().at(bone)->TotalSize();
}

static float get_jump_up_velocity(CPHMovementControl* M)
{
    CPHSimpleCharacter* sp = smart_cast<CPHSimpleCharacter*>(M->PHCharacter());
    if (sp)
        return sp->get_jump_up_velocity();
    return 0;
}

static float get_jump_speed(CActor* pActor) { return CScriptActor::jump_speed(pActor); }

static void set_jump_speed(CActor* pActor, float speed)
{
    CScriptActor::jump_speed(pActor) = speed;
    get_movement(pActor)->SetJumpUpVelocity(speed);
}

static CHitImmunity* get_immunities(CActor* pActor) { return pActor->conditions().cast_hit_immunities(); }

static CEntity::SEntityState* get_actor_state(CActor* pActor)
{
    static CEntity::SEntityState state;
    pActor->g_State(state);
    return &state;
}

static CActorConditionObject* get_actor_condition(CActor* pActor) { return (CActorConditionObject*)(&pActor->conditions()); }
static SRotation& get_actor_orientation(CActor* pActor) { return pActor->Orientation(); }
static EActorCameras get_active_cam(CActor* pActor) { return pActor->active_cam(); }

static bool IsLimping(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eLimping); }
static bool IsCantWalk(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eCantWalk); }
static bool IsCantSprint(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eCantSprint); }

void CScriptActor::script_register(sol::state_view& lua)
{
    lua.new_usertype<CActorCondition>(
        "CActorConditionBase", sol::no_constructor, "health", sol::property(&CActorCondition::GetHealth, &set_health), "health_max",
        sol::property(&CActorCondition::GetMaxHealth, &set_max_health), "alcohol_health", &CActorCondition::m_fAlcohol, "alcohol_v", &CActorCondition::m_fV_Alcohol, "power_v",
        &CActorCondition::m_fV_Power, "satiety", &CActorCondition::m_fSatiety, "satiety_v", &CActorCondition::m_fV_Satiety, "satiety_health_v",
        &CActorCondition::m_fV_SatietyHealth, "satiety_power_v", &CActorCondition::m_fV_SatietyPower,

        "thirst", &CActorCondition::m_fThirst, "thirst_v", &CActorCondition::m_fV_Thirst, "thirst_health_v", &CActorCondition::m_fV_ThirstHealth, "thirst_power_v",
        &CActorCondition::m_fV_ThirstPower,

        "max_power_leak_speed", &CActorCondition::m_fPowerLeakSpeed, "jump_power", &CActorCondition::m_fJumpPower, "stand_power", &CActorCondition::m_fStandPower, "walk_power",
        &CActorCondition::m_fWalkPower, "jump_weight_power", &CActorCondition::m_fJumpWeightPower, "walk_weight_power", &CActorCondition::m_fWalkWeightPower, "overweight_walk_k",
        &CActorCondition::m_fOverweightWalkK, "overweight_jump_k", &CActorCondition::m_fOverweightJumpK, "accel_k", &CActorCondition::m_fAccelK, "sprint_k",
        &CActorCondition::m_fSprintK, "max_walk_weight", &CActorCondition::m_MaxWalkWeight,

        "power_hit_part", &CActorCondition::m_fPowerHitPart,

        "limping_power_begin", &CActorCondition::m_fLimpingPowerBegin, "limping_power_end", &CActorCondition::m_fLimpingPowerEnd, "cant_walk_power_begin",
        &CActorCondition::m_fCantWalkPowerBegin, "cant_walk_power_end", &CActorCondition::m_fCantWalkPowerEnd, "cant_spint_power_begin", &CActorCondition::m_fCantSprintPowerBegin,
        "cant_spint_power_end", &CActorCondition::m_fCantSprintPowerEnd, "limping_health_begin", &CActorCondition::m_fLimpingHealthBegin, "limping_health_end",
        &CActorCondition::m_fLimpingHealthEnd, "limping", sol::property(&IsLimping), "cant_walk", sol::property(&IsCantWalk), "cant_sprint", sol::property(&IsCantSprint),
        "radiation_v", sol::property(&get_change_v<&SConditionChangeV::m_fV_Radiation>, &set_change_v<&SConditionChangeV::m_fV_Radiation>), "psy_health_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_PsyHealth>, &set_change_v<&SConditionChangeV::m_fV_PsyHealth>), "morale_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_EntityMorale>, &set_change_v<&SConditionChangeV::m_fV_EntityMorale>), "radiation_health_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_RadiationHealth>, &set_change_v<&SConditionChangeV::m_fV_RadiationHealth>), "bleeding_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_Bleeding>, &set_change_v<&SConditionChangeV::m_fV_Bleeding>), "wound_incarnation_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_WoundIncarnation>, &set_change_v<&SConditionChangeV::m_fV_WoundIncarnation>), "health_restore_v",
        sol::property(&get_change_v<&SConditionChangeV::m_fV_HealthRestore>, &set_change_v<&SConditionChangeV::m_fV_HealthRestore>), "get_wound_size", &get_wound_size,
        "get_wound_total_size", &get_wound_total_size, sol::base_classes, xr_sol_bases<CActorCondition>());

    lua.new_usertype<CActorConditionObject>("CActorCondition", sol::no_constructor, sol::base_classes, xr_sol_bases<CActorConditionObject>());

    lua.new_usertype<CPHMovementControl>("CPHMovementControl", sol::no_constructor, "ph_mass", &CPHMovementControl::fMass, "crash_speed_max", &CPHMovementControl::fMaxCrashSpeed,
                                         "crash_speed_min", &CPHMovementControl::fMinCrashSpeed, "collision_damage_factor", &CPHMovementControl::fCollisionDamageFactor,
                                         "air_control_param", &CPHMovementControl::fAirControlParam, "jump_up_velocity",
                                         sol::property(&get_jump_up_velocity, &CPHMovementControl::SetJumpUpVelocity));

    lua.new_usertype<CActor>("CActorBase", sol::no_constructor, "condition", sol::property(&get_actor_condition), "immunities", sol::property(&get_immunities), "hit_slowmo",
                             &CActor::hit_slowmo, "hit_probability", &CActor::hit_probability, "walk_accel", &CActor::m_fWalkAccel,

                             "run_coef", &CActor::m_fRunFactor, "run_back_coef", &CActor::m_fRunBackFactor, "walk_back_coef", &CActor::m_fWalkBackFactor, "crouch_coef",
                             &CActor::m_fCrouchFactor, "climb_coef", &CActor::m_fClimbFactor, "sprint_koef", &CActor::m_fSprintFactor, "walk_strafe_coef",
                             &CActor::m_fWalk_StrafeFactor, "run_strafe_coef", &CActor::m_fRun_StrafeFactor, "disp_base", &CActor::m_fDispBase, "disp_aim", &CActor::m_fDispAim,
                             "disp_vel_factor", &CActor::m_fDispVelFactor, "disp_accel_factor", &CActor::m_fDispAccelFactor, "disp_crouch_factor", &CActor::m_fDispCrouchFactor,
                             "disp_crouch_no_acc_factor", &CActor::m_fDispCrouchNoAccelFactor,

                             "movement", sol::property(&get_movement), "jump_speed", sol::property(&get_jump_speed, &set_jump_speed), "state", sol::property(&get_actor_state),
                             "orientation", sol::property(&get_actor_orientation),

                             // Real Wolf. Start. 14.10.2014.
                             "block_action", &CActor::block_action, "unblock_action", &CActor::unblock_action, "press_action", &CActor::IR_OnKeyboardPress, "hold_action",
                             &CActor::IR_OnKeyboardHold, "release_action", &CActor::IR_OnKeyboardRelease, "is_zoom_aiming_mode", &CActor::IsZoomAimingMode,
                             // Real Wolf. End. 14.10.2014.

                             "get_body_state", &CActor::get_state, "is_actor_normal", &CActor::is_actor_normal, "is_actor_crouch", &CActor::is_actor_crouch, "is_actor_creep",
                             &CActor::is_actor_creep, "is_actor_climb", &CActor::is_actor_climb, "is_actor_walking", &CActor::is_actor_walking, "is_actor_running",
                             &CActor::is_actor_running, "is_actor_sprinting", &CActor::is_actor_sprinting, "is_actor_crouching", &CActor::is_actor_crouching, "is_actor_creeping",
                             &CActor::is_actor_creeping, "is_actor_climbing", &CActor::is_actor_climbing, "is_actor_moving", &CActor::is_actor_moving, "UpdateArtefactsOnBelt",
                             &CActor::UpdateArtefactsOnBelt, "IsDetectorActive", &CActor::IsDetectorActive,

                             "active_cam", sol::property(&get_active_cam), "set_active_cam", &CActor::cam_Set, sol::base_classes, xr_sol_bases<CActor>());

    lua.new_usertype<CActorObject>("CActor", sol::no_constructor, sol::base_classes, xr_sol_bases<CActorObject>());

    lua.new_enum("EActorCameras", "eacFirstEye", eacFirstEye, "eacLookAt", eacLookAt, "eacFreeLook", eacFreeLook, "eacMaxCam", eacMaxCam);

    lua.new_enum("inventory_slots", "KNIFE", KNIFE_SLOT, "FIRST_WEAPON", FIRST_WEAPON_SLOT, "SECOND_WEAPON", SECOND_WEAPON_SLOT, "GRENADE", GRENADE_SLOT, "APPARATUS",
                 APPARATUS_SLOT, "BOLT", BOLT_SLOT, "OUTFIT", OUTFIT_SLOT, "PDA", PDA_SLOT, "DETECTOR", DETECTOR_SLOT, "TORCH", TORCH_SLOT, "HELMET", HELMET_SLOT, "NIGHT_VISION",
                 NIGHT_VISION_SLOT, "BIODETECTOR", BIODETECTOR_SLOT, "TOTAL", SLOTS_TOTAL, "NO_ACTIVE", NO_ACTIVE_SLOT);

    lua.new_enum("item_place", "undefined", eItemPlaceUndefined, "slot", eItemPlaceSlot, "belt", eItemPlaceBelt, "ruck", eItemPlaceRuck);
}
