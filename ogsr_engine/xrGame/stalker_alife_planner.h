////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerALifePlanner : public CActionPlannerActionScript<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CStalkerALifePlanner, CActionPlannerActionScript<CAI_Stalker>);

private:
    typedef CActionPlannerActionScript<CAI_Stalker> inherited;

public:
    explicit CStalkerALifePlanner(CAI_Stalker* object = nullptr, LPCSTR action_name = "");
    ~CStalkerALifePlanner() override;

    virtual void setup(CAI_Stalker* object, CPropertyStorage* storage);
    void add_evaluators();
    void add_actions();
};
