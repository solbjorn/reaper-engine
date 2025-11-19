#pragma once

#include "../state.h"

template <typename _Object>
class CStateControllerFastMove : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateControllerFastMove<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;

public:
    explicit CStateControllerFastMove(_Object* obj) : inherited{obj} {}
    ~CStateControllerFastMove() override = default;

    virtual void initialize();
    virtual void finalize();
    virtual void critical_finalize();

    virtual void execute();
};

#include "controller_state_attack_fast_move_inline.h"
