////////////////////////////////////////////////////////////////////////////
// Module : buffer_vector_inline.h
// Created : 10.10.2007
// Modified : 10.10.2007
// Author : Dmitriy Iassenev
// Description : buffer vector template class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename T>
#define buffer_vector_specialized buffer_vector<T>

TEMPLATE_SPECIALIZATION
inline buffer_vector_specialized::buffer_vector(void* buffer, size_type const& max_count)
    : m_begin((T*)buffer), m_end((T*)buffer), m_max_end((T*)buffer + max_count)
{}

TEMPLATE_SPECIALIZATION
inline buffer_vector_specialized::buffer_vector(void* buffer, size_type const& max_count, size_type const& count, value_type const& value)
    : m_begin((T*)buffer), m_end((T*)buffer), m_max_end((T*)buffer + max_count)
{
    assign(count, value);
}

TEMPLATE_SPECIALIZATION
inline buffer_vector_specialized::buffer_vector(void* buffer, size_type const& max_count, self_type const& other)
    : m_begin((T*)buffer), m_end((T*)buffer), m_max_end((T*)buffer + max_count)
{
    assign(other.begin(), other.end());
}

TEMPLATE_SPECIALIZATION
template <typename input_iterator>
inline buffer_vector_specialized::buffer_vector(void* buffer, size_type const& max_count, input_iterator const& begin, input_iterator const& end)
    : m_begin((T*)buffer), m_end((T*)buffer), m_max_end((T*)buffer + max_count)
{
    assign(begin, end);
}

TEMPLATE_SPECIALIZATION
inline buffer_vector_specialized::~buffer_vector() { clear(); }
TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::self_type& buffer_vector_specialized::operator=(self_type const& other)
{
    assign(other.begin(), other.end());
    return *this;
}

