////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _DataStorage, typename _dist_type, typename _index_type, typename _iteration_type>

#define CLevelPathManager \
    CPathManager<CLevelGraph, _DataStorage, SBaseParameters<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>

TEMPLATE_SPECIALIZATION
IC void CLevelPathManager::setup(const _Graph* _graph, _DataStorage* _data_storage, xr_vector<_index_type>* _path, const _index_type& _start_node_index,
                                 const _index_type& _goal_node_index, const _Parameters& parameters)
{
    inherited::setup(_graph, _data_storage, _path, _start_node_index, _goal_node_index, parameters);
    m_distance_xz = this->graph->header().cell_size();
    m_sqr_distance_xz = _sqr(this->graph->header().cell_size());
}

TEMPLATE_SPECIALIZATION
IC void CLevelPathManager::init()
{
    const _Graph::CVertex& tNode1 = *(this->graph->vertex(this->start_node_index));
    this->graph->unpack_xz(tNode1, x2, z2);

    const _Graph::CVertex& tNode2 = *(this->graph->vertex(this->goal_node_index));
    this->graph->unpack_xz(tNode2, x3, z3);

    x1 = x2;
    z1 = z2;
}

TEMPLATE_SPECIALIZATION
IC _dist_type CLevelPathManager::evaluate(const _index_type& /*node_index1*/, const _index_type& /*node_index2*/, const _Graph::const_iterator& /*i*/)
{
    XR_ASSERT(this->graph != nullptr);
    return m_distance_xz;
}

TEMPLATE_SPECIALIZATION
IC _dist_type CLevelPathManager::estimate(const _index_type& /*node_index*/) const
{
    XR_ASSERT(this->graph != nullptr);
    return 2 * m_distance_xz * _dist_type(_abs(x3 - x1) + _abs(z3 - z1));
}

TEMPLATE_SPECIALIZATION
IC bool CLevelPathManager::is_goal_reached(const _index_type& node_index)
{
    if (node_index == this->goal_node_index)
        return true;

    best_node = this->graph->vertex(node_index);
    this->graph->unpack_xz(best_node, x1, z1);

    return false;
}

TEMPLATE_SPECIALIZATION
IC bool CLevelPathManager::is_limit_reached(const _iteration_type iteration_count) const
{
    XR_ASSERT(this->data_storage != nullptr);
    return inherited::is_limit_reached(iteration_count);
}

TEMPLATE_SPECIALIZATION
IC bool CLevelPathManager::is_accessible(const _index_type& vertex_id) const { return XR_ASSERT_VAL(this->graph != nullptr)->is_accessible(vertex_id); }

TEMPLATE_SPECIALIZATION
inline void CLevelPathManager::begin(const _index_type&, const_iterator& begin, const_iterator& end) { this->graph->begin(best_node, begin, end); }

TEMPLATE_SPECIALIZATION
IC const _index_type CLevelPathManager::get_value(const_iterator& i) const { return this->graph->value(best_node, i); }

#undef TEMPLATE_SPECIALIZATION
#undef CLevelPathManager
