////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point
////////////////////////////////////////////////////////////////////////////

#pragma once

class CPatrolPath;
class CLevelGraph;
class CGameLevelCrossTable;
class CGameGraph;

#include "object_interfaces.h"
#include "game_graph_space.h"

class CPatrolPoint : public IPureSerializeObject<IReader, IWriter>
{
    RTTI_DECLARE_TYPEINFO(CPatrolPoint, IPureSerializeObject<IReader, IWriter>);

public:
    friend class CPatrolPathStorage;
    friend class CPatrolPointScript;

protected:
    shared_str m_name;
    Fvector m_position;
    u32 m_flags = 0x0;
    u32 m_level_vertex_id;
    GameGraph::_GRAPH_ID m_game_vertex_id;

protected:
#ifdef DEBUG
    bool m_initialized;
    const CPatrolPath* m_path;
#endif

private:
    IC void correct_position(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph);
#ifdef DEBUG
    void verify_vertex_id(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph) const;
#endif

public:
    CPatrolPoint(const CPatrolPath* path, const Fvector& position, u32 level_vertex_id, u32 flags, shared_str name);
    CPatrolPoint(const CPatrolPath* path = 0);
    virtual void load(IReader& stream);
    virtual void save(IWriter& stream);
    CPatrolPoint& load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream);
    CPatrolPoint& load_ini(CInifile::Sect& section, LPSTR prefix);
    IC const Fvector& position() const;
    IC const u32& level_vertex_id(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph) const;
    IC const GameGraph::_GRAPH_ID& game_vertex_id(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph) const;
    IC const u32& flags() const;
    IC const shared_str& name() const;
    CPatrolPoint& position(Fvector position);

public:
#ifdef XRGAME_EXPORTS
    const u32& level_vertex_id() const;
    const GameGraph::_GRAPH_ID& game_vertex_id() const;
#endif

#ifdef DEBUG
public:
    IC void path(const CPatrolPath* path);
#endif
};

#include "patrol_point_inline.h"
