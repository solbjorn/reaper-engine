////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager_inline.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC u32 CEnemyManager::last_enemy_time() const { return (m_last_enemy_time); }

IC const CEntityAlive* CEnemyManager::last_enemy() const { return (m_last_enemy); }

inline void CEnemyManager::set_useful_callback(sol::function& function, sol::object& object)
{
    m_useful_callback = function;
    m_useful_object = object;
}

IC void CEnemyManager::enable_enemy_change(const bool& value) { m_enable_enemy_change = value; }

IC bool CEnemyManager::enable_enemy_change() const { return (m_enable_enemy_change); }
