////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_unknown_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger unknown actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerUnknownTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerUnknownTakeCover : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerUnknownTakeCover, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    bool m_direction_sight{};

public:
    explicit CStalkerActionDangerUnknownTakeCover(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerUnknownTakeCover() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerUnknownLookAround
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerUnknownLookAround : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerUnknownLookAround, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerUnknownLookAround(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerUnknownLookAround() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerUnknownSearch
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerUnknownSearch : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerUnknownSearch, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerUnknownSearch(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerUnknownSearch() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};
