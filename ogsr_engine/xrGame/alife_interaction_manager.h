////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_communication_manager.h
//	Created 	: 03.09.2003
//  Modified 	: 14.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife communication manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrserver_space.h"
#include "alife_combat_manager.h"
#include "alife_communication_manager.h"

class CALifeInteractionManager : public CALifeCombatManager, public CALifeCommunicationManager
{
    RTTI_DECLARE_TYPEINFO(CALifeInteractionManager, CALifeCombatManager, CALifeCommunicationManager);

public:
    CALifeInteractionManager(xrServer* server);
};
