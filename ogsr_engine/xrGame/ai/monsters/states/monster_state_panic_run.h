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

    virtual void initialize();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
};

#include "monster_state_panic_run_inline.h"
