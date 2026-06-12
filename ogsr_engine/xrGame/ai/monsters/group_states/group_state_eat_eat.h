#pragma once

#include "../state.h"

template <typename _Object>
class CStateGroupEating : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupEating<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    CEntityAlive* corpse;
    u32 time_last_eat{};

public:
    explicit CStateGroupEating(_Object* obj);
    ~CStateGroupEating() override;

    void initialize() override;
    void execute() override;

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;
    void remove_links(CObject* object) override;
};

#include "group_state_eat_eat_inline.h"
