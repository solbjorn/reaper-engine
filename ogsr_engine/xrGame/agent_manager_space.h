////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_space.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AgentManager
{
enum EProperties : u32
{
    ePropertyOrders = u32(0),
    ePropertyItem,
    ePropertyEnemy,
    ePropertyDanger,

    ePropertyScript,
};

enum EOperators : u32
{
    eOperatorNoOrders = u32(0),
    eOperatorGatherItem,
    eOperatorKillEnemy,
    eOperatorReactOnDanger,

    eOperatorScript,
};
}; // namespace AgentManager
