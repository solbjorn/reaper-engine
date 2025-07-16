////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_point_script.cpp
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover point class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "cover_point.h"

void CCoverPoint::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CCoverPoint>("cover_point", sol::no_constructor, "position", &CCoverPoint::position, "level_vertex_id", &CCoverPoint::level_vertex_id);
}
