#pragma once

#include "../state.h"

template <typename _Object>
class CStateControllerPanic : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateControllerPanic<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;

    enum
    {
        eStateRun = u32(0),
        eStateSteal,
        eStateLookAround,
    };

public:
    explicit CStateControllerPanic(_Object* obj);
    ~CStateControllerPanic() override;

    virtual void reselect_state();
};

#include "controller_state_panic_inline.h"
