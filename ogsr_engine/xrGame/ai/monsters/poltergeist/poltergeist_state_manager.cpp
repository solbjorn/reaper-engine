#include "stdafx.h"

#include "poltergeist.h"
#include "poltergeist_state_manager.h"

#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"

#include "poltergeist_state_rest.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "poltergeist_state_attack_hidden.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../../../entitycondition.h"

CStateManagerPoltergeist::CStateManagerPoltergeist(CPoltergeist* obj) : inherited(obj)
{
    add_state(eStateRest, xr_new<CPoltergeistStateRest<CPoltergeist>>(obj));
    add_state(eStateEat, xr_new<CStateMonsterEat<CPoltergeist>>(obj));
    add_state(eStateAttack_AttackHidden, xr_new<CStatePoltergeistAttackHidden<CPoltergeist>>(obj));
    add_state(eStatePanic, xr_new<CStateMonsterPanic<CPoltergeist>>(obj));
    add_state(eStateHitted, xr_new<CStateMonsterHitted<CPoltergeist>>(obj));
    add_state(eStateHearInterestingSound, xr_new<CStateMonsterHearInterestingSound<CPoltergeist>>(obj));
    add_state(eStateHearDangerousSound, xr_new<CStateMonsterHearDangerousSound<CPoltergeist>>(obj));
}

CStateManagerPoltergeist::~CStateManagerPoltergeist() {}

void CStateManagerPoltergeist::reinit()
{
    inherited::reinit();

    time_next_flame_attack = 0;
    time_next_tele_attack = 0;
    time_next_scare_attack = 0;
}

void CStateManagerPoltergeist::execute()
{
    u32 state_id = u32(-1);

    if (object->EnemyMan.get_enemy() && object->detected_enemy())
        state_id = eStateAttack_AttackHidden;
    else
        state_id = eStateRest;

    select_state(state_id);

    // выполнить текущее состояние
    get_state_current()->execute();

    prev_substate = current_substate;
}

void CStateManagerPoltergeist::polter_attack() {}
