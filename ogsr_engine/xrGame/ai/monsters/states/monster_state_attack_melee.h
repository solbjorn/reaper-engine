#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackMelee : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackMelee<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterAttackMelee(_Object* obj);
    ~CStateMonsterAttackMelee() override;

    virtual void execute();

    virtual bool check_completion();
    virtual bool check_start_conditions();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_attack_melee_inline.h"
