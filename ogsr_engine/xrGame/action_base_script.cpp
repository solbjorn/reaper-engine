////////////////////////////////////////////////////////////////////////////
//	Module 		: action_base_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "action_base.h"
#include "script_game_object.h"

template <>
void CActionBase<CScriptGameObject>::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptActionBase>(
        "action_base", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptActionBase(), CScriptActionBase(CScriptGameObject*), CScriptActionBase(CScriptGameObject*, LPCSTR)>(), "object",
        sol::readonly(&CScriptActionBase::m_object), "storage", sol::readonly(&CScriptActionBase::m_storage), "add_precondition",
        sol::resolve<void(const CScriptActionBase::COperatorCondition&)>(&CScriptActionBase::add_condition), "add_effect",
        sol::resolve<void(const CScriptActionBase::COperatorCondition&)>(&CScriptActionBase::add_effect), "remove_precondition",
        sol::resolve<void(const CScriptActionBase::COperatorCondition::condition_type&)>(&CScriptActionBase::remove_condition), "remove_effect",
        sol::resolve<void(const CScriptActionBase::COperatorCondition::condition_type&)>(&CScriptActionBase::remove_effect), "setup", &CScriptActionBase::setup, "initialize",
        &CScriptActionBase::initialize, "execute", &CScriptActionBase::execute, "finalize", &CScriptActionBase::finalize, "weight", &CScriptActionBase::weight, "set_weight",
        &CScriptActionBase::set_weight
#ifdef LOG_ACTION
        ,
        "show", &CScriptActionBase::show
#endif
    );
}
