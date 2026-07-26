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

class CStalkerActionReachWounded final : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionReachWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionReachWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionReachWounded() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionAimWounded final : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionAimWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionAimWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionAimWounded() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPrepareWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPrepareWounded final : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionPrepareWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionPrepareWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionPrepareWounded() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillWounded
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillWounded final : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionKillWounded, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionKillWounded(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionKillWounded() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPauseAfterKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPauseAfterKill final : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionPauseAfterKill, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionPauseAfterKill(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionPauseAfterKill() override = default;

    void initialize() override;
    void execute() override;
};
