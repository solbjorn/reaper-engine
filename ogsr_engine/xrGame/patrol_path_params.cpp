////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_params.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path parameters class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "patrol_path_params.h"

#include "patrol_path_manager.h"
#include "ai_space.h"

CPatrolPathParams::CPatrolPathParams(LPCSTR caPatrolPathToGo, const PatrolPathManager::EPatrolStartType tPatrolPathStart,
                                     const PatrolPathManager::EPatrolRouteType tPatrolPathStop, bool bRandom, u32 index)
{
    m_path_name._set(caPatrolPathToGo);
    m_path = ai().patrol_paths().safe_path(m_path_name, true);

#ifdef CRASH_ON_INVALID_VERTEX_ID
    XR_ASSERT(m_path != nullptr, "patrol path not found", m_path_name, index);
#else
    XR_DEBUG_ASSERT(m_path != nullptr, "patrol path not found", m_path_name, index);
#endif

    m_tPatrolPathStart = tPatrolPathStart;
    m_tPatrolPathStop = tPatrolPathStop;
    m_bRandom = bRandom;
    m_previous_index = index;
}

CPatrolPathParams::~CPatrolPathParams() = default;

u32 CPatrolPathParams::count() const { return m_path->vertices().size(); }

const Fvector& CPatrolPathParams::point(u32 index) const
{
    XR_ASSERT(m_path != nullptr, "empty patrol path", m_path_name, index);
    XR_ASSERT(!m_path->vertices().empty(), "no vertices in patrol path", m_path_name, index);

    if (!m_path->vertex(index))
    {
        Msg("!![{}] Can't get information about patrol point number [{}] in the patrol way [{}]", std::source_location::current().function_name(), index,
            m_path_name);
        index = (*m_path->vertices().begin()).second->vertex_id();
    }

    return XR_ASSERT_VAL(m_path->vertex(index) != nullptr, "can't find patrol path vertex", m_path_name, index)->data().position();
}

u32 CPatrolPathParams::level_vertex_id(u32 index) const
{
    XR_ASSERT(m_path != nullptr, "empty patrol path", m_path_name, index);
    XR_ASSERT(!m_path->vertices().empty(), "no vertices in patrol path", m_path_name, index);

    if (!m_path->vertex(index))
    {
        Msg("!![{}] Can't get information about patrol point number [{}] in the patrol way [{}]", std::source_location::current().function_name(), index,
            m_path_name);
        index = (*m_path->vertices().begin()).second->vertex_id();
    }

    return XR_ASSERT_VAL(m_path->vertex(index) != nullptr, "can't find patrol path vertex", m_path_name, index)->data().level_vertex_id();
}

GameGraph::_GRAPH_ID CPatrolPathParams::game_vertex_id(u32 index) const
{
    XR_ASSERT(m_path != nullptr, "empty patrol path", m_path_name, index);
    XR_ASSERT(!m_path->vertices().empty(), "no vertices in patrol path", m_path_name, index);

    if (!m_path->vertex(index))
    {
        Msg("!![{}] Can't get information about patrol point number [{}] in the patrol way [{}]", std::source_location::current().function_name(), index,
            m_path_name);
        index = (*m_path->vertices().begin()).second->vertex_id();
    }

    return XR_ASSERT_VAL(m_path->vertex(index) != nullptr, "can't find patrol path vertex", m_path_name, index)->data().game_vertex_id();
}

u32 CPatrolPathParams::point(LPCSTR name) const
{
    auto pt = m_path->point(shared_str{name});
    return pt ? pt->vertex_id() : std::numeric_limits<u32>::max();
}

u32 CPatrolPathParams::point(const Fvector& point) const { return (m_path->point(point)->vertex_id()); }

bool CPatrolPathParams::flag(u32 index, u8 flag_index) const
{
    VERIFY(m_path->vertex(index));
    return (!!(m_path->vertex(index)->data().flags() & (u32(1) << flag_index)));
}

Flags32 CPatrolPathParams::flags(u32 index) const
{
    VERIFY(m_path->vertex(index));
    return (Flags32().assign(m_path->vertex(index)->data().flags()));
}

LPCSTR CPatrolPathParams::name(u32 index) const
{
    VERIFY(m_path->vertex(index));
    return m_path->vertex(index)->data().name().c_str();
}

bool CPatrolPathParams::terminal(u32 index) const
{
    VERIFY(m_path->vertex(index));
    return m_path->vertex(index)->edges().empty();
}
