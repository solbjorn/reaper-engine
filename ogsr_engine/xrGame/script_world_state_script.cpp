////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_state_script.cpp
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world state script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "script_world_state.h"
#include "condition_state.h"

void CScriptWorldStateWrapper::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptWorldState>("world_state", sol::no_constructor, sol::call_constructor,
                                                       sol::constructors<CScriptWorldState(), CScriptWorldState(CScriptWorldState)>(), "add_property",
                                                       sol::resolve<void(const CScriptWorldState::COperatorCondition&)>(&CScriptWorldState::add_condition), "remove_property",
                                                       sol::resolve<void(const CScriptWorldState::COperatorCondition::condition_type&)>(&CScriptWorldState::remove_condition),
                                                       "clear", &CScriptWorldState::clear, "includes", &CScriptWorldState::includes, "property", &CScriptWorldState::property);
}
