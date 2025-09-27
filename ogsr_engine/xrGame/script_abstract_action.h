////////////////////////////////////////////////////////////////////////////
//	Module 		: script_abstract_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script abstract action
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptAbstractAction : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptAbstractAction);

public:
    bool m_bCompleted;

public:
    IC CScriptAbstractAction();
    virtual ~CScriptAbstractAction();
    virtual bool completed();

    inline void clone(const CScriptAbstractAction& from);
};

#include "script_abstract_action_inline.h"
