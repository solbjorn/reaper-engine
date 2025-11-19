#pragma once

class DBG_ScriptSphere;
class DBG_ScriptBox;
class DBG_ScriptLine;

enum DebugRenderType
{
    eDBGLine = 0,
    eDBGSphere,
    eDBGBox,
    eDBGUndef
};

class DBG_ScriptObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(DBG_ScriptObject);

public:
    Fcolor m_color;
    bool m_visible{true};
    bool m_hud{false};

    DBG_ScriptObject() { m_color.set(1, 0, 0, 1); }
    ~DBG_ScriptObject() override = default;

    virtual DBG_ScriptSphere* cast_dbg_sphere() { return nullptr; }
    virtual DBG_ScriptBox* cast_dbg_box() { return nullptr; }
    virtual DBG_ScriptLine* cast_dbg_line() { return nullptr; }

    virtual void Render() {}
};

class DBG_ScriptSphere : public DBG_ScriptObject
{
    RTTI_DECLARE_TYPEINFO(DBG_ScriptSphere, DBG_ScriptObject);

public:
    Fmatrix m_mat;

    DBG_ScriptSphere() : m_mat{Fidentity} {}
    ~DBG_ScriptSphere() override = default;

    virtual DBG_ScriptSphere* cast_dbg_sphere() { return this; }

    virtual void Render();
};
XR_SOL_BASE_CLASSES(DBG_ScriptSphere);

class DBG_ScriptBox : public DBG_ScriptObject
{
    RTTI_DECLARE_TYPEINFO(DBG_ScriptBox, DBG_ScriptObject);

public:
    Fmatrix m_mat;
    Fvector m_size{1.0f, 1.0f, 1.0f};

    DBG_ScriptBox() : m_mat{Fidentity} {}
    ~DBG_ScriptBox() override = default;

    virtual DBG_ScriptBox* cast_dbg_box() { return this; }

    virtual void Render();
};
XR_SOL_BASE_CLASSES(DBG_ScriptBox);

class DBG_ScriptLine : public DBG_ScriptObject
{
    RTTI_DECLARE_TYPEINFO(DBG_ScriptLine, DBG_ScriptObject);

public:
    Fvector m_point_a{};
    Fvector m_point_b{};

    DBG_ScriptLine() = default;
    ~DBG_ScriptLine() override = default;

    virtual DBG_ScriptLine* cast_dbg_line() { return this; }

    virtual void Render();
};
XR_SOL_BASE_CLASSES(DBG_ScriptLine);
