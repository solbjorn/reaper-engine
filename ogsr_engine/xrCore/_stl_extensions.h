#pragma once

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wredundant-parens");

#include <absl/container/btree_map.h>

XR_DIAG_POP();

#include <absl/container/btree_set.h>
#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>

#include <plf_bitset.h>

#include <stack>

#include "xalloc.h"

template <typename T, typename allocator = xr_allocator<T>>
using xr_vector = std::vector<T, allocator>;

template <typename T>
void clear_and_reserve(xr_vector<T>& vec)
{
    if (vec.capacity() <= (vec.size() + vec.size() / 4))
        vec.clear();
    else
    {
        const size_t old = vec.size();
        vec.clear();
        vec.reserve(old);
    }
}

namespace xr
{
template <typename T, size_t N, typename allocator = xr_allocator<T>>
using inlined_vector = absl::InlinedVector<T, N, allocator>;

template <size_t total_size, typename storage_type = size_t>
using bitset = plf::bitset<total_size, storage_type, true>;
} // namespace xr

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

namespace xr
{
// Until libc++ implements std::views::enumerate()
[[nodiscard]] constexpr inline auto views_enumerate(std::ranges::viewable_range auto&& rng)
{
    return std::views::zip(std::views::iota(0z, std::ssize(std::forward<decltype(rng)>(rng))), std::forward<decltype(rng)>(rng));
}
} // namespace xr

struct pred_str
{
    [[nodiscard]] constexpr bool operator()(gsl::czstring x, gsl::czstring y) const { return std::is_lt(xr_strcmp(x, y)); }
};

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
#define DEFINE_SVECTOR(T, C, N, I) \
    typedef svector<T, C>; \
    using I = N::iterator
#define DEFINE_SET(T, N, I) \
    using N = xr_set<T>; \
    using I = N::iterator
#define DEFINE_SET_PRED(T, N, I, P) \
    using N = xr_set<T, P>; \
    using I = N::iterator

#include "FixedVector.h"

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
