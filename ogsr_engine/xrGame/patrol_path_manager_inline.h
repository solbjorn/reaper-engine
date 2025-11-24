////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager_inline.h
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"

IC CPatrolPathManager::CPatrolPathManager(CRestrictedObject* object, CGameObject* game_object) : m_object{object}, m_game_object{game_object}
{
    VERIFY(game_object);
    m_path_name._set("");
}

IC bool CPatrolPathManager::actual() const { return (m_actuality); }
IC bool CPatrolPathManager::failed() const { return (m_failed); }

IC const CPatrolPath* CPatrolPathManager::get_path() const { return (m_path); }

IC bool CPatrolPathManager::completed() const { return (m_completed); }
IC bool CPatrolPathManager::random() const { return (m_random); }

IC const Fvector& CPatrolPathManager::destination_position() const
{
    VERIFY(_valid(m_dest_position));
    return (m_dest_position);
}

IC void CPatrolPathManager::set_path(const CPatrolPath* path, shared_str path_name)
{
    if (m_path == path)
        return;
    m_path = path;
    // Msg("~~[CPatrolPathManager::set_path] old path [%s], new_path [%s]", m_path_name.c_str(), path_name.c_str());
    m_path_name = path_name;
    m_actuality = false;
    m_completed = false;
    reset();
}

IC void CPatrolPathManager::set_start_type(const PatrolPathManager::EPatrolStartType patrol_start_type)
{
    m_actuality = m_actuality && (m_start_type == patrol_start_type);
    m_completed = m_completed && m_actuality;
    m_start_type = patrol_start_type;
}

IC void CPatrolPathManager::set_route_type(const PatrolPathManager::EPatrolRouteType patrol_route_type)
{
    m_actuality = m_actuality && (m_route_type == patrol_route_type);
    m_completed = m_completed && m_actuality;
    m_route_type = patrol_route_type;
}

IC void CPatrolPathManager::set_random(bool random) { m_random = random; }

IC void CPatrolPathManager::make_inactual()
{
    m_actuality = false;
    m_completed = false;
}

IC void CPatrolPathManager::set_path(shared_str path_name) { set_path(ai().patrol_paths().safe_path(path_name, false, true), path_name); }

IC void CPatrolPathManager::set_path(shared_str path_name, const PatrolPathManager::EPatrolStartType patrol_start_type, const PatrolPathManager::EPatrolRouteType patrol_route_type,
                                     bool random)
{
    set_path(ai().patrol_paths().safe_path(path_name, false, true), path_name);
    set_start_type(patrol_start_type);
    set_route_type(patrol_route_type);
    set_random(random);
}

IC u32 CPatrolPathManager::get_current_point_index() const { return (m_curr_point_index); }

IC CRestrictedObject& CPatrolPathManager::object() const
{
    VERIFY(m_object);
    return (*m_object);
}

inline CExtrapolateCallback& CPatrolPathManager::extrapolate_callback() { return m_extrapolate_callback; }
inline const CExtrapolateCallback& CPatrolPathManager::extrapolate_callback() const { return m_extrapolate_callback; }
