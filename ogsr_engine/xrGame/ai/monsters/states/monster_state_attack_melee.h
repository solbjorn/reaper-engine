#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterAttackMelee : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterAttackMelee<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterAttackMelee(_Object* obj);
    ~CStateMonsterAttackMelee() override;

    void execute() override;

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_attack_melee_inline.h"