TEMPLATE_SPECIALIZATION
template <typename input_iterator>
inline void buffer_vector_specialized::assign(input_iterator begin, input_iterator const& end)
{
    destroy(m_begin, m_end);
    m_end = XR_ASSERT_VAL(m_begin + (end - begin) <= m_max_end);

    for (iterator I = m_begin; begin != end; ++begin, ++I)
        construct(I, *begin);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::assign(size_type const& count, const_reference value)
{
    destroy(m_begin, m_end);
    m_end = XR_ASSERT_VAL(m_begin + count <= m_max_end);

    for (iterator I = m_begin; I != m_end; ++I)
        construct(I, value);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::swap(self_type& other)
{
    std::swap(m_begin, other.m_begin);
    std::swap(m_end, other.m_end);
    std::swap(m_max_end, other.m_max_end);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::clear()
{
    destroy(m_begin, m_end);
    m_end = m_begin;
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::resize(size_type const& size)
{
    size_type current_size = this->size();
    if (size == current_size)
        return;

    if (size < current_size)
    {
        destroy(m_begin + size, m_begin + current_size);
        m_end = XR_ASSERT_VAL(m_begin + size <= m_max_end);
        return;
    }

    construct(m_begin + current_size, m_begin + size);
    m_end = XR_ASSERT_VAL(m_begin + size <= m_max_end);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::reserve(size_type const&)
{
    // nothing to do here
}

TEMPLATE_SPECIALIZATION
template <typename input_iterator>
inline void buffer_vector_specialized::insert(iterator const& where, input_iterator begin, input_iterator const& end)
{
    difference_type count = end - begin;

    iterator j = m_end + count - 1; // new end
    iterator i = m_end - 1; // old end
    iterator e = XR_ASSERT_VAL(where >= m_begin) - 1;
    for (; i != e; --i, --j)
    {
        construct(j, *i);
        destroy(i);
    }

    m_end = XR_ASSERT_VAL(m_end + count <= m_max_end);

    for (iterator i = where, e = i + count; i != e; ++i, ++begin)
        construct(i, *begin);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::insert(iterator const& where, size_type const& count, const_reference value)
{
    iterator j = m_end + count - 1; // new end
    iterator i = m_end - 1; // old end
    iterator e = XR_ASSERT_VAL(where >= m_begin) - 1;
    for (; i != e; --i, --j)
    {
        construct(j, *i);
        destroy(i);
    }

    m_end = XR_ASSERT_VAL(m_end + count <= m_max_end);

    for (iterator i = where, e = i + count; i != e; ++i)
        construct(i, value);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::insert(iterator const& where, const_reference value) { insert(where, 1, value); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::erase(iterator const& begin, iterator const& end)
{
    XR_ASSERT(begin >= m_begin && begin <= m_end, "", begin, m_begin, m_end);
    XR_ASSERT(end >= m_begin && end <= m_end, "", end, m_begin, m_end);

    if (XR_ASSERT_VAL(begin <= end) == end)
        return;

    for (iterator i = begin, j = end; j != m_end; ++i, ++j)
    {
        destroy(i);
        construct(i, *j);
    }

    size_type count = end - begin;
    size_type new_size = XR_ASSERT_VAL(this->size() >= count) - count;

    destroy(m_begin + new_size, m_end);
    m_end = XR_ASSERT_VAL(m_begin + new_size <= m_max_end);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::erase(iterator const& where) { erase(where, where + 1); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::pop_back()
{
    XR_ASSERT(!empty());

    --m_end;
    destroy(m_end);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::push_back(const_reference value)
{
    construct(XR_ASSERT_VAL(m_end < m_max_end), value);
    ++m_end;
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reference buffer_vector_specialized::at(size_type const& index) { return m_begin[XR_ASSERT_VAL(index < size())]; }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reference buffer_vector_specialized::at(size_type const& index) const
{
    return m_begin[XR_ASSERT_VAL(index < size())];
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reference buffer_vector_specialized::operator[](size_type const& index)
{
    return m_begin[XR_ASSERT_VAL(index < size())];
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reference buffer_vector_specialized::operator[](size_type const& index) const
{
    return m_begin[XR_ASSERT_VAL(index < size())];
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reference buffer_vector_specialized::back()
{
    XR_ASSERT(!empty());
    return (*(m_end - 1));
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reference buffer_vector_specialized::back() const
{
    XR_ASSERT(!empty());
    return (*(m_end - 1));
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reference buffer_vector_specialized::front()
{
    XR_ASSERT(!empty());
    return (*m_begin);
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reference buffer_vector_specialized::front() const
{
    XR_ASSERT(!empty());
    return (*m_begin);
}

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::iterator buffer_vector_specialized::begin() { return (m_begin); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_iterator buffer_vector_specialized::begin() const { return (m_begin); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::iterator buffer_vector_specialized::end() { return (m_end); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_iterator buffer_vector_specialized::end() const { return (m_end); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reverse_iterator buffer_vector_specialized::rbegin() { return (reverse_iterator(end())); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reverse_iterator buffer_vector_specialized::rbegin() const { return (const_reverse_iterator(end())); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::reverse_iterator buffer_vector_specialized::rend() { return (reverse_iterator(begin())); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::const_reverse_iterator buffer_vector_specialized::rend() const { return (const_reverse_iterator(begin())); }

TEMPLATE_SPECIALIZATION
inline bool buffer_vector_specialized::empty() const { return (m_begin == m_end); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::size_type buffer_vector_specialized::size() const { return (m_end - m_begin); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::size_type buffer_vector_specialized::capacity() const { return (m_max_end - m_begin); }

TEMPLATE_SPECIALIZATION
inline typename buffer_vector_specialized::size_type buffer_vector_specialized::max_size() const { return (m_max_end - m_begin); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::construct(pointer p) { new (p) T(); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::construct(pointer p, const_reference value) { new (p) T(value); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::construct(iterator begin, iterator const& end)
{
    for (; begin != end; ++begin)
        construct(begin);
}

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::destroy(pointer p) { p->~T(); }

TEMPLATE_SPECIALIZATION
inline void buffer_vector_specialized::destroy(iterator begin, iterator const& end)
{
    for (; begin != end; ++begin)
        destroy(begin);
}

TEMPLATE_SPECIALIZATION
inline void swap(buffer_vector_specialized& left, buffer_vector_specialized& right) { left.swap(right); }

#undef TEMPLATE_SPECIALIZATION
#undef buffer_vector_specialized
