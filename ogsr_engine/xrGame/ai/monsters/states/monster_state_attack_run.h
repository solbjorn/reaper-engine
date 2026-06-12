#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackRun : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackRun<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    TTime m_time_path_rebuild{};

public:
    explicit CStateMonsterAttackRun(_Object* obj) : inherited{obj} {}
    ~CStateMonsterAttackRun() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;
};

#include "monster_state_attack_run_inline.h"
