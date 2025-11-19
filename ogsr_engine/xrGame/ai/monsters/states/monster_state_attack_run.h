#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackRun : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackRun<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    TTime m_time_path_rebuild{};

public:
    explicit CStateMonsterAttackRun(_Object* obj) : inherited{obj} {}
    ~CStateMonsterAttackRun() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
    virtual bool check_start_conditions();
};

#include "monster_state_attack_run_inline.h"
