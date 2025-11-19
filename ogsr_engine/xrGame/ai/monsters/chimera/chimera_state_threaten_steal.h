#pragma once

#include "../state.h"

template <typename _Object>
class CStateChimeraThreatenSteal : public CStateMonsterMoveToPointEx<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateChimeraThreatenSteal<_Object>, CStateMonsterMoveToPointEx<_Object>);

private:
    typedef CStateMonsterMoveToPointEx<_Object> inherited;
    using inherited::data;
    using inherited::inherited::object;

public:
    explicit CStateChimeraThreatenSteal(_Object* obj) : inherited{obj} {}
    ~CStateChimeraThreatenSteal() override = default;

    virtual void initialize();
    virtual void finalize();
    virtual void execute();
    virtual bool check_completion();
    virtual bool check_start_conditions();
};

#include "chimera_state_threaten_steal_inline.h"
