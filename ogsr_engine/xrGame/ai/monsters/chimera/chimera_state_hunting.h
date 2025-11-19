#pragma once

#include "../state.h"

template <typename _Object>
class CStateChimeraHunting : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateChimeraHunting<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;

    enum
    {
        eStateMoveToCover,
        eStateComeOut
    };

public:
    explicit CStateChimeraHunting(_Object* obj);
    ~CStateChimeraHunting() override = default;

    virtual void reselect_state();
    virtual bool check_start_conditions();
    virtual bool check_completion();
};

#include "chimera_state_hunting_inline.h"
