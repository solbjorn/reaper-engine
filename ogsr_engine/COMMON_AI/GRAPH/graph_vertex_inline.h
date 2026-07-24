////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_base_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex base class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _data_type, typename _vertex_id_type, typename _graph_type>

#define CSGraphVertex CVertex<_data_type, _vertex_id_type, _graph_type>

TEMPLATE_SPECIALIZATION
IC CSGraphVertex::CVertex(const _data_type& data, const _vertex_id_type& vertex_id, size_t* edge_count)
{
    m_data = data;
    m_vertex_id = vertex_id;
    m_edge_count = XR_ASSERT_VAL(edge_count != nullptr);
}

TEMPLATE_SPECIALIZATION
IC CSGraphVertex::~CVertex()
{
    while (!edges().empty())
        remove_edge(edges().back().vertex_id());

    while (!m_vertices.empty())
        m_vertices.back()->remove_edge(vertex_id());

    delete_data(m_data);
}

TEMPLATE_SPECIALIZATION
IC const typename _graph_type::CEdge* CSGraphVertex::edge(const _vertex_id_type& vertex_id) const
{
    if (const auto I = std::ranges::find(edges(), vertex_id); I != m_edges.end())
        return std::to_address(I);

    return 0;
}

TEMPLATE_SPECIALIZATION
IC typename _graph_type::CEdge* CSGraphVertex::edge(const _vertex_id_type& vertex_id)
{
    if (const auto I = std::ranges::find(m_edges, vertex_id); I != m_edges.end())
        return std::to_address(I);

    return 0;
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::add_edge(CVertex* vertex, const typename _graph_type::CEdge::edge_weight_type& edge_weight)
{
    XR_DEBUG_ASSERT(std::ranges::find(m_edges, vertex->vertex_id()) == m_edges.end());

    vertex->on_edge_addition(this);
    m_edges.emplace_back(edge_weight, vertex);

    ++*m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::remove_edge(const _vertex_id_type& vertex_id)
{
    const auto I = XR_ASSERT_VAL(std::ranges::find(m_edges, vertex_id, &_graph_type::CEdge::vertex_id) != m_edges.end());

    I->vertex()->on_edge_removal(this);
    m_edges.erase(I);

    --*m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::on_edge_addition(CVertex* vertex)
{
    XR_DEBUG_ASSERT(std::ranges::find(m_vertices, vertex) == m_vertices.end());
    m_vertices.push_back(vertex);
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::on_edge_removal(const CVertex* vertex) { m_vertices.erase(XR_ASSERT_VAL(std::ranges::find(m_vertices, vertex) != m_vertices.end())); }

TEMPLATE_SPECIALIZATION
IC const _vertex_id_type& CSGraphVertex::vertex_id() const { return (m_vertex_id); }

TEMPLATE_SPECIALIZATION
IC const _data_type& CSGraphVertex::data() const { return (m_data); }

TEMPLATE_SPECIALIZATION
IC _data_type& CSGraphVertex::data() { return (m_data); }

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::data(const _data_type& data) { m_data = data; }

TEMPLATE_SPECIALIZATION
IC const typename CSGraphVertex::EDGES& CSGraphVertex::edges() const { return (m_edges); }

TEMPLATE_SPECIALIZATION
IC bool CSGraphVertex::operator==(const CVertex& obj) const
{
    if (vertex_id() != obj.vertex_id())
        return (false);

    if (!equal(edges(), obj.edges()))
        return (false);

    return (equal(data(), obj.data()));
}

#undef TEMPLATE_SPECIALIZATION
#undef CSGraphVertex
