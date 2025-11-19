#pragma once

#include "../state.h"

template <typename _Object>
class CStateBurerAttackRunAround : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAttackRunAround<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    Fvector selected_point;
    u32 time_started{};

    Fvector dest_direction;

public:
    explicit CStateBurerAttackRunAround(_Object* obj);
    ~CStateBurerAttackRunAround() override = default;

    virtual void initialize();
    virtual void execute();

    virtual bool check_start_conditions();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "burer_state_attack_run_around_inline.h"
