////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object.cpp
//	Created 	: 06.10.2003
//  Modified 	: 14.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_object.h"

CScriptObject::CScriptObject() = default;
CScriptObject::~CScriptObject() = default;

DLL_Pure* CScriptObject::_construct()
{
    std::ignore = CGameObject::_construct();
    CScriptEntity::_construct();

    return this;
}

void CScriptObject::reinit()
{
    CScriptEntity::reinit();
    CGameObject::reinit();
}

tmc::task<bool> CScriptObject::net_Spawn(CSE_Abstract* DC) { co_return co_await CGameObject::net_Spawn(DC) && co_await CScriptEntity::net_Spawn(DC); }

tmc::task<void> CScriptObject::net_Destroy()
{
    co_await CGameObject::net_Destroy();
    co_await CScriptEntity::net_Destroy();
}

BOOL CScriptObject::UsedAI_Locations() { return (FALSE); }

tmc::task<void> CScriptObject::shedule_Update(u32 DT)
{
    co_await CGameObject::shedule_Update(DT);
    co_await CScriptEntity::shedule_Update(DT);
}

tmc::task<void> CScriptObject::UpdateCL()
{
    co_await CGameObject::UpdateCL();
    co_await CScriptEntity::UpdateCL();
}
