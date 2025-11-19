////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_death_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker death action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDead : public CStalkerActionBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDead, CStalkerActionBase);

protected:
    typedef CStalkerActionBase inherited;

private:
    bool fire() const;

public:
    explicit CStalkerActionDead(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDead() override = default;

    virtual void initialize();
    virtual void execute();
};
