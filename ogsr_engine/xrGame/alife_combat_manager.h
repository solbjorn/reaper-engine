////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_combat_manager.h
//	Created 	: 12.08.2003
//  Modified 	: 14.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife combat manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"

class CALifeCombatManager : public virtual CALifeSimulatorBase
{
    RTTI_DECLARE_TYPEINFO(CALifeCombatManager, CALifeSimulatorBase);

public:
    CALifeCombatManager(xrServer* server);

    void kill_entity(CSE_ALifeMonsterAbstract* l_tpALifeMonsterAbstract, const GameGraph::_GRAPH_ID& l_tGraphID);
};
