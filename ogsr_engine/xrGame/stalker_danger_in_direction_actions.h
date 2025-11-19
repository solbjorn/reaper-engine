////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionTakeCover : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerInDirectionTakeCover, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerInDirectionTakeCover(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerInDirectionTakeCover() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionLookOut : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerInDirectionLookOut, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerInDirectionLookOut(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerInDirectionLookOut() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionHoldPosition
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionHoldPosition : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerInDirectionHoldPosition, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerInDirectionHoldPosition(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerInDirectionHoldPosition() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionDetour
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionDetour : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerInDirectionDetour, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerInDirectionDetour(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerInDirectionDetour() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionSearch
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerInDirectionSearch : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerInDirectionSearch, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerInDirectionSearch(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerInDirectionSearch() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};
