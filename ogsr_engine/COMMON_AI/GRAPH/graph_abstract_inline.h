////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _data_type, typename _edge_weight_type, typename _vertex_id_type>

#define CAbstractGraph CGraphAbstract<_data_type, _edge_weight_type, _vertex_id_type>

TEMPLATE_SPECIALIZATION
IC CAbstractGraph::~CGraphAbstract() { clear(); }

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::add_vertex(const _data_type& data, const _vertex_id_type& vertex_id)
{
    XR_ASSERT(vertex(vertex_id) == nullptr);
    m_vertices.emplace(vertex_id, xr_new<CVertex>(data, vertex_id, &m_edge_count));
}

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::remove_vertex(const _vertex_id_type& vertex_id)
{
    const auto I = XR_ASSERT_VAL(m_vertices.find(vertex_id) != m_vertices.end());

    auto v = *I;
    delete_data(v);

    m_vertices.erase(I);
}

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::add_edge(const _vertex_id_type& vertex_id0, const _vertex_id_type& vertex_id1, const _edge_weight_type& edge_weight)
{
    XR_ASSERT_VAL(vertex(vertex_id0) != nullptr)->add_edge(XR_ASSERT_VAL(vertex(vertex_id1) != nullptr), edge_weight);
}

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::add_edge(const _vertex_id_type& vertex_id0, const _vertex_id_type& vertex_id1, const _edge_weight_type& edge_weight0,
                                 const _edge_weight_type& edge_weight1)
{
    add_edge(vertex_id0, vertex_id1, edge_weight0);
    add_edge(vertex_id1, vertex_id0, edge_weight1);
}

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::remove_edge(const _vertex_id_type& vertex_id0, const _vertex_id_type& vertex_id1)
{
    XR_ASSERT(vertex(vertex_id1) != nullptr);
    XR_ASSERT_VAL(vertex(vertex_id0) != nullptr)->remove_edge(vertex_id1);
}

TEMPLATE_SPECIALIZATION
IC u32 CAbstractGraph::vertex_count() const { return (m_vertices.size()); }

TEMPLATE_SPECIALIZATION
IC u32 CAbstractGraph::edge_count() const { return (m_edge_count); }

TEMPLATE_SPECIALIZATION
IC bool CAbstractGraph::empty() const { return (m_vertices.empty()); }

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::clear()
{
    while (!vertices().empty())
        remove_vertex(vertices().begin()->first);

    XR_ASSERT(m_edge_count == 0);
}

TEMPLATE_SPECIALIZATION
IC const typename CAbstractGraph::CVertex* CAbstractGraph::vertex(const _vertex_id_type& vertex_id) const
{
    const_vertex_iterator I = vertices().find(vertex_id);
    if (vertices().end() == I)
        return nullptr;

    return ((*I).second);
}

TEMPLATE_SPECIALIZATION
IC typename CAbstractGraph::CVertex* CAbstractGraph::vertex(const _vertex_id_type& vertex_id)
{
    vertex_iterator I = m_vertices.find(vertex_id);
    if (m_vertices.end() == I)
        return nullptr;

    return ((*I).second);
}

TEMPLATE_SPECIALIZATION
IC const typename CAbstractGraph::CEdge* CAbstractGraph::edge(const _vertex_id_type& vertex_id0, const _vertex_id_type& vertex_id1) const
{
    const CVertex* _vertex = vertex(vertex_id0);
    if (!_vertex)
        return nullptr;

    return (_vertex->edge(vertex_id1));
}

TEMPLATE_SPECIALIZATION
IC typename CAbstractGraph::CEdge* CAbstractGraph::edge(const _vertex_id_type& vertex_id0, const _vertex_id_type& vertex_id1)
{
    CVertex* _vertex = vertex(vertex_id0);
    if (!_vertex)
        return nullptr;

    return (_vertex->edge(vertex_id1));
}

TEMPLATE_SPECIALIZATION
IC const typename CAbstractGraph::VERTICES& CAbstractGraph::vertices() const { return (m_vertices); }

TEMPLATE_SPECIALIZATION
IC typename CAbstractGraph::VERTICES& CAbstractGraph::vertices() { return (m_vertices); }

