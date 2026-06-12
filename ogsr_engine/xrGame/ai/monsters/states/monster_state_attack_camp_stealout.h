#pragma once

template <typename _Object>
class CStateMonsterAttackCampStealOut : public CStateMove<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackCampStealOut<_Object>, CStateMove<_Object>);

private:
    typedef CStateMove<_Object> inherited;
    using inherited::time_state_started;
    using inherited::inherited::object;

public:
    explicit CStateMonsterAttackCampStealOut(_Object* obj);
    ~CStateMonsterAttackCampStealOut() override = default;

    void execute() override;
    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_attack_camp_stealout_inline.h"
