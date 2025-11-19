////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_communication_manager.h
//	Created 	: 03.09.2003
//  Modified 	: 14.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife communication manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"

class CSE_ALifeSchedulable;
class CSE_ALifeHumanAbstract;
class CSE_ALifeTrader;
class CSE_ALifeTraderAbstract;

// TODO: Dima to Dima : Be attentive with this speed optimization - it doesn't suit to the OOP paradigm!
#define FAST_OWNERSHIP

class XR_NOVTABLE CALifeCommunicationManager : public virtual CALifeSimulatorBase
{
    RTTI_DECLARE_TYPEINFO(CALifeCommunicationManager, CALifeSimulatorBase);

public:
    CALifeCommunicationManager() = default;
    ~CALifeCommunicationManager() override = 0;
};

inline CALifeCommunicationManager::~CALifeCommunicationManager() = default;
