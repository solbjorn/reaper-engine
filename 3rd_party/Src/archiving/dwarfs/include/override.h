#if __has_include(<parallel_hashmap/phmap_config.h>)
#include <parallel_hashmap/phmap_config.h>
#endif

#if !defined(ABSL_OPTION_HARDENED) || ABSL_OPTION_HARDENED != 2
#error Invalid override order
#endif

#ifndef PHMAP_BTREE_BTREE_CONTAINER_H_
#define PHMAP_BTREE_BTREE_CONTAINER_H_

#include <absl/container/btree_map.h>
#include <absl/container/btree_set.h>

namespace phmap
{
using absl::btree_map;
using absl::btree_set;
} // namespace phmap

#endif // PHMAP_BTREE_BTREE_CONTAINER_H_

#if !defined(phmap_h_guard_)
#define phmap_h_guard_

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <absl/container/node_hash_map.h>
#include <absl/container/node_hash_set.h>

namespace phmap
{
template <
    class K, class V,
    class Hash =
        typename absl::container_internal::FlatHashMapPolicy<K, V>::DefaultHash,
    class Eq = typename absl::container_internal::FlatHashMapPolicy<K, V>::DefaultEq,
    class Allocator =
        typename absl::container_internal::FlatHashMapPolicy<K, V>::DefaultAlloc>
class ABSL_ATTRIBUTE_OWNER flat_hash_map : public absl::flat_hash_map<K, V, Hash, Eq, Allocator>
{
public:
    using slot_type = typename absl::container_internal::FlatHashMapPolicy<K, V>::slot_type;
};

using absl::flat_hash_set;
using absl::node_hash_map;
using absl::node_hash_set;
} // namespace phmap

#endif // phmap_h_guard_