TEMPLATE_SPECIALIZATION
IC const CAbstractGraph& CAbstractGraph::header() const { return (*this); }

TEMPLATE_SPECIALIZATION
IC bool CAbstractGraph::operator==(const CGraphAbstract& obj) const
{
    if (vertex_count() != obj.vertex_count())
        return (false);

    if (edge_count() != obj.edge_count())
        return (false);

    return (equal(vertices(), obj.vertices()));
}

TEMPLATE_SPECIALIZATION
IC const _edge_weight_type CAbstractGraph::get_edge_weight(const _vertex_id_type vertex_index0, const _vertex_id_type vertex_index1, const_iterator i) const
{
    XR_DEBUG_ASSERT(edge(vertex_index0, vertex_index1) != nullptr);
    return i->weight();
}

TEMPLATE_SPECIALIZATION
IC bool CAbstractGraph::is_accessible(const _vertex_id_type) const { return true; }

TEMPLATE_SPECIALIZATION
IC const typename CAbstractGraph::CVertex* CAbstractGraph::value(const _vertex_id_type, const_iterator i) const { return i->vertex(); }

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::begin(const CVertex* vertex, const_iterator& b, const_iterator& e) const
{
    XR_ASSERT(vertex != nullptr);
    b = vertex->edges().begin();
    e = vertex->edges().end();
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractGraph

#define TEMPLATE_SPECIALIZATION template <typename _data_type, typename _edge_weight_type, typename _vertex_id_type>

#define CAbstractGraph CGraphAbstractSerialize<_data_type, _edge_weight_type, _vertex_id_type>

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::save(IWriter& stream)
{
    stream.open_chunk(0);
    stream.w_u32((u32)this->vertices().size());
    stream.close_chunk();

    stream.open_chunk(1);
    size_t index = 0;
    for (const auto& it : this->vertices())
    {
        stream.open_chunk(index);
        {
            stream.open_chunk(0);
            save_data(it.second->vertex_id(), stream);
            stream.close_chunk();

            stream.open_chunk(1);
            save_data(it.second->data(), stream);
            stream.close_chunk();
        }
        stream.close_chunk();
        ++index;
    }
    stream.close_chunk();

    stream.open_chunk(2);
    {
        for (const auto& it : this->vertices())
        {
            if (it.second->edges().empty())
                continue;

            save_data(it.second->vertex_id(), stream);

            stream.w_u32((u32)it.second->edges().size());
            for (const auto& it2 : it.second->edges())
            {
                save_data(it2.vertex_id(), stream);
                save_data(it2.weight(), stream);
            }
        }
    }
    stream.close_chunk();
}

TEMPLATE_SPECIALIZATION
IC void CAbstractGraph::load(IReader& stream)
{
    this->clear();

    IReader* chunk0 = stream.open_chunk(0);
    std::ignore = chunk0->r_u32();
    chunk0->close();

    u32 id;
    chunk0 = stream.open_chunk(1);

    for (IReader* chunk1 = chunk0->open_chunk_iterator(id); chunk1; chunk1 = chunk0->open_chunk_iterator(id, chunk1))
    {
        _data_type data{};
        _vertex_id_type vertex_id;

        IReader* chunk2 = chunk1->open_chunk(0);
        load_data(vertex_id, *chunk2);
        chunk2->close();

        chunk2 = chunk1->open_chunk(1);
        load_data(data, *chunk2);
        chunk2->close();

        this->add_vertex(data, vertex_id);
    }

    chunk0->close();

    chunk0 = stream.open_chunk(2);
    if (!chunk0)
        return;

    while (!chunk0->eof())
    {
        _vertex_id_type vertex_id0;
        load_data(vertex_id0, *chunk0);

        for (u32 i = 0, n = XR_ASSERT_VAL(chunk0->r_u32() > 0); i < n; ++i)
        {
            _vertex_id_type vertex_id1;
            load_data(vertex_id1, *chunk0);

            _edge_weight_type edge_weight;
            load_data(edge_weight, *chunk0);

            this->add_edge(vertex_id0, std::move(vertex_id1), std::move(edge_weight));
        }
    }

    chunk0->close();
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractGraph
