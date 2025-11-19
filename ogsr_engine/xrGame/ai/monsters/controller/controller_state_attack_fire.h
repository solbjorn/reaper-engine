#pragma once

template <typename _Object>
class CStateControlFire : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateControlFire<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    u32 m_time_started;
    u32 m_time_state_last_execute;

public:
    explicit CStateControlFire(_Object* obj) : inherited{obj} {}
    ~CStateControlFire() override = default;

    virtual void reinit();
    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();
    virtual bool check_completion();
    virtual bool check_start_conditions();
};

#include "controller_state_attack_fire_inline.h"
