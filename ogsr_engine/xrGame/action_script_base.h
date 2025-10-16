////////////////////////////////////////////////////////////////////////////
//	Module 		: action_script_base.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Base action with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"
#include "script_game_object.h"

template <typename _object_type>
class CActionScriptBase : public CScriptActionBase
{
    RTTI_DECLARE_TYPEINFO(CActionScriptBase<_object_type>, CScriptActionBase);

protected:
    typedef CScriptActionBase inherited;

public:
    _object_type* m_object;

    IC CActionScriptBase(const xr_vector<COperatorCondition>& conditions, const xr_vector<COperatorCondition>& effects, _object_type* object = nullptr, LPCSTR action_name = "");
    IC CActionScriptBase(_object_type* object = nullptr, LPCSTR action_name = "");
    virtual ~CActionScriptBase();

    virtual void setup(_object_type* object, CPropertyStorage*);
    virtual void setup(CScriptGameObject* object, CPropertyStorage* storage);
};

#include "action_script_base_inline.h"
