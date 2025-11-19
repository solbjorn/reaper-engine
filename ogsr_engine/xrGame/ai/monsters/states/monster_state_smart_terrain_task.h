#pragma once

#include "../state.h"

#include "../../../alife_smart_terrain_task.h"

template <typename _Object>
class CStateMonsterSmartTerrainTask : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterSmartTerrainTask<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    CALifeSmartTerrainTask* m_current_task;

public:
    explicit CStateMonsterSmartTerrainTask(_Object* obj);
    ~CStateMonsterSmartTerrainTask() override;

    virtual void initialize();
    virtual void reselect_state();
    virtual bool check_start_conditions();
    virtual bool check_completion();
    virtual void setup_substates();
    virtual void check_force_state();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_smart_terrain_task_inline.h"
