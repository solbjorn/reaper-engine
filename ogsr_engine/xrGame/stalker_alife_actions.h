////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_base_action.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGatherItems
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGatherItems : public CStalkerActionBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionGatherItems, CStalkerActionBase);

protected:
    typedef CStalkerActionBase inherited;

public:
    explicit CStalkerActionGatherItems(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionGatherItems() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionNoALife
//////////////////////////////////////////////////////////////////////////

class CStalkerActionNoALife : public CStalkerActionBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionNoALife, CStalkerActionBase);

protected:
    typedef CStalkerActionBase inherited;

protected:
    u32 m_stop_weapon_handling_time{};

public:
    explicit CStalkerActionNoALife(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionNoALife() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};
