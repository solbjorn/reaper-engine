////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_planner.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound planner class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerDangerBySoundPlanner : public CActionPlannerActionScript<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CStalkerDangerBySoundPlanner, CActionPlannerActionScript<CAI_Stalker>);

private:
    typedef CActionPlannerActionScript<CAI_Stalker> inherited;

protected:
    void add_evaluators();
    void add_actions();

public:
    explicit CStalkerDangerBySoundPlanner(CAI_Stalker* object = nullptr, LPCSTR action_name = "");
    ~CStalkerDangerBySoundPlanner() override = default;

    virtual void setup(CAI_Stalker* object, CPropertyStorage* storage);
    virtual void initialize();
    virtual void update();
    virtual void finalize();
};
