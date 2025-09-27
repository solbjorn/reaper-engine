////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_space_script.cpp
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory space script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "memory_space.h"
#include "script_game_object.h"
#include "gameobject.h"
#include "entity_alive.h"
#include "danger_object.h"

namespace
{
CScriptGameObject* not_yet_visible_object(const MemorySpace::CNotYetVisibleObject& object)
{
    return (object.m_object && object.m_object->m_spawned ? object.m_object->lua_game_object() : nullptr);
}

int get_sound_type(const CSoundObject& sound_object) { return ((int)sound_object.m_sound_type); }

template <typename T>
CScriptGameObject* get_memory_object(const MemorySpace::CMemoryObject<T>& memory_object)
{
    return (memory_object.m_object && memory_object.m_object->m_spawned ? memory_object.m_object->lua_game_object() : nullptr);
}

CScriptGameObject* CDangerObject_object(const CDangerObject* self)
{
    VERIFY(self);
    return (self->object() && self->object()->m_spawned ? self->object()->lua_game_object() : nullptr);
}

CScriptGameObject* CDangerObject_dependent_object(const CDangerObject* self)
{
    VERIFY(self);
    if (!self->dependent_object())
        return nullptr;

    const CGameObject* game_object = smart_cast<const CGameObject*>(self->dependent_object());
    return (game_object && game_object->m_spawned ? game_object->lua_game_object() : nullptr);
}

Fvector CDangerObject__position(const CDangerObject* self)
{
    THROW(self);
    return (self->position());
}
} // namespace

