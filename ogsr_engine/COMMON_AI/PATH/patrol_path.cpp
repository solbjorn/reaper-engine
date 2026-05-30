////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "patrol_path.h"

#include "levelgamedef.h"

CPatrolPath::CPatrolPath([[maybe_unused]] shared_str name)
{
#ifdef DEBUG
    m_name = name;
#endif
}

CPatrolPath::~CPatrolPath() = default;

CPatrolPath& CPatrolPath::load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream)
{
    R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_POINTS));
    u32 vertex_count = stream.r_u16();
    for (u32 i = 0; i < vertex_count; ++i)
        add_vertex(CPatrolPoint{this}.load_raw(level_graph, cross, game_graph, stream), i);

    R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_LINKS));
    u32 edge_count = stream.r_u16();
    for (u32 i = 0; i < edge_count; ++i)
    {
        u16 vertex0 = stream.r_u16();
        u16 vertex1 = stream.r_u16();
        float probability = stream.r_float();
        add_edge(vertex0, vertex1, probability);
    }

    return (*this);
}

CPatrolPath& CPatrolPath::load_ini(CInifile::Sect& section)
{
    const char* points = section.r_string("points");
    const int vertex_count = _GetItemCount(points);
    string16 prefix;

    for (int i = 0; i < vertex_count; ++i)
        add_vertex(CPatrolPoint{this}.load_ini(section, _GetItem(points, i, prefix)), i);

    for (int i = 0; i < vertex_count; ++i)
    {
        if (const auto sect = xr::format("{}:links", _GetItem(points, i, prefix)); section.line_exist(sect.c_str()))
        {
            const char* links = section.r_string(sect.c_str());
            const int links_count = _GetItemCount(links);

            for (int k = 0; k < links_count; ++k)
            {
                string32 link;

                const auto res = scn::scan<u32, f32>(std::string_view{_GetItem(links, k, link)}, "p{}({})");
                R_ASSERT(res, res.error().msg());

                const auto [vertex_idx, probability] = res->values();
                add_edge(i, vertex_idx, probability);
            }
        }
    }

    return *this;
}

#ifdef DEBUG
void CPatrolPath::load(IReader& stream)
{
    inherited::load(stream);
    vertex_iterator I = vertices().begin();
    vertex_iterator E = vertices().end();
    for (; I != E; ++I)
        (*I).second->data().path(this);
}
#endif

CPatrolPoint CPatrolPath::add_point(Fvector pos)
{
    auto pp = CPatrolPoint(this).position(pos);
    u32 index = vertices().size();
    add_vertex(pp, index);
    return vertex(index)->data();
}

CPatrolPoint CPatrolPath::point(u32 index) { return vertex(index)->data(); }

CPatrolPoint* CPatrolPath::point_raw(u32 index) { return &vertex(index)->data(); }
