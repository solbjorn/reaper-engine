#pragma once

#include "../state.h"

template <typename _Object>
class CStateZombieAttackRun : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateZombieAttackRun<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    TTime m_time_action_change{};
    EAction action;

public:
    explicit CStateZombieAttackRun(_Object* obj);
    ~CStateZombieAttackRun() override;

    virtual void initialize();
    virtual void execute();

    virtual bool check_completion();
    virtual bool check_start_conditions();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

private:
    void choose_action();
};

#include "zombie_state_attack_run_inline.h"
