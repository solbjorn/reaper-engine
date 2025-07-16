////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone_script.cpp
//	Created 	: 10.10.2003
//  Modified 	: 11.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Script zone object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_zone.h"
#include "smart_zone.h"

void CScriptZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptZone>("ce_script_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CScriptZone>), sol::base_classes,
                                                 xr_sol_bases<CScriptZone>());
}

void CSmartZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSmartZone>("ce_smart_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSmartZone>), sol::base_classes,
                                                xr_sol_bases<CSmartZone>());
}
