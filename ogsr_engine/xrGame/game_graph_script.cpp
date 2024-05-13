////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "game_graph.h"

#include "ai_space.h"

namespace
{
[[nodiscard]] const CGameGraph* get_game_graph() { return &ai().game_graph(); }

[[nodiscard]] bool get_accessible(const CGameGraph* self, u32 vertex_id) { return self->accessible(vertex_id); }
void set_accessible(const CGameGraph* self, u32 vertex_id, bool value) { self->accessible(vertex_id, value); }

[[nodiscard]] u32 vertex_count(const CGameGraph* self) { return self->header().vertex_count(); }

[[nodiscard]] Fvector CVertex__level_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return vertex->level_point();
}

[[nodiscard]] Fvector CVertex__game_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return vertex->game_point();
}

[[nodiscard]] const CGameLevelCrossTable* get_cross_table() { return &ai().cross_table(); }

[[nodiscard]] std::unique_ptr<CGameLevelCrossTable> get_cross_table_for_level(gsl::czstring level_name)
{
    const GameGraph::SLevel* level = ai().game_graph().header().level(level_name, true);
    if (level == nullptr)
    {
        Msg("! Unknown level %s!", level_name);
        return std::unique_ptr<CGameLevelCrossTable>{};
    }

    CGameLevelCrossTable* result = ai().game_graph().find_cross_table_for_level(level->id());
    if (result != nullptr)
        return absl::WrapUnique(result);

    string_path fn, fName;
    strconcat(sizeof(fn), fn, level_name, "\\", CROSS_TABLE_NAME);
    std::ignore = FS.update_path(fName, "$game_levels$", fn);

    return std::make_unique<CGameLevelCrossTable>(fName);
}

[[nodiscard]] Fvector4 CVertex__mask_(const CGameGraph::CVertex* vertex)
{
    const auto mask = std::span{vertex->vertex_type(), 4};

    return Fvector4{gsl::narrow_cast<float>(mask[0]), gsl::narrow_cast<float>(mask[1]), gsl::narrow_cast<float>(mask[2]), gsl::narrow_cast<float>(mask[3])};
}
} // namespace

void CGameGraph::script_register(sol::state_view& lua)
{
    lua.set("game_graph", &get_game_graph);

    lua.new_usertype<CGameGraph>("CGameGraph", sol::no_constructor, "accessible", sol::overload(&get_accessible, &set_accessible), "valid_vertex_id", &CGameGraph::valid_vertex_id,
                                 "vertex", &CGameGraph::vertex, "vertex_id", &CGameGraph::vertex_id, "vertex_count", &vertex_count);

    lua.new_usertype<CVertex>("GameGraph__CVertex", sol::no_constructor, "level_point", &CVertex__level_point, "game_point", &CVertex__game_point, "level_id", &CVertex::level_id,
                              "level_vertex_id", &CVertex::level_vertex_id, "mask", &CVertex__mask_);

    lua.set("cross_table", sol::overload(&get_cross_table_for_level, &get_cross_table));

    lua.new_usertype<CGameLevelCrossTable>("CGameLevelCrossTable", sol::no_constructor, "vertex", &CGameLevelCrossTable::vertex);
    lua.new_usertype<CGameLevelCrossTable::CCell>("CGameLevelCrossTable__CCell", sol::no_constructor, "game_vertex_id", &CGameLevelCrossTable::CCell::game_vertex_id);
}
