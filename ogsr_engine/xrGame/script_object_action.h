////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"
#include "ai_monster_space.h"

class CScriptGameObject;

class CScriptObjectAction : public CScriptAbstractAction
{
    RTTI_DECLARE_TYPEINFO(CScriptObjectAction, CScriptAbstractAction);

public:
    CObject* m_tpObject{};
    MonsterSpace::EObjectAction m_tGoalType{MonsterSpace::eObjectActionIdle};
    u32 m_dwQueueSize{};
    shared_str m_caBoneName;

public:
    inline CScriptObjectAction();
    inline explicit CScriptObjectAction(CScriptGameObject* tpLuaGameObject, MonsterSpace::EObjectAction tObjectActionType, u32 dwQueueSize = u32(-1));
    inline explicit CScriptObjectAction(LPCSTR caBoneName, MonsterSpace::EObjectAction tObjectActionType);
    inline explicit CScriptObjectAction(MonsterSpace::EObjectAction tObjectActionType);
    ~CScriptObjectAction() override;

    void SetObject(CScriptGameObject* tpLuaGameObject);
    IC void SetObject(LPCSTR caBoneName);
    IC void SetObjectAction(MonsterSpace::EObjectAction tObjectActionType);
    IC void SetQueueSize(u32 dwQueueSize);
    IC void initialize();

    inline void clone(const CScriptObjectAction& from);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptObjectAction);
#undef script_type_list
#define script_type_list save_type_list(CScriptObjectAction)

#include "script_object_action_inline.h"
