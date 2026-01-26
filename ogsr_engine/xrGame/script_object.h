////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object.h
//	Created 	: 06.10.2003
//  Modified 	: 14.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "script_entity.h"

class CScriptObject : public CGameObject, public CScriptEntity
{
    RTTI_DECLARE_TYPEINFO(CScriptObject, CGameObject, CScriptEntity);

public:
    CScriptObject();
    ~CScriptObject() override;

    virtual DLL_Pure* _construct();
    virtual void reinit();
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    virtual BOOL UsedAI_Locations();
    tmc::task<void> shedule_Update(u32 DT) override;
    tmc::task<void> UpdateCL() override;
    virtual CScriptEntity* cast_script_entity() { return this; }
};
