#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackRunAttack : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackRunAttack<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterAttackRunAttack(_Object* obj) : inherited{obj} {}
    ~CStateMonsterAttackRunAttack() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;
};

#include "monster_state_attack_run_attack_inline.h"
