////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_planner.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker planner class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_script.h"
#include "action_script_base.h"
#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerPlanner final : public CActionPlannerScript<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CStalkerPlanner, CActionPlannerScript<CAI_Stalker>);

protected:
    typedef CActionPlannerScript<CAI_Stalker> inherited;
    typedef CActionScriptBase<CAI_Stalker> CAction;
    typedef GraphEngineSpace::_solver_value_type _value_type;
    typedef GraphEngineSpace::_solver_condition_type _condition_type;
    typedef CActionPlannerActionScript<CAI_Stalker> CActionPlannerAction;

private:
    bool m_active;
    bool m_affect_cover;
    GraphEngineSpace::CWorldState m_alive_goal;
    GraphEngineSpace::CWorldState m_dead_goal;

protected:
    void add_evaluators();
    void add_actions();

public:
#ifdef LOG_ACTION
    [[nodiscard]] LPCSTR action2string(const _action_id_type& action_id);
    [[nodiscard]] LPCSTR property2string(const _condition_type& property_id);
#endif

    CStalkerPlanner();
    ~CStalkerPlanner() override;

    void setup(CAI_Stalker* object) override;
    void update(u32);
    IC void affect_cover(bool value);
    IC bool affect_cover() const;
    IC void active(bool);
    IC bool active() const;

#ifdef LOG_ACTION
    [[nodiscard]] LPCSTR object_name() const;
#endif
};

#include "stalker_planner_inline.h"
