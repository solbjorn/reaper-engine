////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_actions.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAgentManager;

typedef CActionBase<CAgentManager> CAgentManagerActionBase;

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionNoOrders
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionNoOrders : public CAgentManagerActionBase
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerActionNoOrders, CAgentManagerActionBase);

protected:
    typedef CAgentManagerActionBase inherited;

public:
    explicit CAgentManagerActionNoOrders(CAgentManager* object, LPCSTR action_name = "");
    ~CAgentManagerActionNoOrders() override = default;

    virtual void finalize();
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionGatherItems
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionGatherItems : public CAgentManagerActionBase
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerActionGatherItems, CAgentManagerActionBase);

protected:
    typedef CAgentManagerActionBase inherited;

public:
    explicit CAgentManagerActionGatherItems(CAgentManager* object, LPCSTR action_name = "");
    ~CAgentManagerActionGatherItems() override = default;
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionKillEnemy : public CAgentManagerActionBase
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerActionKillEnemy, CAgentManagerActionBase);

protected:
    typedef CAgentManagerActionBase inherited;

public:
    explicit CAgentManagerActionKillEnemy(CAgentManager* object, LPCSTR action_name = "");
    ~CAgentManagerActionKillEnemy() override = default;

    virtual void initialize();
    virtual void finalize();
    virtual void execute();
};

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionReactOnDanger
//////////////////////////////////////////////////////////////////////////

class CAgentManagerActionReactOnDanger : public CAgentManagerActionBase
{
    RTTI_DECLARE_TYPEINFO(CAgentManagerActionReactOnDanger, CAgentManagerActionBase);

protected:
    typedef CAgentManagerActionBase inherited;

public:
    explicit CAgentManagerActionReactOnDanger(CAgentManager* object, LPCSTR action_name = "");
    ~CAgentManagerActionReactOnDanger() override = default;

    virtual void initialize();
    virtual void execute();
};
