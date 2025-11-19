#pragma once

#include "../state.h"
#include "state_data.h"

template <typename _Object>
class CStateMonsterMoveAroundPoint : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterMoveAroundPoint<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

    SStateDataMoveAroundPoint data;

public:
    explicit CStateMonsterMoveAroundPoint(_Object* obj) : inherited{obj, &data} {}
    ~CStateMonsterMoveAroundPoint() override = default;

    virtual void initialize();
    virtual void execute();

    virtual bool check_completion();
};

#include "state_move_to_point_inline.h"
