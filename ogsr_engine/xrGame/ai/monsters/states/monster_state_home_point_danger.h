#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterDangerMoveToHomePoint : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterDangerMoveToHomePoint<_Object>, CState<_Object>);

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
    using inherited::time_state_started;

    u32 m_target_node;
    bool m_skip_camp;
    Fvector m_danger_pos;

public:
    explicit CStateMonsterDangerMoveToHomePoint(_Object* obj);
    ~CStateMonsterDangerMoveToHomePoint() override = default;

    void initialize() override;
    void finalize() override;
    void critical_finalize() override;

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    void reselect_state() override;
    void setup_substates() override;

private:
    [[nodiscard]] Fvector& get_most_danger_pos();
};

#include "monster_state_home_point_danger_inline.h"
