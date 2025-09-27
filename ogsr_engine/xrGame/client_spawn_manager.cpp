////////////////////////////////////////////////////////////////////////////
//	Module 		: client_spawn_manager.cpp
//	Created 	: 08.10.2004
//  Modified 	: 08.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Seniority hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "client_spawn_manager.h"
#include "ai_space.h"
#include "script_engine.h"
#include "level.h"
#include "gameobject.h"
#include "script_game_object.h"

CClientSpawnManager::~CClientSpawnManager() { VERIFY(m_registry.empty()); }

void CClientSpawnManager::add(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, sol::function function, sol::object object)
{
    CSpawnCallback callback;
    callback.m_callback = function;
    callback.m_object = object;

    add(requesting_id, requested_id, callback);
}

void CClientSpawnManager::add(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, sol::function function)
{
    CSpawnCallback callback;
    callback.m_callback = function;

    add(requesting_id, requested_id, callback);
}

void CClientSpawnManager::add(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const CALLBACK_TYPE& object_callback)
{
    CSpawnCallback callback;
    callback.m_object_callback = object_callback;

    add(requesting_id, requested_id, callback);
}

void CClientSpawnManager::add(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, CSpawnCallback& spawn_callback)
{
    CObject* object = Level().Objects.net_Find(requesting_id);
    if (object)
    {
        callback(spawn_callback, object);
        return;
    }

    REQUEST_REGISTRY::iterator I = m_registry.find(requesting_id);
    if (I == m_registry.end())
    {
        I = m_registry.try_emplace(requesting_id).first;
        I->second.try_emplace(requested_id, spawn_callback);

        return;
    }

    REQUESTED_REGISTRY::iterator J = (*I).second.find(requested_id);
    if (J == (*I).second.end())
    {
        I->second.try_emplace(requested_id, spawn_callback);
        return;
    }

    merge_spawn_callbacks(spawn_callback, (*J).second);
}

void CClientSpawnManager::remove(REQUESTED_REGISTRY& registry, ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, bool no_warning)
{
    REQUESTED_REGISTRY::iterator I = registry.find(requested_id);
    if (I == registry.end())
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "There is no spawn callback on object with id %d from object with id %d!", requesting_id, requested_id);
        return;
    }

    registry.erase(I);
}

void CClientSpawnManager::remove(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id)
{
    REQUEST_REGISTRY::iterator I = m_registry.find(requesting_id);
    if (I == m_registry.end())
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "There is no spawn callback on object with id %d from object with id %d!", requesting_id, requested_id);
        return;
    }

    remove((*I).second, requesting_id, requested_id);
    if (!(*I).second.empty())
        return;

    m_registry.erase(I);
}

void CClientSpawnManager::clear(ALife::_OBJECT_ID requested_id)
{
    for (auto I = m_registry.begin(); I != m_registry.end();)
    {
        remove((*I).second, (*I).first, requested_id, true);
        if (!(*I).second.empty())
        {
            ++I;
            continue;
        }

        I = m_registry.erase(I);
    }
}

void CClientSpawnManager::callback(CSpawnCallback& spawn_callback, CObject* object)
{
    spawn_callback.m_object_callback(object);

    if (!spawn_callback.m_callback)
        return;

    CGameObject* game_object = smart_cast<CGameObject*>(object);
    CScriptGameObject* script_game_object = game_object ? game_object->lua_game_object() : nullptr;

    if (spawn_callback.m_object)
        spawn_callback.m_callback(object->ID(), script_game_object, spawn_callback.m_object);
    else
        spawn_callback.m_callback(object->ID(), script_game_object);
}

void CClientSpawnManager::callback(CObject* object)
{
    REQUEST_REGISTRY::iterator I = m_registry.find(object->ID());
    if (I == m_registry.end())
        return;

    for (auto& item : I->second)
        callback(item.second, object);

    I->second.clear();
    m_registry.erase(I);
}

void CClientSpawnManager::merge_spawn_callbacks(CSpawnCallback& new_callback, CSpawnCallback& old_callback)
{
    old_callback.m_object_callback = new_callback.m_object_callback;

    old_callback.m_callback = new_callback.m_callback;
    old_callback.m_object = new_callback.m_object;
}

const CClientSpawnManager::CSpawnCallback* CClientSpawnManager::callback(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id) const
{
    REQUEST_REGISTRY::const_iterator I = m_registry.find(requested_id);
    if (I == m_registry.end())
        return nullptr;

    REQUESTED_REGISTRY::const_iterator i = (*I).second.find(requesting_id);
    if (i == (*I).second.end())
        return nullptr;

    return (&(*i).second);
}

#ifdef DEBUG
void CClientSpawnManager::dump() const
{
    if (m_registry.empty())
        return;

    Msg("dumping client spawn manager(%d objects being waited):", m_registry.size());

    for (auto& req : m_registry)
    {
        Msg("[%d], i.e. object with id %d left with hanging callbacks on it", req.first, req.first);

        for (auto& item : req.second)
            Msg("[%d][%d], i.e. object with id %d waits for object with id %d", req.first, item.first, item.first, req.first);
    }
}

void CClientSpawnManager::dump(ALife::_OBJECT_ID requesting_id) const
{
    for (auto& req : m_registry)
    {
        if (req.first == requesting_id)
            Msg("! CClientSpawnManager::dump[hanging id %d]", requesting_id);

        for (auto& item : req.second)
            Msg("! CClientSpawnManager::dump[id %d waits for %d]", requesting_id, item.first);
    }
}
#endif // DEBUG

void CClientSpawnManager::clear() { m_registry.clear(); }
