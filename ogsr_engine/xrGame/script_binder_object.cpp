////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../COMMON_AI/xrServer_Objects_ALife.h"
#include "script_binder_object.h"
#include "script_game_object.h"

CScriptBinderObject::CScriptBinderObject(CScriptGameObject* object) { m_object = object; }

CScriptBinderObject::~CScriptBinderObject()
{
#ifdef DEBUG
    if (m_object)
        Msg("Destroying binded object %s", m_object->Name());
#endif
}

void CScriptBinderObject::reinit()
{
    auto op = ops.find(binder_ops::REINIT);
    if (op == ops.end())
        return;

    op->second(this);
}

void CScriptBinderObject::reload(LPCSTR section)
{
    auto op = ops.find(binder_ops::RELOAD);
    if (op == ops.end())
        return;

    op->second(this, section);
}

bool CScriptBinderObject::net_Spawn(SpawnType DC)
{
    auto op = ops.find(binder_ops::NET_SPAWN);
    if (op == ops.end())
        return true;

    return op->second(this, DC);
}

void CScriptBinderObject::net_Destroy()
{
    auto op = ops.find(binder_ops::NET_DESTROY);
    if (op == ops.end())
        return;

    op->second(this);
}

void CScriptBinderObject::shedule_Update(u32 time_delta)
{
    auto op = ops.find(binder_ops::UPDATE);
    if (op == ops.end())
        return;

    op->second(this, time_delta);
}

void CScriptBinderObject::save(NET_Packet* output_packet)
{
    auto op = ops.find(binder_ops::SAVE);
    if (op == ops.end())
        return;

    op->second(this, output_packet);
}

void CScriptBinderObject::load(IReader* input_packet)
{
    auto op = ops.find(binder_ops::LOAD);
    if (op == ops.end())
        return;

    op->second(this, input_packet);
}

bool CScriptBinderObject::net_SaveRelevant()
{
    auto op = ops.find(binder_ops::NET_SAVE_RELEVANT);
    if (op == ops.end())
        return false;

    return op->second(this);
}

void CScriptBinderObject::net_Relcase(CScriptGameObject* object)
{
    auto op = ops.find(binder_ops::NET_RELCASE);
    if (op == ops.end())
        return;

    op->second(this, object);
}
