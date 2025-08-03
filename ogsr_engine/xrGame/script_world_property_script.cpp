////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_property_script.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world property script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_world_property.h"
#include "operator_abstract.h"

void CScriptWorldPropertyWrapper::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptWorldProperty>("world_property", sol::no_constructor, sol::call_constructor,
                                           sol::constructors<CScriptWorldProperty(CScriptWorldProperty::condition_type, CScriptWorldProperty::value_type)>(), "condition",
                                           &CScriptWorldProperty::condition, "value", &CScriptWorldProperty::value);
}
