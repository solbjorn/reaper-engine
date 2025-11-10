////////////////////////////////////////////////////////////////////////////
//  Module      : data_storage_constructor.h
//  Created     : 21.03.2002
//  Modified    : 28.02.2004
//  Author      : Dmitriy Iassenev
//  Description : Data storage constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

struct EmptyVertexData
{
    template <typename TCompoundVertex> // result mixin type
    struct VertexData
    {};
};

template <typename... Components>
struct CompoundVertex : Components::template VertexData<CompoundVertex<Components...>>...
{};

template <typename TPriorityQueuee, // CDataStorageBucketList|CDataStorageBinaryHeap
          typename TVertexManager, // CVertexManagerFixed|CVertexManagerHashFixed
          typename TPathBuilder, // CEdgePath|CVertexPath
          typename TVertexAllocator, // CVertexAllocatorFixed
          typename TCompoundVertex, typename TManagerDataStorage = typename TVertexManager::template CDataStorage<TPathBuilder, TVertexAllocator, TCompoundVertex>,
          typename TDataStorageBase = typename TPriorityQueuee::template CDataStorage<TManagerDataStorage>>
struct PriorityQueueConstructor : public TDataStorageBase
{
    RTTI_DECLARE_TYPEINFO(PriorityQueueConstructor<TPriorityQueuee, TVertexManager, TPathBuilder, TVertexAllocator, TCompoundVertex, TManagerDataStorage, TDataStorageBase>,
                          TDataStorageBase);

public:
    using Inherited = TDataStorageBase;
    using Vertex = TCompoundVertex;
    using Index = typename Vertex::Index;

    explicit PriorityQueueConstructor(u32 vertex_count) : Inherited{vertex_count} {}

    void init() { Inherited::init(); }
    Vertex& create_vertex(const Index& vertex_id) { return Inherited::create_vertex(Inherited::CDataStorageAllocator::create_vertex(), vertex_id); }
};
