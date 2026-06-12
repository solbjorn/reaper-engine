#pragma once

#include "../state.h"

template <typename _Object>
class CStatePoltergeistAttackHidden : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStatePoltergeistAttackHidden<_Object>, CState<_Object>);

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

public:
    explicit CStatePoltergeistAttackHidden(_Object* obj);
    ~CStatePoltergeistAttackHidden() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_home_point();

private:
    void select_target_for_move();

    u32 m_fly_side_select_tick;
    float m_fly_radius_factor;
    bool m_fly_left;
    Fvector m_target;
    u32 m_target_vertex;
};

#include "poltergeist_state_attack_hidden_inline.h"
