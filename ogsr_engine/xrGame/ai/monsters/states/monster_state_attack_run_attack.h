#pragma once
#include "../state.h"

template <typename _Object>
class CStateMonsterAttackRunAttack : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackRunAttack<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    CStateMonsterAttackRunAttack(_Object* obj) : inherited{obj} {}

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
    virtual bool check_start_conditions();
};

#include "monster_state_attack_run_attack_inline.h"
