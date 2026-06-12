#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterSteal : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterSteal<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterSteal(_Object* obj);
    ~CStateMonsterSteal() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;

private:
    bool check_conditions();
};

#include "monster_state_steal_inline.h"
