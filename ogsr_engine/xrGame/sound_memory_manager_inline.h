////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"

IC CSoundMemoryManager::CSoundMemoryManager(CCustomMonster* object, CAI_Stalker* stalker, CSound_UserDataVisitor* visitor)
{
    VERIFY(object);
    m_object = object;
    VERIFY(visitor);
    m_visitor = visitor;
    m_stalker = stalker;
    m_max_sound_count = 0;
#ifdef USE_SELECTED_SOUND
    m_selected_sound = 0;
#endif
}

IC const CSoundMemoryManager::SOUNDS& CSoundMemoryManager::objects() const
{
    m_sounds->erase(std::remove_if(m_sounds->begin(), m_sounds->end(), [](const auto& obj) { return obj.m_object->getDestroy(); }), m_sounds->end());
    return *m_sounds;
}

IC void CSoundMemoryManager::priority(const ESoundTypes& sound_type, u32 priority)
{
    VERIFY(m_priorities.end() == m_priorities.find(sound_type));
    m_priorities.insert(std::make_pair(sound_type, priority));
}

#ifdef USE_SELECTED_SOUND
IC const MemorySpace::CSoundObject* CSoundMemoryManager::sound() const { return (m_selected_sound); }
#endif

IC void CSoundMemoryManager::set_squad_objects(SOUNDS* squad_objects) { m_sounds = squad_objects; }

IC void CSoundMemoryManager::set_threshold(float threshold)
{
    m_sound_threshold = threshold;
    VERIFY(_valid(m_sound_threshold));
}

IC void CSoundMemoryManager::restore_threshold()
{
    m_sound_threshold = m_min_sound_threshold;
    VERIFY(_valid(m_sound_threshold));
}
