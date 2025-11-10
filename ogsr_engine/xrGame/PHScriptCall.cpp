#include "stdafx.h"

#include "PHScriptCall.h"

#include "..\xr_3da\xr_object.h"

CPHScriptCondition::CPHScriptCondition(const sol::function& func) { m_lua_function = func; }
CPHScriptCondition::CPHScriptCondition(const CPHScriptCondition& func) { m_lua_function = func.m_lua_function; }
CPHScriptCondition::~CPHScriptCondition() = default;

bool CPHScriptCondition::is_true() { return m_lua_function(); }
bool CPHScriptCondition::obsolete() const { return false; }

//
CPHScriptAction::CPHScriptAction(const sol::function& func)
{
    m_lua_function = func;
    b_obsolete = false;
}

CPHScriptAction::CPHScriptAction(const CPHScriptAction& action)
{
    m_lua_function = action.m_lua_function;
    b_obsolete = action.b_obsolete;
}

CPHScriptAction::~CPHScriptAction() = default;

void CPHScriptAction::run()
{
    m_lua_function();
    b_obsolete = true;
}

bool CPHScriptAction::obsolete() const { return b_obsolete; }

/////////////////////////////////////////////////////////////////////////////////////////////
CPHScriptObjectAction::CPHScriptObjectAction(const sol::object& lua_object, LPCSTR method)
{
    m_lua_object = lua_object;
    m_method_name._set(method);

    b_obsolete = false;
}

CPHScriptObjectAction::CPHScriptObjectAction(const CPHScriptObjectAction& object)
{
    m_lua_object = object.m_lua_object;
    m_method_name = object.m_method_name;

    b_obsolete = object.b_obsolete;
}

CPHScriptObjectAction::~CPHScriptObjectAction() = default;

bool CPHScriptObjectAction::compare(const CPHScriptObjectAction* v) const { return m_method_name == v->m_method_name && m_lua_object == v->m_lua_object; }

void CPHScriptObjectAction::run()
{
    sol::function func;

    switch (m_lua_object.get_type())
    {
    case sol::type::userdata: func = m_lua_object.as<sol::userdata>()[*m_method_name]; break;
    case sol::type::table: func = m_lua_object.as<sol::table>()[*m_method_name]; break;
    default: break;
    }

    R_ASSERT(func);

    func(m_lua_object);
    b_obsolete = true;
}

bool CPHScriptObjectAction::obsolete() const { return b_obsolete; }

//
CPHScriptObjectCondition::CPHScriptObjectCondition(const sol::object& lua_object, LPCSTR method)
{
    m_lua_object = lua_object;
    m_method_name._set(method);
}

CPHScriptObjectCondition::CPHScriptObjectCondition(const CPHScriptObjectCondition& object)
{
    m_lua_object = object.m_lua_object;
    m_method_name = object.m_method_name;
}

CPHScriptObjectCondition::~CPHScriptObjectCondition() = default;

bool CPHScriptObjectCondition::compare(const CPHScriptObjectCondition* v) const { return m_method_name == v->m_method_name && m_lua_object == v->m_lua_object; }

bool CPHScriptObjectCondition::is_true()
{
    sol::function func;

    switch (m_lua_object.get_type())
    {
    case sol::type::userdata: func = m_lua_object.as<sol::userdata>()[*m_method_name]; break;
    case sol::type::table: func = m_lua_object.as<sol::table>()[*m_method_name]; break;
    default: break;
    }

    R_ASSERT(func);

    return func(m_lua_object);
}

bool CPHScriptObjectCondition::obsolete() const { return false; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CPHScriptObjectActionN::CPHScriptObjectActionN(const sol::object& object, const sol::function& function)
{
    m_callback = function;
    m_object = object;

    b_obsolete = false;
}

CPHScriptObjectActionN::~CPHScriptObjectActionN() {}

void CPHScriptObjectActionN::run()
{
    m_callback(m_object);
    b_obsolete = true;
}

bool CPHScriptObjectActionN::obsolete() const { return b_obsolete; }

CPHScriptObjectConditionN::CPHScriptObjectConditionN(const sol::object& object, const sol::function& function)
{
    m_callback = function;
    m_object = object;
}

CPHScriptObjectConditionN::~CPHScriptObjectConditionN() {}

bool CPHScriptObjectConditionN::is_true() { return m_callback(m_object); }

bool CPHScriptObjectConditionN::obsolete() const { return false; }