void CMemoryInfo::script_register(sol::state_view& lua)
{
#ifdef USE_ORIENTATION
    lua.new_usertype<SRotation>("rotation", sol::no_constructor, "yaw", sol::readonly(&SRotation::yaw), "pitch", sol::readonly(&SRotation::pitch));
#endif

    lua.new_usertype<MemorySpace::SObjectParams>("object_params", sol::no_constructor,
#ifdef USE_ORIENTATION
                                                 "orientation", sol::readonly(&MemorySpace::SObjectParams::m_orientation),
#endif
                                                 "level_vertex", sol::readonly(&MemorySpace::SObjectParams::m_level_vertex_id), "position",
                                                 sol::readonly(&MemorySpace::SObjectParams::m_position));

    lua.new_usertype<MemorySpace::SMemoryObject>("memory_object", sol::no_constructor
#ifdef USE_GAME_TIME
                                                 ,
                                                 "game_time", sol::readonly(&MemorySpace::SMemoryObject::m_game_time)
#endif
#ifdef USE_LEVEL_TIME
                                                                  ,
                                                 "level_time", sol::readonly(&MemorySpace::SMemoryObject::m_level_time)
#endif
#ifdef USE_LAST_GAME_TIME
                                                                   ,
                                                 "last_game_time", sol::readonly(&MemorySpace::SMemoryObject::m_last_game_time)
#endif
#ifdef USE_LAST_LEVEL_TIME
                                                                       ,
                                                 "last_level_time", sol::readonly(&MemorySpace::SMemoryObject::m_last_level_time)
#endif
#ifdef USE_FIRST_GAME_TIME
                                                                        ,
                                                 "first_game_time", sol::readonly(&MemorySpace::SMemoryObject::m_first_game_time)
#endif
#ifdef USE_LEVEL_TIME // USE_FIRST_LEVEL_TIME
                                                                        ,
                                                 "first_level_time", sol::readonly(&MemorySpace::SMemoryObject::m_first_level_time)
#endif
#ifdef USE_UPDATE_COUNT
                                                                         ,
                                                 "update_count", sol::readonly(&MemorySpace::SMemoryObject::m_update_count)
#endif
    );

    lua.new_usertype<MemorySpace::CMemoryObject<CEntityAlive>>("entity_memory_object", sol::no_constructor, "object_info",
                                                               sol::readonly(&MemorySpace::CMemoryObject<CEntityAlive>::m_object_params), "self_info",
                                                               sol::readonly(&MemorySpace::CMemoryObject<CEntityAlive>::m_self_params), "object", &get_memory_object<CEntityAlive>,
                                                               sol::base_classes, xr_sol_bases<MemorySpace::CMemoryObject<CEntityAlive>>());

    lua.new_usertype<MemorySpace::CMemoryObject<CGameObject>>("game_memory_object", sol::no_constructor, "object_info",
                                                              sol::readonly(&MemorySpace::CMemoryObject<CGameObject>::m_object_params), "self_info",
                                                              sol::readonly(&MemorySpace::CMemoryObject<CGameObject>::m_self_params), "object", &get_memory_object<CGameObject>,
                                                              sol::base_classes, xr_sol_bases<MemorySpace::CMemoryObject<CGameObject>>());

    lua.new_usertype<MemorySpace::CHitObject>("hit_memory_object", sol::no_constructor, "direction", sol::readonly(&MemorySpace::CHitObject::m_direction), "bone_index",
                                              sol::readonly(&MemorySpace::CHitObject::m_bone_index), "amount", sol::readonly(&MemorySpace::CHitObject::m_amount), sol::base_classes,
                                              xr_sol_bases<MemorySpace::CHitObject>());

    lua.new_usertype<MemorySpace::CVisibleObject>("visible_memory_object", sol::no_constructor, sol::base_classes, xr_sol_bases<MemorySpace::CVisibleObject>());

    lua.new_usertype<MemorySpace::CMemoryInfo>("memory_info", sol::no_constructor, "visual_info", sol::readonly(&MemorySpace::CMemoryInfo::m_visual_info), "sound_info",
                                               sol::readonly(&MemorySpace::CMemoryInfo::m_sound_info), "hit_info", sol::readonly(&MemorySpace::CMemoryInfo::m_hit_info),
                                               sol::base_classes, xr_sol_bases<MemorySpace::CMemoryInfo>());

    lua.new_usertype<MemorySpace::CSoundObject>("sound_memory_object", sol::no_constructor, "type", &MemorySpace::CSoundObject::sound_type, "power",
                                                sol::readonly(&MemorySpace::CSoundObject::m_power), sol::base_classes, xr_sol_bases<MemorySpace::CSoundObject>());

    lua.new_usertype<MemorySpace::CNotYetVisibleObject>("not_yet_visible_object", sol::no_constructor, "value", sol::readonly(&MemorySpace::CNotYetVisibleObject::m_value),
                                                        "object", &not_yet_visible_object);

    lua.new_usertype<CDangerObject>("danger_object", sol::no_constructor,
                                    // danger_type
                                    "bullet_ricochet", sol::var(CDangerObject::eDangerTypeBulletRicochet), "attack_sound", sol::var(CDangerObject::eDangerTypeAttackSound),
                                    "entity_attacked", sol::var(CDangerObject::eDangerTypeEntityAttacked), "entity_death", sol::var(CDangerObject::eDangerTypeEntityDeath),
                                    "entity_corpse", sol::var(CDangerObject::eDangerTypeFreshEntityCorpse), "attacked", sol::var(CDangerObject::eDangerTypeAttacked), "grenade",
                                    sol::var(CDangerObject::eDangerTypeGrenade), "enemy_sound", sol::var(CDangerObject::eDangerTypeEnemySound),

                                    // danger_perceive_type
                                    "visual", sol::var(CDangerObject::eDangerPerceiveTypeVisual), "sound", sol::var(CDangerObject::eDangerPerceiveTypeSound), "hit",
                                    sol::var(CDangerObject::eDangerPerceiveTypeHit),

                                    "position", &CDangerObject__position, "time", &CDangerObject::time, "type", &CDangerObject::type, "perceive_type",
                                    &CDangerObject::perceive_type, "object", &CDangerObject_object, "dependent_object", &CDangerObject_dependent_object);
}
