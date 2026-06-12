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

    void initialize() override;
    void reselect_state() override;
    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;
    void setup_substates() override;
    void check_force_state() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_smart_terrain_task_inline.h"
