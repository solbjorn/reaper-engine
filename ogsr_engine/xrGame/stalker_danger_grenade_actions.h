////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_grenade_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger grenade actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerGrenadeTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerGrenadeTakeCover : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerGrenadeTakeCover, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerGrenadeTakeCover(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerGrenadeTakeCover() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerGrenadeWaitForExplosion
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerGrenadeWaitForExplosion : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerGrenadeWaitForExplosion, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerGrenadeWaitForExplosion(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerGrenadeWaitForExplosion() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerGrenadeTakeCoverAfterExplosion
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerGrenadeTakeCoverAfterExplosion : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerGrenadeTakeCoverAfterExplosion, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    bool m_direction_sight{};

public:
    explicit CStalkerActionDangerGrenadeTakeCoverAfterExplosion(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerGrenadeTakeCoverAfterExplosion() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerGrenadeLookAround
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerGrenadeLookAround : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerGrenadeLookAround, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerGrenadeLookAround(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerGrenadeLookAround() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerGrenadeSearch
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerGrenadeSearch : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerGrenadeSearch, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerGrenadeSearch(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerGrenadeSearch() override = default;

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
};
