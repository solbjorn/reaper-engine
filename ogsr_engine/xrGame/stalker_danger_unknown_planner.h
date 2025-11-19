////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_unknown_planner.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger unknown planner class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerDangerUnknownPlanner : public CActionPlannerActionScript<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CStalkerDangerUnknownPlanner, CActionPlannerActionScript<CAI_Stalker>);

private:
    typedef CActionPlannerActionScript<CAI_Stalker> inherited;

protected:
    void add_evaluators();
    void add_actions();

public:
    explicit CStalkerDangerUnknownPlanner(CAI_Stalker* object = nullptr, LPCSTR action_name = "");
    ~CStalkerDangerUnknownPlanner() override = default;

    virtual void setup(CAI_Stalker* object, CPropertyStorage* storage);
    virtual void initialize();
    virtual void update();
    virtual void finalize();
};
