////////////////////////////////////////////////////////////////////////////
//	Module 		: script_value_container.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script value container
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptValue : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptValue);

public:
    ~CScriptValue() override = default;
};

class CScriptValueContainer : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptValueContainer);

protected:
    xr_vector<CScriptValue*> m_values;

public:
    ~CScriptValueContainer() override;

    IC void assign();
    IC void clear();
    IC void add(CScriptValue*);
};
