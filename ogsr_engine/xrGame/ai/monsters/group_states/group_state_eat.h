#pragma once

#include "../state.h"

template <typename _Object>
class CStateGroupEat : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupEat<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    const CEntityAlive* corpse;

    u32 m_time_last_eat{};

public:
    explicit CStateGroupEat(_Object* obj);
    ~CStateGroupEat() override;

    void reinit() override;
    void initialize() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override;

    void reselect_state() override;
    void setup_substates() override;
    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;

private:
    [[nodiscard]] bool hungry();
};

#include "group_state_eat_inline.h"
