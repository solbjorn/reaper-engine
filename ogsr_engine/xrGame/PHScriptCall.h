#pragma once

#include "PHCommander.h"
#include "PHReqComparer.h"

class CPHScriptCondition : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptCondition, CPHCondition, CPHReqComparerV);

public:
    sol::function m_lua_function;

    explicit CPHScriptCondition(const CPHScriptCondition& func);
    explicit CPHScriptCondition(const sol::function& func);
    ~CPHScriptCondition() override;

    [[nodiscard]] bool is_true() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptCondition* v) const override { return m_lua_function == v->m_lua_function; }
};

class CPHScriptAction : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptAction, CPHAction, CPHReqComparerV);

public:
    sol::function m_lua_function;
    bool b_obsolete;

    explicit CPHScriptAction(const sol::function& func);
    explicit CPHScriptAction(const CPHScriptAction& action);
    ~CPHScriptAction() override;

    void run() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptAction* v) const override { return m_lua_function == v->m_lua_function; }
};

class CPHScriptObjectCondition : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectCondition, CPHCondition, CPHReqComparerV);

public:
    sol::object m_lua_object;
    shared_str m_method_name;

    explicit CPHScriptObjectCondition(const sol::object& lua_object, LPCSTR method);
    explicit CPHScriptObjectCondition(const CPHScriptObjectCondition& object);
    ~CPHScriptObjectCondition() override;

    [[nodiscard]] bool is_true() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] virtual bool compare(const sol::object& v) const { return m_lua_object == v; }
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptObjectCondition* v) const override;
};

class CPHScriptObjectAction : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectAction, CPHAction, CPHReqComparerV);

public:
    sol::object m_lua_object;
    shared_str m_method_name;
    bool b_obsolete;

    explicit CPHScriptObjectAction(const sol::object& lua_object, LPCSTR method);
    explicit CPHScriptObjectAction(const CPHScriptObjectAction& object);
    ~CPHScriptObjectAction() override;

    void run() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] virtual bool compare(const sol::object& v) const { return m_lua_object == v; }
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptObjectAction* v) const override;
};

//////////////////////////////////////////////////////////////////////////////////////////

class CPHScriptObjectConditionN : public CPHCondition, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectConditionN, CPHCondition, CPHReqComparerV);

public:
    sol::function m_callback;
    sol::object m_object;

    explicit CPHScriptObjectConditionN(const sol::object& object, const sol::function& function);
    ~CPHScriptObjectConditionN() override;

    [[nodiscard]] bool is_true() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] virtual bool compare(const sol::object& v) const { return m_object == v; }
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptObjectConditionN* v) const override { return m_callback == v->m_callback && m_object == v->m_object; }
};

class CPHScriptObjectActionN : public CPHAction, public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHScriptObjectActionN, CPHAction, CPHReqComparerV);

public:
    sol::function m_callback;
    sol::object m_object;
    bool b_obsolete;

    explicit CPHScriptObjectActionN(const sol::object& object, const sol::function& function);
    ~CPHScriptObjectActionN() override;

    void run() override;
    [[nodiscard]] bool obsolete() const override;
    [[nodiscard]] virtual bool compare(const sol::object& v) const { return m_object == v; }
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool compare(const CPHScriptObjectActionN* v) const override { return m_callback == v->m_callback && m_object == v->m_object; }
};

class CPHScriptGameObjectCondition : public CPHScriptObjectConditionN
{
    RTTI_DECLARE_TYPEINFO(CPHScriptGameObjectCondition, CPHScriptObjectConditionN);

public:
    CObject* m_obj;
    bool b_obsolete{};

    explicit CPHScriptGameObjectCondition(const sol::object& object, const sol::function& function, CObject* gobj)
        : CPHScriptObjectConditionN{object, function}, m_obj{gobj}
    {}
    ~CPHScriptGameObjectCondition() override = default;

    [[nodiscard]] bool is_true() override
    {
        b_obsolete = CPHScriptObjectConditionN::is_true();
        return b_obsolete;
    }

    [[nodiscard]] virtual bool compare(const CObject* v) const { return m_obj->ID() == v->ID(); }
    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] bool obsolete() const override { return b_obsolete; }
};

class CPHScriptGameObjectAction : public CPHScriptObjectActionN
{
    RTTI_DECLARE_TYPEINFO(CPHScriptGameObjectAction, CPHScriptObjectActionN);

public:
    CObject* m_obj;

    explicit CPHScriptGameObjectAction(const sol::object& object, const sol::function& function, CObject* gobj)
        : CPHScriptObjectActionN{object, function}, m_obj{gobj}
    {}
    ~CPHScriptGameObjectAction() override = default;

    [[nodiscard]] bool compare(const CPHReqComparerV* v) const override { return v->compare(this); }
    [[nodiscard]] virtual bool compare(const CObject* v) const { return m_obj->ID() == v->ID(); }
};

class CPHSriptReqObjComparer : public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHSriptReqObjComparer, CPHReqComparerV);

public:
    sol::object m_lua_object;

    explicit CPHSriptReqObjComparer(const sol::object& lua_object) : m_lua_object{lua_object} {}
    explicit CPHSriptReqObjComparer(const CPHSriptReqObjComparer& object) : m_lua_object{object.m_lua_object} {}
    ~CPHSriptReqObjComparer() override = default;

    [[nodiscard]] bool compare(const CPHScriptObjectCondition* v) const override { return v->compare(m_lua_object); }
    [[nodiscard]] bool compare(const CPHScriptObjectAction* v) const override { return v->compare(m_lua_object); }
    [[nodiscard]] bool compare(const CPHScriptObjectConditionN* v) const override { return v->compare(m_lua_object); }
    [[nodiscard]] bool compare(const CPHScriptObjectActionN* v) const override { return v->compare(m_lua_object); }
};

class CPHSriptReqGObjComparer : public CPHReqComparerV
{
    RTTI_DECLARE_TYPEINFO(CPHSriptReqGObjComparer, CPHReqComparerV);

public:
    CObject* m_object;

    explicit CPHSriptReqGObjComparer(CObject* object) : m_object{object} {}
    ~CPHSriptReqGObjComparer() override = default;

    [[nodiscard]] bool compare(const CPHScriptGameObjectAction* v) const override { return v->compare(m_object); }
    [[nodiscard]] bool compare(const CPHScriptGameObjectCondition* v) const override { return v->compare(m_object); }
};
