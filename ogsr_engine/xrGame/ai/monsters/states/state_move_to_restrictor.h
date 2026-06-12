#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterMoveToRestrictor : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterMoveToRestrictor<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterMoveToRestrictor(_Object* obj) : inherited{obj} {}
    ~CStateMonsterMoveToRestrictor() override = default;

    void initialize() override;
    void execute() override;

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "state_move_to_restrictor_inline.h"
