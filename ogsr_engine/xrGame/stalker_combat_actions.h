////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_action_base.h"

class CCoverPoint;

namespace MonsterSpace
{
enum EBodyState : u32;
enum EMovementType : u32;
} // namespace MonsterSpace

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetItemToKill : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionGetItemToKill, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionGetItemToKill(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionGetItemToKill() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

class CStalkerActionMakeItemKilling : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionMakeItemKilling, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionMakeItemKilling(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionMakeItemKilling() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionRetreatFromEnemy : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionRetreatFromEnemy, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionRetreatFromEnemy(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionRetreatFromEnemy() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    [[nodiscard]] edge_value_type weight(const CSConditionState&, const CSConditionState&) const override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKill : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionGetReadyToKill, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    MonsterSpace::EBodyState m_body_state;
    bool m_affect_properties;
    bool m_enable_enemy_change;

public:
    explicit CStalkerActionGetReadyToKill(bool affect_properties, CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionGetReadyToKill() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemy : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionKillEnemy, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionKillEnemy(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionKillEnemy() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCover
//////////////////////////////////////////////////////////////////////////

class CStalkerActionTakeCover : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionTakeCover, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    MonsterSpace::EBodyState m_body_state;
    MonsterSpace::EMovementType m_movement_type;

public:
    explicit CStalkerActionTakeCover(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionTakeCover() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionLookOut
//////////////////////////////////////////////////////////////////////////

class CStalkerActionLookOut : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionLookOut, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

private:
    u32 m_last_change_time;

public:
    explicit CStalkerActionLookOut(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionLookOut() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHoldPosition
//////////////////////////////////////////////////////////////////////////

class CStalkerActionHoldPosition : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionHoldPosition, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionHoldPosition(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionHoldPosition() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetDistance
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetDistance : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionGetDistance, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionGetDistance(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionGetDistance() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetourEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionDetourEnemy : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionDetourEnemy, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionDetourEnemy(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionDetourEnemy() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSearchEnemy
//////////////////////////////////////////////////////////////////////////

class CStalkerActionSearchEnemy : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionSearchEnemy, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionSearchEnemy(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionSearchEnemy() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPostCombatWait
//////////////////////////////////////////////////////////////////////////

class CStalkerActionPostCombatWait : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionPostCombatWait, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionPostCombatWait(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionPostCombatWait() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHideFromGrenade
//////////////////////////////////////////////////////////////////////////

class CStalkerActionHideFromGrenade : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionHideFromGrenade, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionHideFromGrenade(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionHideFromGrenade() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSuddenAttack
//////////////////////////////////////////////////////////////////////////

class CStalkerActionSuddenAttack : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionSuddenAttack, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionSuddenAttack(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionSuddenAttack() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyIfPlayerOnThePath
//////////////////////////////////////////////////////////////////////////

class CStalkerActionKillEnemyIfPlayerOnThePath : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionKillEnemyIfPlayerOnThePath, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionKillEnemyIfPlayerOnThePath(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionKillEnemyIfPlayerOnThePath() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCriticalHit
//////////////////////////////////////////////////////////////////////////

class CStalkerActionCriticalHit : public CStalkerActionCombatBase
{
    RTTI_DECLARE_TYPEINFO(CStalkerActionCriticalHit, CStalkerActionCombatBase);

protected:
    typedef CStalkerActionCombatBase inherited;

public:
    explicit CStalkerActionCriticalHit(CAI_Stalker* object, LPCSTR action_name = "");
    ~CStalkerActionCriticalHit() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

float current_cover(CAI_Stalker* object);

#include "stalker_combat_actions_inline.h"
