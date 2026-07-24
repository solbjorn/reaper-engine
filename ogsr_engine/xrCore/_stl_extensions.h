#pragma once

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wredundant-parens");

#include <absl/container/btree_map.h>

XR_DIAG_POP();

#include <absl/container/btree_set.h>
#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wmisleading-indentation");
XR_DIAG_IGNORE("-Wsign-conversion");

#include <plf_bitset.h>

XR_DIAG_POP();

#define _LIBCPP_ABI_BOUNDED_ITERATORS_IN_INPLACE_VECTOR

#include <inplace_vector>
#include <stack>

#include "xalloc.h"

namespace xr
{
template <std::size_t total_size, typename storage_type = std::size_t>
using bitset = plf::bitset<total_size, storage_type, true>;

template <typename T, std::size_t N, typename allocator = xr_allocator<T>>
using inlined_vector = absl::InlinedVector<T, N, allocator>;
} // namespace xr

template <typename T, typename allocator = xr_allocator<T>>
using xr_vector = std::vector<T, allocator>;

template <typename T, typename allocator = xr_allocator<T>>
using xr_deque = std::deque<T, allocator>;

template <typename T, class C = xr_deque<T>>
using xr_stack = std::stack<T, C>;

template <typename T, typename allocator = xr_allocator<T>>
using xr_list = std::list<T, allocator>;

template <typename Key, typename Compare = std::less<Key>, typename Alloc = xr_allocator<Key>>
using xr_set = absl::btree_set<Key, Compare, Alloc>;

template <typename Key, typename Compare = std::less<Key>, typename Alloc = xr_allocator<Key>>
using xr_multiset = absl::btree_multiset<Key, Compare, Alloc>;

template <typename Key, typename Value, typename Compare = std::less<Key>, typename Alloc = xr_allocator<std::pair<const Key, Value>>>
using xr_map = absl::btree_map<Key, Value, Compare, Alloc>;

template <typename Key, typename Value, typename Compare = std::less<Key>, typename Alloc = xr_allocator<std::pair<const Key, Value>>>
using xr_multimap = absl::btree_multimap<Key, Value, Compare, Alloc>;

template <class K, class V, class Hash = typename absl::container_internal::FlatHashMapPolicy<K, V>::DefaultHash,
          class Eq = typename absl::container_internal::FlatHashMapPolicy<K, V>::DefaultEq, class Allocator = xr_allocator<std::pair<const K, V>>>
using xr_unordered_map = absl::flat_hash_map<K, V, Hash, Eq, Allocator>;

template <class K, class V, class Hash = typename absl::container_internal::FlatHashMapPolicy<std::string_view, V>::DefaultHash,
          class Eq = typename absl::container_internal::FlatHashMapPolicy<std::string_view, V>::DefaultEq,
          class Allocator = xr_allocator<std::pair<const K, V>>>
using string_unordered_map = absl::flat_hash_map<K, V, Hash, Eq, Allocator>;

namespace xr
{
template <typename Key, typename Value, typename Compare = absl::container_internal::StringBtreeDefaultLess,
          typename Alloc = xr_allocator<std::pair<const Key, Value>>>
using string_map = absl::btree_map<Key, Value, Compare, Alloc>;

template <typename Key, typename Value, typename Compare = absl::container_internal::StringBtreeDefaultLess,
          typename Alloc = xr_allocator<std::pair<const Key, Value>>>
using string_multimap = absl::btree_multimap<Key, Value, Compare, Alloc>;
} // namespace xr

namespace std
{
// Same as for std::vector<> and absl::InlinedVector<>
template <typename H, typename T, std::size_t C>
[[nodiscard]] constexpr H AbslHashValue(H hash_state, const std::inplace_vector<T, C>& vector)
{
    return H::combine_contiguous(std::move(hash_state), vector.data(), vector.size());
}
} // namespace std

namespace plf
{
// Same as for std::bitset<>
template <typename H, std::size_t total_size, typename storage_type, bool hardened>
[[nodiscard]] constexpr H AbslHashValue(H hash_state, const plf::bitset<total_size, storage_type, hardened>& set)
{
    typename H::AbslInternalPiecewiseCombiner combiner;
    std::size_t i{0};

    while (i + 64 <= total_size)
    {
        u64 word{0};

        for (std::size_t j = 0; j < 64; ++j)
            word |= u64{set[i + j]} << j;

        hash_state = combiner.add_buffer(std::move(hash_state), reinterpret_cast<const std::byte*>(&word), sizeof(word));
        i += 64;
    }

    if (i < total_size)
    {
        const std::size_t rem = total_size - i;
        u64 word{0};

        for (std::size_t j = 0; j < rem; ++j)
            word |= u64{set[i + j]} << j;

        hash_state = combiner.add_buffer(std::move(hash_state), reinterpret_cast<const std::byte*>(&word), (rem + 7) / 8);
    }

    return H::combine(combiner.finalize(std::move(hash_state)), total_size);
}
} // namespace plf

