#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterSteal : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterSteal<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterSteal(_Object* obj);
    ~CStateMonsterSteal() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
    virtual bool check_start_conditions();

private:
    bool check_conditions();
};

#include "monster_state_steal_inline.h"
