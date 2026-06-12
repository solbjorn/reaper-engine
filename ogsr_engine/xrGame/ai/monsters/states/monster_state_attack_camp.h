#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackCamp : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackCamp<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    u32 m_target_node{};

public:
    explicit CStateMonsterAttackCamp(_Object* obj);
    ~CStateMonsterAttackCamp() override = default;

    void initialize() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;

    void check_force_state() override;
    void reselect_state() override;
    void setup_substates() override;
};

#include "monster_state_attack_camp_inline.h"
