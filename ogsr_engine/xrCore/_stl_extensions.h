#pragma once

#include <queue>

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>
#include <absl/container/flat_hash_map.h>
#include <absl/hash/hash.h>

#include "xalloc.h"

using std::swap; // TODO: Убрать!

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

template <typename T, typename allocator = xr_allocator<T>>
using xr_deque = std::deque<T, allocator>;

template <typename T, class C = xr_deque<T>>
using xr_stack = std::stack<T, C>;

template <typename T, class C = xr_deque<T>>
using xr_queue = std::queue<T, C>;

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

template <class K, class V, class Hash = absl::DefaultHashContainerHash<K>, class Eq = absl::DefaultHashContainerEq<K>, class Allocator = xr_allocator<std::pair<const K, V>>>
using xr_unordered_map = absl::flat_hash_map<K, V, Hash, Eq, Allocator>;

#define mk_pair std::make_pair // TODO: Везде заменить, а это убрать.

struct pred_str
{
    bool operator()(const char* x, const char* y) const { return std::strcmp(x, y) < 0; }
};

struct pred_stri
{
    bool operator()(const char* x, const char* y) const { return _stricmp(x, y) < 0; }
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
#define DEFINE_STACK(T, N) \
    using N = xr_stack<T>; \
    using N##_it = N::iterator

#define DEFINE_DEQUE(T, N, I) \
    using N = xr_deque<T>; \
    using I = N::iterator
#define DEFINE_LIST(T, N, I) \
    using N = xr_list<T>; \
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
