#pragma once

#include "../state.h"

template <typename _Object>
class CStateBurerAttackGravi : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAttackGravi<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateBurerAttackGravi(_Object* obj);
    ~CStateBurerAttackGravi() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;

private:
    void ExecuteGraviStart();
    void ExecuteGraviContinue();
    void ExecuteGraviFire();

private:
    enum
    {
        ACTION_GRAVI_STARTED,
        ACTION_GRAVI_CONTINUE,
        ACTION_GRAVI_FIRE,
        ACTION_WAIT_ANIM_END,
        ACTION_COMPLETED,

    } m_action;

    u32 m_time_gravi_started{};
    TTime m_next_gravi_allowed_tick;
    TTime m_anim_end_tick;
};

#include "burer_state_attack_gravi_inline.h"
