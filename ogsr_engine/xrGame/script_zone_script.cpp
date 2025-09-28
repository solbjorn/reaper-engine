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

void CScriptZone::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptZone>("ce_script_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CScriptZone>), sol::base_classes,
                                  xr::sol_bases<CScriptZone>());
}

void CSmartZone::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSmartZone>("ce_smart_zone", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSmartZone>), "factory",
                                 &xr::client_factory<CSmartZone>, sol::base_classes, xr::sol_bases<CSmartZone>());
}
