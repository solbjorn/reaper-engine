#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterSmartTerrainTaskGraphWalk : public CStateMove<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterSmartTerrainTaskGraphWalk<_Object>, CStateMove<_Object>);

private:
    typedef CStateMove<_Object> inherited;
    using inherited::inherited::object;

    CALifeSmartTerrainTask* m_task;

public:
    explicit CStateMonsterSmartTerrainTaskGraphWalk(_Object* obj) : inherited{obj} {}
    ~CStateMonsterSmartTerrainTaskGraphWalk() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_start_conditions();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_smart_terrain_task_graph_walk_inline.h"
