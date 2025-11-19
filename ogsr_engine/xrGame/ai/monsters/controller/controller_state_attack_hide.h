#pragma once

template <typename _Object>
class CStateControlHide : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateControlHide<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::object;

    bool m_cover_reached;

    struct
    {
        Fvector position;
        u32 node;
    } target;

    u32 m_time_finished;

    bool m_state_fast_run;

public:
    explicit CStateControlHide(_Object* obj) : inherited{obj} {}
    ~CStateControlHide() override = default;

    virtual void initialize();
    virtual void execute();

    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
    virtual bool check_start_conditions();

private:
    void select_target_point();
};

#include "controller_state_attack_hide_inline.h"
