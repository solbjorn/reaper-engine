////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "alife_human_brain.h"

void CALifeHumanBrain::script_register(sol::state_view& lua)
{
    lua.new_usertype<CALifeHumanBrain>("CALifeHumanBrain", sol::no_constructor, sol::base_classes, xr::sol_bases<CALifeHumanBrain>());
}
