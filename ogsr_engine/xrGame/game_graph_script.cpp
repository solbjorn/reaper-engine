////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "game_graph.h"
#include "ai_space.h"

static const CGameGraph* get_game_graph() { return (&ai().game_graph()); }

static const CGameGraph::CHeader* get_header(const CGameGraph* self) { return (&self->header()); }

static bool get_accessible1(const CGameGraph* self, const u32& vertex_id) { return (self->accessible(vertex_id)); }

static void get_accessible2(const CGameGraph* self, const u32& vertex_id, bool value) { self->accessible(vertex_id, value); }

static u32 vertex_count(const CGameGraph* self) { return self->header().vertex_count(); }

static Fvector CVertex__level_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->level_point());
}

static Fvector CVertex__game_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->game_point());
}

static const CGameLevelCrossTable* get_cross_table() { return &ai().cross_table(); }

static Fvector4 CVertex__mask_(const CGameGraph::CVertex* vertex)
{
    const u8* mask = vertex->vertex_type();
    return Fvector4{(float)mask[0], (float)mask[1], (float)mask[2], (float)mask[3]};
}

void CGameGraph::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.set_function("game_graph", &get_game_graph);

    lua.new_usertype<CGameGraph>("CGameGraph", sol::no_constructor, "accessible", sol::overload(&get_accessible1, &get_accessible2), "valid_vertex_id",
                                 &CGameGraph::valid_vertex_id, "vertex", &CGameGraph::vertex, "vertex_id", &CGameGraph::vertex_id, "vertex_count", &vertex_count);

    lua.new_usertype<CVertex>("GameGraph__CVertex", sol::no_constructor, "level_point", &CVertex__level_point, "game_point", &CVertex__game_point, "level_id", &CVertex::level_id,
                              "level_vertex_id", &CVertex::level_vertex_id, "mask", &CVertex__mask_);

    lua.set_function("cross_table", &get_cross_table);

    lua.new_usertype<CGameLevelCrossTable>("CGameLevelCrossTable", sol::no_constructor, "vertex", &CGameLevelCrossTable::vertex);
    lua.new_usertype<CGameLevelCrossTable::CCell>("CGameLevelCrossTable__CCell", sol::no_constructor, "game_vertex_id", &CGameLevelCrossTable::CCell::game_vertex_id);
}
