////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_script.cpp
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "property_evaluator_const.h"
#include "script_game_object.h"

template <>
void CScriptPropertyEvaluator::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptPropertyEvaluator>("property_evaluator", sol::no_constructor, sol::call_constructor,
                                               sol::factories(std::make_unique<CScriptPropertyEvaluator>, std::make_unique<CScriptPropertyEvaluator, CScriptGameObject*>,
                                                              std::make_unique<CScriptPropertyEvaluator, CScriptGameObject*, LPCSTR>),
                                               "priv", &CScriptPropertyEvaluator::priv, "ops", &CScriptPropertyEvaluator::ops, "EVALUATE",
                                               sol::var(CScriptPropertyEvaluator::EVALUATE), "object", sol::readonly(&CScriptPropertyEvaluator::m_object), "storage",
                                               sol::readonly(&CScriptPropertyEvaluator::m_storage), "setup", &CScriptPropertyEvaluator::setup, "evaluate",
                                               &CScriptPropertyEvaluator::evaluate);

    lua.new_usertype<CPropertyEvaluatorConst<CScriptGameObject>>(
        "property_evaluator_const", sol::no_constructor, sol::call_constructor,
        sol::constructors<CPropertyEvaluatorConst<CScriptGameObject>(CPropertyEvaluatorConst<CScriptGameObject>::_value_type)>(), sol::base_classes,
        xr_sol_bases<CPropertyEvaluatorConst<CScriptGameObject>>());
}
