#pragma once

template <typename _Object>
class CStateMonsterPanicRun : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterPanicRun<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterPanicRun(_Object* obj) : inherited{obj} {}
    ~CStateMonsterPanicRun() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
};

#include "monster_state_panic_run_inline.h"
