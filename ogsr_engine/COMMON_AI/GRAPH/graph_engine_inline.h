////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGraphEngine::CGraphEngine(u32 max_vertex_count)
{
    m_algorithm = xr_new<CAlgorithm>(max_vertex_count);
    m_algorithm->data_storage().set_min_bucket_value(GraphEngineSpace::_dist_type(0));
    m_algorithm->data_storage().set_max_bucket_value(GraphEngineSpace::_dist_type(2000));

    m_solver_algorithm = xr_new<CSolverAlgorithm>(SolverMaxVertexCount);
}

IC CGraphEngine::~CGraphEngine()
{
    xr_delete(m_algorithm);
    xr_delete(m_solver_algorithm);
}

IC const CGraphEngine::CSolverAlgorithm& CGraphEngine::solver_algorithm() const { return (*m_solver_algorithm); }

template <typename _Graph, typename _Parameters>
IC bool CGraphEngine::search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node,
                             xr_vector<GraphEngineSpace::_index_type>* node_path, const _Parameters& parameters)
{
    Device.Statistic->AI_Path.Begin();

    using CPathManagerGeneric =
        CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, GraphEngineSpace::_dist_type, GraphEngineSpace::_index_type, GraphEngineSpace::_iteration_type>;

    CPathManagerGeneric path_manager;

    path_manager.setup(&graph, &m_algorithm->data_storage(), node_path, start_node, dest_node, parameters);
    const bool successful = m_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return successful;
}

template <typename _Graph, typename _Parameters>
IC bool CGraphEngine::search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node,
                             xr_vector<GraphEngineSpace::_index_type>* node_path, _Parameters& parameters)
{
    Device.Statistic->AI_Path.Begin();

    using CPathManagerGeneric =
        CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, GraphEngineSpace::_dist_type, GraphEngineSpace::_index_type, GraphEngineSpace::_iteration_type>;

    CPathManagerGeneric path_manager;

    path_manager.setup(&graph, &m_algorithm->data_storage(), node_path, start_node, dest_node, parameters);
    const bool successful = m_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return successful;
}

template <typename _Graph, typename _Parameters, typename _PathManager>
IC bool CGraphEngine::search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node,
                             xr_vector<GraphEngineSpace::_index_type>* node_path, const _Parameters& parameters, _PathManager& path_manager)
{
    Device.Statistic->AI_Path.Begin();

    path_manager.setup(&graph, &m_algorithm->data_storage(), node_path, start_node, dest_node, parameters);
    const bool successful = m_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return successful;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, bool T6, typename T7, typename T8, typename _Parameters>
IC bool CGraphEngine::search(const CProblemSolver<T1, T2, T3, T4, T5, T6, T7, T8>& graph, const GraphEngineSpace::_solver_index_type& start_node,
                             const GraphEngineSpace::_solver_index_type& dest_node, xr_vector<GraphEngineSpace::_solver_edge_type>* node_path, const _Parameters& parameters)
{
    Device.Statistic->AI_Path.Begin();

    using CSProblemSolver = CProblemSolver<T1, T2, T3, T4, T5, T6, T7, T8>;
    using CSolverPathManager = CPathManager<CSProblemSolver, CSolverAlgorithm::CDataStorage, _Parameters, GraphEngineSpace::_solver_dist_type, GraphEngineSpace::_solver_index_type,
                                            GraphEngineSpace::_iteration_type>;

    CSolverPathManager path_manager;

    path_manager.setup(&graph, &m_solver_algorithm->data_storage(), node_path, start_node, dest_node, parameters);
    const bool successful = m_solver_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return successful;
}
