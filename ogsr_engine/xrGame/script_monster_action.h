////////////////////////////////////////////////////////////////////////////
//	Module 		: script_monster_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script monster action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"
#include "ai_monster_space.h"

class CScriptGameObject;

class CScriptMonsterAction : public CScriptAbstractAction
{
    RTTI_DECLARE_TYPEINFO(CScriptMonsterAction, CScriptAbstractAction);

public:
    MonsterSpace::EScriptMonsterGlobalAction m_tAction;
    CObject* m_tObject{};

public:
    inline CScriptMonsterAction();
    inline explicit CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action);
    inline explicit CScriptMonsterAction(MonsterSpace::EScriptMonsterGlobalAction action, CScriptGameObject* tObj);
    ~CScriptMonsterAction() override;

    void SetObject(CScriptGameObject* tObj);

    inline void clone(const CScriptMonsterAction& from);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptMonsterAction);
#undef script_type_list
#define script_type_list save_type_list(CScriptMonsterAction)

#include "script_monster_action_inline.h"
