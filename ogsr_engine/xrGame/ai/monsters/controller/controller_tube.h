#pragma once

#include "../state.h"

template <typename _Object>
class CStateControllerTube : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateControllerTube<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateControllerTube(_Object* obj) : inherited{obj} {}
    ~CStateControllerTube() override = default;

    virtual void execute();
    virtual bool check_start_conditions();
    virtual bool check_completion();
};

#include "controller_tube_inline.h"
