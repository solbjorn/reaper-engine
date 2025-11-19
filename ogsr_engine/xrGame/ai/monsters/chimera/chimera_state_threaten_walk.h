#pragma once

#include "../state.h"

template <typename _Object>
class CStateChimeraThreatenWalk : public CStateMonsterMoveToPointEx<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateChimeraThreatenWalk<_Object>, CStateMonsterMoveToPointEx<_Object>);

private:
    typedef CStateMonsterMoveToPointEx<_Object> inherited;
    using inherited::data;
    using inherited::inherited::object;

public:
    explicit CStateChimeraThreatenWalk(_Object* obj) : inherited{obj} {}
    ~CStateChimeraThreatenWalk() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_completion();
    virtual bool check_start_conditions();
};

#include "chimera_state_threaten_walk_inline.h"
