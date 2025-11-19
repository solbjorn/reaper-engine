////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

namespace LevelGraph
{
class CVertex;
}

class CAI_ObjectLocation : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CAI_ObjectLocation);

private:
    u32 m_level_vertex_id;
    GameGraph::_GRAPH_ID m_game_vertex_id;

public:
    inline CAI_ObjectLocation();
    ~CAI_ObjectLocation() override = default;

    void init();
    inline virtual void reinit();

    void game_vertex(const GameGraph::CVertex* game_vertex);
    void game_vertex(const GameGraph::_GRAPH_ID game_vertex_id);
    [[nodiscard]] const GameGraph::CVertex* game_vertex() const;
    [[nodiscard]] inline GameGraph::_GRAPH_ID game_vertex_id() const;

    void level_vertex(const LevelGraph::CVertex* level_vertex);
    void level_vertex(const u32 level_vertex_id);
    [[nodiscard]] const LevelGraph::CVertex* level_vertex() const;
    [[nodiscard]] inline u32 level_vertex_id() const;
};

#include "ai_object_location_inline.h"
