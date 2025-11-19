////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_nearest_vertex.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : path manager level nearest vertex
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_level.h"

template <typename _DataStorage, typename _dist_type, typename _index_type, typename _iteration_type>
class CPathManager<CLevelGraph, _DataStorage, SNearestVertex<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>
    : public CPathManager<CLevelGraph, _DataStorage, SBaseParameters<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>
{
    RTTI_DECLARE_TYPEINFO(CPathManager<CLevelGraph, _DataStorage, SNearestVertex<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>,
                          CPathManager<CLevelGraph, _DataStorage, SBaseParameters<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>);

protected:
    using _Graph = CLevelGraph;
    using _Parameters = SNearestVertex<_dist_type, _index_type, _iteration_type>;
    using inherited = CPathManager<_Graph, _DataStorage, SBaseParameters<_dist_type, _index_type, _iteration_type>, _dist_type, _index_type, _iteration_type>;

protected:
    int x0, y0;
    u32 max_range_sqr;
    float m_cell_dist;
    Fvector m_target_position;
    float m_best_distance_to_target;

public:
    ~CPathManager() override = default;

    IC void setup(const _Graph* graph, _DataStorage* _data_storage, xr_vector<_index_type>* _path, const _index_type& _start_node_index, const _index_type& _goal_node_index,
                  const _Parameters& params);
    IC bool is_goal_reached(const _index_type& node_index);
    inline _dist_type evaluate(const _index_type&, const _index_type&, const _Graph::const_iterator&);
    inline _dist_type estimate(const _index_type&) const;
    IC bool is_accessible(const _index_type& vertex_id) const;
    IC bool is_limit_reached(const _iteration_type iteration_count) const;

    template <typename T>
    inline void create_path(T&);
};

#include "path_manager_level_nearest_vertex_inline.h"
