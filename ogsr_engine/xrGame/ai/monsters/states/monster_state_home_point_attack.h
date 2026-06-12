#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackMoveToHomePoint : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackMoveToHomePoint<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::object;

    u32 m_target_node;
    Fvector m_target_pos;
    bool m_skip_camp;
    TTime m_selected_target_time;

public:
    explicit CStateMonsterAttackMoveToHomePoint(_Object* obj);
    ~CStateMonsterAttackMoveToHomePoint() override = default;

    void initialize() override;
    void finalize() override;
    void critical_finalize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;

private:
    void select_target();
    void clean();
};

#include "monster_state_home_point_attack_inline.h"
