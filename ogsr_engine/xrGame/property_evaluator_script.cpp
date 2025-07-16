////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_script.cpp
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_game_object.h"
#include "property_evaluator_const.h"

template <>
void CScriptPropertyEvaluator::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CScriptPropertyEvaluator>(
        "property_evaluator", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptPropertyEvaluator(), CScriptPropertyEvaluator(CScriptGameObject*), CScriptPropertyEvaluator(CScriptGameObject*, LPCSTR)>(), "object",
        sol::readonly(&CScriptPropertyEvaluator::m_object), "storage", sol::readonly(&CScriptPropertyEvaluator::m_storage), "setup", &CScriptPropertyEvaluator::setup, "evaluate",
        &CScriptPropertyEvaluator::evaluate);

    lua.new_usertype<CPropertyEvaluatorConst<CScriptGameObject>>(
        "property_evaluator_const", sol::no_constructor, sol::call_constructor,
        sol::constructors<CPropertyEvaluatorConst<CScriptGameObject>(CPropertyEvaluatorConst<CScriptGameObject>::_value_type)>(), sol::base_classes,
        xr_sol_bases<CPropertyEvaluatorConst<CScriptGameObject>>());
}
