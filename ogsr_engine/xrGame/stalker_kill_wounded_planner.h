////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_planner.h
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerKillWoundedPlanner : public CActionPlannerActionScript<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CStalkerKillWoundedPlanner, CActionPlannerActionScript<CAI_Stalker>);

private:
    typedef CActionPlannerActionScript<CAI_Stalker> inherited;

private:
    void add_evaluators();
    void add_actions();

public:
    explicit CStalkerKillWoundedPlanner(CAI_Stalker* object = nullptr, LPCSTR action_name = "");
    ~CStalkerKillWoundedPlanner() override;

    virtual void setup(CAI_Stalker* object, CPropertyStorage* storage);
    virtual void update();
    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};
