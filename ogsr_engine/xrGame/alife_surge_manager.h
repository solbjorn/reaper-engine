////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_surge_manager.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator surge manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_simulator_base.h"
#include "xrserver_space.h"

class CSE_ALifeTrader;

class CALifeSurgeManager : public virtual CALifeSimulatorBase
{
    RTTI_DECLARE_TYPEINFO(CALifeSurgeManager, CALifeSimulatorBase);

protected:
    typedef CALifeSimulatorBase inherited;

protected:
    xr_vector<ALife::_SPAWN_ID> m_temp_spawns;

private:
    void spawn_new_spawns();

protected:
    void spawn_new_objects();

public:
    IC CALifeSurgeManager(xrServer* server, LPCSTR section);
    virtual ~CALifeSurgeManager();
};

#include "alife_surge_manager_inline.h"
