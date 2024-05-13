#include "stdafx.h"

#include "level_graph.h"

#include "ai_space.h"

namespace
{
[[nodiscard]] const CLevelGraph* get_level_graph() { return &ai().level_graph(); }

[[nodiscard]] std::unique_ptr<CLevelGraph> get_level_graph_for_level(gsl::czstring level)
{
    string_path fn, fName;
    strconcat(sizeof(fn), fn, level, "\\", LEVEL_GRAPH_NAME.data());
    std::ignore = FS.update_path(fName, "$game_levels$", fn);

    return std::make_unique<CLevelGraph>(fName);
}

[[nodiscard]] u32 vertex_count(const CLevelGraph* self) { return self->header().vertex_count(); }
} // namespace

void CLevelGraph::script_register(sol::state_view& lua)
{
    lua.set("level_graph", sol::overload(&get_level_graph_for_level, &get_level_graph));

    lua.new_usertype<CLevelGraph>(
        "CLevelGraph", sol::no_constructor, "vertex_position", sol::resolve<const Fvector(u32) const>(&CLevelGraph::vertex_position), "vertex_count", &vertex_count, "vertex_id",
        sol::overload(sol::resolve<u32(u32, const Fvector&) const>(&CLevelGraph::vertex_id), sol::resolve<u32(const Fvector&) const>(&CLevelGraph::vertex_id)), "nearest_vertex_id",
        &CLevelGraph::nearest_vertex_id, "valid_vertex_id", &CLevelGraph::valid_vertex_id, "is_accessible_vertex_id", &CLevelGraph::is_accessible);
}
