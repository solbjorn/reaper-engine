#pragma once

#include "../state.h"

template <typename Object>
class CStateBurerAttack : public CState<Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAttack<Object>, CState<Object>);

private:
    typedef CState<Object> inherited;
    typedef CState<Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

public:
    explicit CStateBurerAttack(Object* obj);
    ~CStateBurerAttack() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    void finalize() override;
    void critical_finalize() override;
    [[nodiscard]] bool check_control_start_conditions(ControlCom::EControlType type) override;

private:
    bool m_wait_state_end;
    bool m_lost_delta_health;
    bool m_allow_anti_aim;
    float m_last_health{};
    TTime m_next_runaway_allowed_tick{};
};

#include "burer_state_attack_inline.h"
