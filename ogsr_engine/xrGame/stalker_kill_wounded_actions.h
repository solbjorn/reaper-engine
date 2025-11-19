////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_actions.h
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionReachWounded : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionReachWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionReachWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionReachWounded() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionAimWounded : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionAimWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    //	float				m_speed;

public:
    explicit CStalkerActionAimWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionAimWounded() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPrepareWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPrepareWounded : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionPrepareWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionPrepareWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionPrepareWounded() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillWounded : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionKillWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionKillWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionKillWounded() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPauseAfterKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPauseAfterKill : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionPauseAfterKill, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionPauseAfterKill(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionPauseAfterKill() override = default;

    virtual void initialize();
    virtual void execute();
};
