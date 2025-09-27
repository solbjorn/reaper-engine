#pragma once

#include "PHReqComparer.h"

class CPHScriptCondition : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptCondition, CPHCondition, CPHReqComparerV);

public:
    sol::function m_lua_function;

    CPHScriptCondition(const CPHScriptCondition& func);
    CPHScriptCondition(const sol::function& func);
    virtual ~CPHScriptCondition();

    virtual bool is_true();
    virtual bool obsolete() const;
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptCondition* v) const { return m_lua_function == v->m_lua_function; }
};

class CPHScriptAction : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptAction, CPHAction, CPHReqComparerV);

public:
    sol::function m_lua_function;
    bool b_obsolete;

    CPHScriptAction(const sol::function& func);
    CPHScriptAction(const CPHScriptAction& action);
    virtual ~CPHScriptAction();

    virtual void run();
    virtual bool obsolete() const;
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptAction* v) const { return m_lua_function == v->m_lua_function; }
};

class CPHScriptObjectCondition : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectCondition, CPHCondition, CPHReqComparerV);

public:
    sol::object m_lua_object;
    shared_str m_method_name;

    CPHScriptObjectCondition(const sol::object& lua_object, LPCSTR method);
    CPHScriptObjectCondition(const CPHScriptObjectCondition& object);
    virtual ~CPHScriptObjectCondition();

    virtual bool is_true();
    virtual bool obsolete() const;
    virtual bool compare(const sol::object& v) const { return m_lua_object == v; }
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptObjectCondition* v) const;
};

class CPHScriptObjectAction : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectAction, CPHAction, CPHReqComparerV);

public:
    sol::object m_lua_object;
    shared_str m_method_name;
    bool b_obsolete;

    CPHScriptObjectAction(const sol::object& lua_object, LPCSTR method);
    CPHScriptObjectAction(const CPHScriptObjectAction& object);
    virtual ~CPHScriptObjectAction();

    virtual void run();
    virtual bool obsolete() const;
    virtual bool compare(const sol::object& v) const { return m_lua_object == v; }
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptObjectAction* v) const;
};

//////////////////////////////////////////////////////////////////////////////////////////

class CPHScriptObjectConditionN : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectConditionN, CPHCondition, CPHReqComparerV);

public:
    sol::function m_callback;
    sol::object m_object;

    CPHScriptObjectConditionN(const sol::object& object, const sol::function& function);
    virtual ~CPHScriptObjectConditionN();

    virtual bool is_true();
    virtual bool obsolete() const;
    virtual bool compare(const sol::object& v) const { return m_object == v; }
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptObjectConditionN* v) const { return m_callback == v->m_callback && m_object == v->m_object; }
};

class CPHScriptObjectActionN : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectActionN, CPHAction, CPHReqComparerV);

public:
    sol::function m_callback;
    sol::object m_object;
    bool b_obsolete;

    CPHScriptObjectActionN(const sol::object& object, const sol::function& function);
    virtual ~CPHScriptObjectActionN();

    virtual void run();
    virtual bool obsolete() const;
    virtual bool compare(const sol::object& v) const { return m_object == v; }
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHScriptObjectActionN* v) const { return m_callback == v->m_callback && m_object == v->m_object; }
};

class CPHScriptGameObjectCondition : public CPHScriptObjectConditionN
{
    RTTI_DECLARE_TYPEINFO(CPHScriptGameObjectCondition, CPHScriptObjectConditionN);

public:
    CObject* m_obj;
    bool b_obsolete;

    CPHScriptGameObjectCondition(const sol::object& object, const sol::function& function, CObject* gobj) : CPHScriptObjectConditionN(object, function)
    {
        m_obj = gobj;
        b_obsolete = false;
    }

    virtual bool is_true()
    {
        b_obsolete = CPHScriptObjectConditionN::is_true();
        return b_obsolete;
    }
    virtual bool compare(const CObject* v) const { return m_obj->ID() == v->ID(); }
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool obsolete() const { return b_obsolete; }
};

class CPHScriptGameObjectAction : public CPHScriptObjectActionN
{
    RTTI_DECLARE_TYPEINFO(CPHScriptGameObjectAction, CPHScriptObjectActionN);

public:
    CObject* m_obj;

    CPHScriptGameObjectAction(const sol::object& object, const sol::function& function, CObject* gobj) : CPHScriptObjectActionN(object, function) { m_obj = gobj; }

    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CObject* v) const { return m_obj->ID() == v->ID(); }
};

class CPHSriptReqObjComparer : public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHSriptReqObjComparer, CPHReqComparerV);

public:
    sol::object m_lua_object;

    CPHSriptReqObjComparer(const sol::object& lua_object) { m_lua_object = lua_object; }
    CPHSriptReqObjComparer(const CPHSriptReqObjComparer& object) { m_lua_object = object.m_lua_object; }
    virtual ~CPHSriptReqObjComparer() {}

    virtual bool compare(const CPHScriptObjectCondition* v) const { return v->compare(m_lua_object); }
    virtual bool compare(const CPHScriptObjectAction* v) const { return v->compare(m_lua_object); }
    virtual bool compare(const CPHScriptObjectConditionN* v) const { return v->compare(m_lua_object); }
    virtual bool compare(const CPHScriptObjectActionN* v) const { return v->compare(m_lua_object); }
};

class CPHSriptReqGObjComparer : public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHSriptReqGObjComparer, CPHReqComparerV);

public:
    CObject* m_object;

    CPHSriptReqGObjComparer(CObject* object) { m_object = object; }
    virtual bool compare(const CPHScriptGameObjectAction* v) const { return v->compare(m_object); }
    virtual bool compare(const CPHScriptGameObjectCondition* v) const { return v->compare(m_object); }
};
