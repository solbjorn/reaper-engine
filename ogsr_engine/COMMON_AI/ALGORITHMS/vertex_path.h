////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_path.h
//	Created 	: 21.03.2002
//  Modified 	: 02.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Vertex path class
////////////////////////////////////////////////////////////////////////////

#pragma once

template <bool EuclidianHeuristics = true>
struct CVertexPath
{
    template <typename TCompoundVertex>
    struct VertexData
    {};

    template <typename TCompoundVertex>
    class CDataStorage : public virtual RTTI::Enable
    {
        RTTI_DECLARE_TYPEINFO(CDataStorage<TCompoundVertex>);

    public:
        using Vertex = TCompoundVertex;
        using Index = typename Vertex::Index;

    public:
        inline explicit CDataStorage(const u32 vertex_count);
        inline virtual ~CDataStorage();

        inline void init();
        inline void assign_parent(Vertex& neighbour, Vertex* parent);
        template <typename T>
        inline void assign_parent(Vertex& neighbour, Vertex* parent, const T&);
        inline void update_successors(Vertex& neighbour);
        inline void get_node_path(xr_vector<Index>& path, Vertex* best);
    };
};

#include "vertex_path_inline.h"
