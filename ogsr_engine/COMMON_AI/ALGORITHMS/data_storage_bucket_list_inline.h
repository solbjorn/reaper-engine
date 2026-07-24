////////////////////////////////////////////////////////////////////////////
//  Module      : data_storage_bucket_list_inline.h
//  Created     : 21.03.2002
//  Modified    : 26.02.2004
//  Author      : Dmitriy Iassenev
//  Description : Bucket list data storage inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
    template <typename TPathId, typename TBucketId, u32 BucketCount, bool ClearBuckets> \
    template <typename TManagerDataStorage>

#define CBucketList CDataStorageBucketList<TPathId, TBucketId, BucketCount, ClearBuckets>::CDataStorage<TManagerDataStorage>

TEMPLATE_SPECIALIZATION
inline CBucketList::CDataStorage(const u32 vertex_count) : TManagerDataStorage(vertex_count)
{
    m_max_distance = typename TManagerDataStorage::Vertex::Distance(-1);
    m_min_bucket_value = typename TManagerDataStorage::Vertex::Distance(0);
    m_max_bucket_value = typename TManagerDataStorage::Vertex::Distance(1000);
    std::memset(m_buckets, 0, BucketSize);
    m_min_bucket_id = 0;
}

TEMPLATE_SPECIALIZATION
CBucketList::~CDataStorage() = default;

TEMPLATE_SPECIALIZATION
inline void CBucketList::init()
{
    TManagerDataStorage::init();
    std::memset(m_list_data, 0, 2 * sizeof(typename TManagerDataStorage::Vertex));
    m_list_head = m_list_data;
    m_list_tail = m_list_data + 1;
    m_list_head->next() = m_list_tail;
    m_list_tail->f() = m_max_distance;
    m_list_tail->prev() = m_list_head;
    m_min_bucket_id = BucketCount;

    if constexpr (ClearBuckets)
        std::memset(m_buckets, 0, BucketSize);
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::add_best_closed()
{
    XR_DEBUG_ASSERT(!is_opened_empty());
    TManagerDataStorage::add_closed(*m_buckets[m_min_bucket_id]);
}

TEMPLATE_SPECIALIZATION
inline bool CBucketList::is_opened_empty()
{
    if (m_min_bucket_id == BucketCount)
        return true;
    if (!m_buckets[m_min_bucket_id])
    {
        m_min_bucket_id++;
        if constexpr (!ClearBuckets)
        {
            while (m_min_bucket_id < BucketCount)
            {
                auto bucket = m_buckets[m_min_bucket_id];
                if (!bucket || bucket->m_path_id != this->current_path_id() || bucket->m_bucket_id != m_min_bucket_id)
                {
                    m_min_bucket_id++;
                    continue;
                }
                break;
            }
        }
        else
        {
            while (m_min_bucket_id < BucketCount && !m_buckets[m_min_bucket_id])
                m_min_bucket_id++;
        }
        return m_min_bucket_id >= BucketCount;
    }
    return false;
}

TEMPLATE_SPECIALIZATION
inline u32 CBucketList::compute_bucket_id(typename TManagerDataStorage::Vertex& vertex) const
{
    typename TManagerDataStorage::Vertex::Distance dist = vertex.f();
    if (dist >= m_max_bucket_value)
        return BucketCount - 1;
    if (dist <= m_min_bucket_value)
        return 0;
    return u32(BucketCount * (dist - m_min_bucket_value) / (m_max_bucket_value - m_min_bucket_value));
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::verify_buckets() const {}

TEMPLATE_SPECIALIZATION
inline void CBucketList::add_to_bucket(typename TManagerDataStorage::Vertex& vertex, u32 m_bucket_id)
{
    if (m_bucket_id < m_min_bucket_id)
        m_min_bucket_id = m_bucket_id;
    typename TManagerDataStorage::Vertex* i = m_buckets[m_bucket_id];
    if (!i || (!ClearBuckets && (i->m_path_id != this->current_path_id())) || (i->m_bucket_id != m_bucket_id))
    {
        vertex.m_bucket_id = m_bucket_id;
        vertex.m_path_id = this->current_path_id();
        m_buckets[m_bucket_id] = &vertex;
        vertex.next() = nullptr;
        vertex.prev() = nullptr;
        verify_buckets();
        return;
    }
    vertex.m_bucket_id = m_bucket_id;
    vertex.m_path_id = this->current_path_id();
    if (i->f() >= vertex.f())
    {
        m_buckets[m_bucket_id] = &vertex;
        vertex.next() = i;
        vertex.prev() = nullptr;
        i->prev() = &vertex;
        verify_buckets();
        return;
    }
    if (!i->next())
    {
        vertex.prev() = i;
        vertex.next() = nullptr;
        i->next() = &vertex;
        verify_buckets();
        return;
    }
    for (i = i->next(); i->next(); i = i->next())
    {
        if (i->f() >= vertex.f())
        {
            vertex.next() = i;
            vertex.prev() = i->prev();
            i->prev()->next() = &vertex;
            i->prev() = &vertex;
            verify_buckets();
            return;
        }
    }
    if (i->f() >= vertex.f())
    {
        vertex.next() = i;
        vertex.prev() = i->prev();
        i->prev()->next() = &vertex;
        i->prev() = &vertex;
        verify_buckets();
        return;
    }
    else
    {
        vertex.next() = nullptr;
        vertex.prev() = i;
        i->next() = &vertex;
        verify_buckets();
        return;
    }
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::add_opened(typename TManagerDataStorage::Vertex& vertex)
{
    TManagerDataStorage::add_opened(vertex);

    add_to_bucket(vertex, compute_bucket_id(vertex));
    verify_buckets();
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::decrease_opened(typename TManagerDataStorage::Vertex& vertex)
{
    XR_DEBUG_ASSERT(!is_opened_empty());
    u32 node_bucket_id = compute_bucket_id(vertex);

    if (vertex.prev() != nullptr)
    {
        vertex.prev()->next() = vertex.next();
    }
    else
    {
        auto& bucket = m_buckets[vertex.m_bucket_id];
        XR_ASSERT(bucket == &vertex);
        bucket = vertex.next();
    }

    if (vertex.next() != nullptr)
        vertex.next()->prev() = vertex.prev();

    verify_buckets();
    add_to_bucket(vertex, node_bucket_id);
    verify_buckets();
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::remove_best_opened()
{
    XR_DEBUG_ASSERT(!is_opened_empty());
    verify_buckets();

    auto& bucket = m_buckets[m_min_bucket_id];
    XR_ASSERT(bucket != nullptr);
    XR_DEBUG_ASSERT(this->is_visited(bucket->index()));

    bucket = bucket->next();
    if (bucket != nullptr)
        bucket->prev() = nullptr;

    verify_buckets();
}

TEMPLATE_SPECIALIZATION
inline typename TManagerDataStorage::Vertex& CBucketList::get_best()
{
    XR_DEBUG_ASSERT(!is_opened_empty());
    return *m_buckets[m_min_bucket_id];
}

TEMPLATE_SPECIALIZATION
inline void CBucketList::set_min_bucket_value(const typename TManagerDataStorage::Vertex::Distance min_bucket_value) { m_min_bucket_value = min_bucket_value; }

TEMPLATE_SPECIALIZATION
inline void CBucketList::set_max_bucket_value(const typename TManagerDataStorage::Vertex::Distance max_bucket_value) { m_max_bucket_value = max_bucket_value; }

#undef TEMPLATE_SPECIALIZATION
#undef CBucketList
