#pragma once

template <typename _Object>
class CStateGroupPanicRun : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupPanicRun<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateGroupPanicRun(_Object* obj) : inherited{obj} {}
    ~CStateGroupPanicRun() override = default;

    virtual void initialize();
    virtual void execute();

    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "group_state_panic_run_inline.h"
