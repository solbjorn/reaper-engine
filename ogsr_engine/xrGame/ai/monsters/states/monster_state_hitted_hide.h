#pragma once

template <typename _Object>
class CStateMonsterHittedHide : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterHittedHide<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::object;
    using inherited::time_state_started;

public:
    explicit CStateMonsterHittedHide(_Object* obj) : inherited{obj} {}
    ~CStateMonsterHittedHide() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
    [[nodiscard]] bool check_start_conditions() override;
};

#include "monster_state_hitted_hide_inline.h"
