#include "stdafx.h"
#include "helicopter.h"
#include "script_game_object.h"

int CHelicopter::GetMovementState() { return m_movement.type; }
int CHelicopter::GetHuntState() { return m_enemy.type; }
int CHelicopter::GetBodyState() { return m_body.type; }

void CHelicopter::script_register(sol::state_view& lua)
{
    lua.new_usertype<CHelicopter>(
        "CHelicopter", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CHelicopter>), "factory", &client_factory<CHelicopter>,

        // state
        "eAlive", sol::var(CHelicopter::eAlive), "eDead", sol::var(CHelicopter::eDead),
        // movement_state
        "eMovNone", sol::var(eMovNone), "eMovToPoint", sol::var(eMovToPoint), "eMovPatrolPath", sol::var(eMovPatrolPath), "eMovRoundPath", sol::var(eMovRoundPath), "eMovLanding",
        sol::var(eMovLanding), "eMovTakeOff", sol::var(eMovTakeOff),
        // hunt_state
        "eEnemyNone", sol::var(eEnemyNone), "eEnemyPoint", sol::var(eEnemyPoint), "eEnemyEntity", sol::var(eEnemyEntity),
        // body_state
        "eBodyByPath", sol::var(eBodyByPath), "eBodyToPoint", sol::var(eBodyToPoint),

        "GetState", &CHelicopter::state_script, "GetMovementState", &CHelicopter::GetMovementState, "GetHuntState", &CHelicopter::GetHuntState, "GetBodyState",
        &CHelicopter::GetBodyState,

        "GetCurrVelocity", &CHelicopter::GetCurrVelocity, "GetMaxVelocity", &CHelicopter::GetMaxVelocity, "SetMaxVelocity", &CHelicopter::SetMaxVelocity, "GetCurrVelocityVec",
        &CHelicopter::GetCurrVelocityVec, "GetfHealth", &CHelicopter::GetHeliHealth, "SetfHealth", &CHelicopter::SetHeliHealth, "SetSpeedInDestPoint",
        &CHelicopter::SetSpeedInDestPoint, "GetSpeedInDestPoint", &CHelicopter::GetSpeedInDestPoint,

        //////////////////////Start By JoHnY///////////////////////
        "SetLinearAcc", &CHelicopter::SetLinearAcc,
        //////////////////////End By JoHnY/////////////////////////

        "SetOnPointRangeDist", &CHelicopter::SetOnPointRangeDist, "GetOnPointRangeDist", &CHelicopter::GetOnPointRangeDist, "GetDistanceToDestPosition",
        &CHelicopter::GetDistanceToDestPosition,

        "ClearEnemy", &CHelicopter::UnSetEnemy, "SetEnemy",
        sol::overload(sol::resolve<void(CScriptGameObject*)>(&CHelicopter::SetEnemy), sol::resolve<void(Fvector*)>(&CHelicopter::SetEnemy)), "GoPatrolByPatrolPath",
        &CHelicopter::goPatrolByPatrolPath, "GoPatrolByRoundPath", &CHelicopter::goByRoundPath, "SetDestPosition", &CHelicopter::SetDestPosition, "LookAtPoint",
        &CHelicopter::LookAtPoint, "SetFireTrailLength", &CHelicopter::SetFireTrailLength, "SetBarrelDirTolerance", &CHelicopter::SetBarrelDirTolerance,

        "UseFireTrail", sol::overload(sol::resolve<bool()>(&CHelicopter::UseFireTrail), sol::resolve<void(bool)>(&CHelicopter::UseFireTrail)), "Die", &CHelicopter::DieHelicopter,
        "StartFlame", &CHelicopter::StartFlame, "Explode", &CHelicopter::ExplodeHelicopter,

        "isVisible", &CHelicopter::isVisible, "GetRealAltitude", &CHelicopter::GetRealAltitude, "GetSafeAltitude", &CHelicopter::GetSafeAltitude, "TurnLighting",
        &CHelicopter::TurnLighting, "TurnEngineSound", &CHelicopter::TurnEngineSound,

        "m_use_rocket_on_attack", &CHelicopter::m_use_rocket_on_attack, "m_use_mgun_on_attack", &CHelicopter::m_use_mgun_on_attack, "m_min_rocket_dist",
        &CHelicopter::m_min_rocket_dist, "m_max_rocket_dist", &CHelicopter::m_max_rocket_dist, "m_min_mgun_dist", &CHelicopter::m_min_mgun_dist, "m_max_mgun_dist",
        &CHelicopter::m_max_mgun_dist, "m_time_between_rocket_attack", &CHelicopter::m_time_between_rocket_attack, "m_syncronize_rocket", &CHelicopter::m_syncronize_rocket,
        "m_flame_started", sol::readonly(&CHelicopter::m_flame_started), "m_light_started", sol::readonly(&CHelicopter::m_light_started), "m_exploded",
        sol::readonly(&CHelicopter::m_exploded), "m_dead", sol::readonly(&CHelicopter::m_dead), sol::base_classes, xr_sol_bases<CHelicopter>());
}
