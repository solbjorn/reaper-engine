////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator.h
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_interaction_manager.h"
#include "alife_update_manager.h"
#include "script_export_space.h"

class CALifeSimulator : public CALifeUpdateManager, public CALifeInteractionManager
{
    RTTI_DECLARE_TYPEINFO(CALifeSimulator, CALifeUpdateManager, CALifeInteractionManager);

protected:
    void setup_simulator(CSE_ALifeObject* object) override;
    void reload(LPCSTR section) override;

public:
    CALifeSimulator(xrServer* server, shared_str* command_line);
    virtual ~CALifeSimulator();

    void destroy() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CALifeSimulator);
#undef script_type_list
#define script_type_list save_type_list(CALifeSimulator)

#include "alife_simulator_inline.h"
