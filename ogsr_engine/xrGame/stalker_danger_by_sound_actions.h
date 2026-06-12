////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_by_sound_actions.h
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger by sound actions classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundListenTo
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundListenTo : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerBySoundListenTo, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerBySoundListenTo(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerBySoundListenTo() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundCheck
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundCheck : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerBySoundCheck, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerBySoundCheck(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerBySoundCheck() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundTakeCover : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerBySoundTakeCover, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerBySoundTakeCover(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerBySoundTakeCover() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundLookOut : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerBySoundLookOut, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerBySoundLookOut(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerBySoundLookOut() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerBySoundLookAround
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDangerBySoundLookAround : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDangerBySoundLookAround, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDangerBySoundLookAround(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDangerBySoundLookAround() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};
