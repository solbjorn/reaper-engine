////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "game_graph.h"

template <typename _VertexEvaluator, typename _vertex_id_type, typename _index_type>
class CBasePathManager<CGameGraph, _VertexEvaluator, _vertex_id_type, _index_type> : public CAbstractPathManager<CGameGraph, _VertexEvaluator, _vertex_id_type, _index_type>
{
    RTTI_DECLARE_TYPEINFO(CBasePathManager<CGameGraph, _VertexEvaluator, _vertex_id_type, _index_type>,
                          CAbstractPathManager<CGameGraph, _VertexEvaluator, _vertex_id_type, _index_type>);

public:
    typedef CAbstractPathManager<CGameGraph, _VertexEvaluator, _vertex_id_type, _index_type> inherited;

    using inherited::dest_vertex_id;
    using inherited::m_intermediate_index;
    using inherited::m_object;
    using inherited::path;

protected:
    IC virtual void before_search(const _vertex_id_type, const _vertex_id_type);
    IC virtual void after_search();

public:
    inline explicit CBasePathManager(CRestrictedObject* object);
    ~CBasePathManager() override = default;

    IC virtual void reinit(const CGameGraph* graph = nullptr);
    IC bool actual() const;
    IC virtual void select_intermediate_vertex();
    IC virtual bool completed() const;
};

#include "game_path_manager_inline.h"