namespace xr
{
template <typename T>
[[nodiscard]] constexpr auto size_bytes(const T& cont)
{
    return cont.size() * sizeof(typename T::value_type);
}

template <typename T>
[[nodiscard]] constexpr auto ssize_bytes(const T& cont)
{
    return std::ssize(cont) * gsl::index{sizeof(typename T::value_type)};
}
} // namespace xr

// STL extensions
#define DEF_VECTOR(N, T) \
    using N = xr_vector<T>; \
    using N##_it = N::iterator
#define DEF_LIST(N, T) \
    using N = xr_list<T>; \
    using N##_it = N::iterator
#define DEF_DEQUE(N, T) \
    using N = xr_deque<T>; \
    using N##_it = N::iterator
#define DEF_MAP(N, K, T) \
    using N = xr_map<K, T>; \
    using N##_it = N::iterator

#define DEFINE_DEQUE(T, N, I) \
    using N = xr_deque<T>; \
    using I = N::iterator
#define DEFINE_VECTOR(T, N, I) \
    using N = xr_vector<T>; \
    using I = N::iterator
#define DEFINE_MAP(K, T, N, I) \
    using N = xr_map<K, T>; \
    using I = N::iterator
#define DEFINE_MAP_PRED(K, T, N, I, P) \
    using N = xr_map<K, T, P>; \
    using I = N::iterator
#define DEFINE_MMAP(K, T, N, I) \
    using N = xr_multimap<K, T>; \
    using I = N::iterator
#define DEFINE_SET(T, N, I) \
    using N = xr_set<T>; \
    using I = N::iterator
#define DEFINE_SET_PRED(T, N, I, P) \
    using N = xr_set<T, P>; \
    using I = N::iterator

// auxilary definition
DEFINE_VECTOR(bool, boolVec, boolIt);
DEFINE_VECTOR(BOOL, BOOLVec, BOOLIt);
DEFINE_VECTOR(BOOL*, LPBOOLVec, LPBOOLIt);
DEFINE_VECTOR(Frect, FrectVec, FrectIt);
DEFINE_VECTOR(Irect, IrectVec, IrectIt);
DEFINE_VECTOR(Fplane, PlaneVec, PlaneIt);
DEFINE_VECTOR(Fvector2, Fvector2Vec, Fvector2It);
DEFINE_VECTOR(Fvector, FvectorVec, FvectorIt);
DEFINE_VECTOR(Fvector*, LPFvectorVec, LPFvectorIt);
DEFINE_VECTOR(Fcolor, FcolorVec, FcolorIt);
DEFINE_VECTOR(Fcolor*, LPFcolorVec, LPFcolorIt);
DEFINE_VECTOR(LPSTR, LPSTRVec, LPSTRIt);
DEFINE_VECTOR(LPCSTR, LPCSTRVec, LPCSTRIt);

DEFINE_VECTOR(s8, S8Vec, S8It);
DEFINE_VECTOR(s8*, LPS8Vec, LPS8It);
DEFINE_VECTOR(s16, S16Vec, S16It);
DEFINE_VECTOR(s16*, LPS16Vec, LPS16It);
DEFINE_VECTOR(s32, S32Vec, S32It);
DEFINE_VECTOR(s32*, LPS32Vec, LPS32It);
DEFINE_VECTOR(u8, U8Vec, U8It);
DEFINE_VECTOR(u8*, LPU8Vec, LPU8It);
DEFINE_VECTOR(u16, U16Vec, U16It);
DEFINE_VECTOR(u16*, LPU16Vec, LPU16It);
DEFINE_VECTOR(u32, U32Vec, U32It);
DEFINE_VECTOR(u32*, LPU32Vec, LPU32It);
DEFINE_VECTOR(float, FloatVec, FloatIt);
DEFINE_VECTOR(float*, LPFloatVec, LPFloatIt);
DEFINE_VECTOR(int, IntVec, IntIt);
DEFINE_VECTOR(int*, LPIntVec, LPIntIt);
