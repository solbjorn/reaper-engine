#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterRestMoveToHomePoint : public CStateMove<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterRestMoveToHomePoint<_Object>, CStateMove<_Object>);

private:
    typedef CStateMove<_Object> inherited;
    typedef CStateMove<_Object>* state_ptr;
    using inherited::inherited::object;

    u32 m_target_node{};

public:
    explicit CStateMonsterRestMoveToHomePoint(_Object* obj) : inherited{obj} {}
    ~CStateMonsterRestMoveToHomePoint() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_start_conditions();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_home_point_rest_inline.h"
