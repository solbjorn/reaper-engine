////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_script.h
//	Created 	: 07.07.2004
//  Modified 	: 07.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action.h"
#include "script_game_object.h"

template <typename _object_type>
class CActionPlannerActionScript : public CScriptActionPlannerAction
{
    RTTI_DECLARE_TYPEINFO(CActionPlannerActionScript<_object_type>, CScriptActionPlannerAction);

protected:
    typedef CScriptActionPlannerAction inherited;

public:
    _object_type* m_object;

public:
    inline explicit CActionPlannerActionScript(const xr_vector<COperatorCondition>& conditions, const xr_vector<COperatorCondition>& effects, _object_type* object = nullptr,
                                               LPCSTR action_name = "");
    inline explicit CActionPlannerActionScript(_object_type* object = nullptr, LPCSTR action_name = "");
    ~CActionPlannerActionScript() override = default;

    virtual void setup(_object_type* object, CPropertyStorage*);
    virtual void setup(CScriptGameObject* object, CPropertyStorage* storage);
    IC _object_type& object() const;
};

#include "action_planner_action_script_inline.h"
