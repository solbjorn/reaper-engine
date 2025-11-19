#pragma once

#include "../state.h"
#include "../states/monster_state_attack.h"

template <typename _Object>
class CStateBurerAttackMelee : public CStateMonsterAttack<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAttackMelee<_Object>, CStateMonsterAttack<_Object>);

private:
    typedef CStateMonsterAttack<_Object> inherited;
    using inherited::object;

public:
    explicit CStateBurerAttackMelee(_Object* obj);
    ~CStateBurerAttackMelee() override = default;

    virtual bool check_start_conditions();
    virtual bool check_completion();
};

#include "burer_state_attack_melee_inline.h"
