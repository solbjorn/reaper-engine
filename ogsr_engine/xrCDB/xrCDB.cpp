// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "../xrExternal/tinybvh.h"

namespace xxh
{
#include <xxhash.h>
}

using namespace CDB;

namespace xr
{
namespace
{
constexpr auto build_settings = [] [[nodiscard]] {
    tinybvh::BVHBuildSettings settings;

    settings.useSpatialSplits = true;
    settings.postOptimize = true;
    settings.optimizeIterations = 30;

    return settings;
}();
} // namespace
} // namespace xr

// Model building
MODEL::MODEL() = default;

MODEL::~MODEL()
{
    syncronize(); // maybe model still in building
    status = state::S_INIT;
}

void MODEL::build(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc, void* bcp)
{
    XR_ASSERT(status == state::S_INIT);
    XR_ASSERT(V.size() >= 4 && T.size() >= 2, "", V, T);

    build_internal(V, T, bc, bcp);
    status = state::S_READY;
}

void MODEL::build_internal(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc, void* bcp)
{
    // verts
    verts.clear();
    verts.reserve(V.size());

    for (auto& vert : V)
        verts.emplace_back(vert, 1.0f);

    // tris
    tris.assign_range(T);

    // callback
    if (bc != nullptr)
        bc(verts, tris, bcp);

    status = state::S_BUILD;

    xr_vector<u32> indices;
    indices.reserve(tris.size() * 3);

    for (const auto& tri : tris)
    {
        indices.emplace_back(tri.verts[0]);
        indices.emplace_back(tri.verts[1]);
        indices.emplace_back(tri.verts[2]);
    }

    tree = std::make_unique<tinybvh::BVH>();
    tree->settings = xr::build_settings;

    tree->context.malloc = [] [[nodiscard]] (std::size_t size, void*) {
        return xrMemory::mem_alloc_aligned(gsl::narrow_cast<gsl::index>(xr::roundup(size, 64uz)), 64z);
    };
    tree->context.free = [](void* ptr, void*) { xrMemory::mem_free_aligned(ptr); };

    tree->Build(tinybvh::bvhvec4slice{reinterpret_cast<const tinybvh::bvhvec4*>(verts.data()), gsl::narrow_cast<u32>(verts.size())}, indices.data(),
                gsl::narrow_cast<u32>(indices.size() / 3));

    // Although none of tree's methods is used beyond this point, zero out pointers to verts
    // and indices inside it to trigger stable access violation instead of UB due to possible
    // dangling references just in case this changes in future.
    tree->verts = tinybvh::bvhvec4slice{};
    tree->vertIdx = nullptr;
}

gsl::index MODEL::memory() const
{
    if (status != state::S_READY)
    {
        Log("! xrCDB: model still isn't ready");
        return 0;
    }

    const auto V = verts.capacity() * sizeof(Fvector4);
    const auto T = tris.capacity() * sizeof(TRI);
    const auto nodes = xr::roundup(tree->allocatedNodes * sizeof(*tree->bvhNode), 64uz);
    const auto prim_ids = xr::roundup(tree->idxCount * sizeof(*tree->primIdx), 64uz);
    const auto frags = tree->fragment != nullptr ? xr::roundup(tree->triCount * sizeof(*tree->fragment), 64uz) : 0;

    return gsl::narrow_cast<gsl::index>(sizeof(*this) + V + T + sizeof(std::remove_cvref_t<decltype(*tree)>) + nodes + prim_ids + frags);
}

// Serialization and deserialization

namespace xr
{
namespace
{
// tinybvh::BVHBuildSettings of determined size and with no paddings
struct XR_TRIVIAL alignas(16) bvh_build_serdes final
{
private:
    static constexpr auto curr{(u32{tinybvh::BVHBase::BVHType::LAYOUT_BVH} << 24) | u32{TINY_BVH_CACHE_VERSION}};

    u32 magic;
    u16 sizeof_settings;

    u16 usePresplitting : 1;
    u16 useSpatialSplits : 1;
    u16 presplitPostPass : 1;
    u16 useFullSweep : 1;
    u16 useLBVH : 1;
    u16 postOptimize : 1;
    u16 useSIMDifavailable : 1;
    u16 padding : 9;

    f32 presplitFactor;
    s32 optimizeIterations;

public:
    constexpr bvh_build_serdes() = default;

    constexpr explicit bvh_build_serdes(const tinybvh::BVHBuildSettings& settings)
    {
        magic = curr;
        sizeof_settings = sizeof(std::remove_cvref_t<decltype(settings)>);

        usePresplitting = settings.usePresplitting;
        useSpatialSplits = settings.useSpatialSplits;
        presplitPostPass = settings.presplitPostPass;
        useFullSweep = settings.useFullSweep;
        useLBVH = settings.useLBVH;
        postOptimize = settings.postOptimize;
        useSIMDifavailable = settings.useSIMDifavailable;
        padding = 0;

        presplitFactor = settings.presplitFactor;
        optimizeIterations = settings.optimizeIterations;
    }

    constexpr bvh_build_serdes(const bvh_build_serdes& that) { xr_memcpy16(this, &that); }

    XR_DIAG_PUSH();
    XR_DIAG_IGNORE("-Wunused-member-function");

#ifdef XR_TRIVIAL_BROKEN
    constexpr bvh_build_serdes(bvh_build_serdes&&) = default;
#else
    constexpr bvh_build_serdes(bvh_build_serdes&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr bvh_build_serdes& operator=(const bvh_build_serdes& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr bvh_build_serdes& operator=(bvh_build_serdes&&) = default;
#else
    constexpr bvh_build_serdes& operator=(bvh_build_serdes&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif

    XR_DIAG_POP();

    [[nodiscard]] constexpr auto operator==(const bvh_build_serdes& that) const
    {
        if (magic != that.magic)
            return false;
        if (sizeof_settings != that.sizeof_settings)
            return false;

        if (usePresplitting != that.usePresplitting)
            return false;
        if (useSpatialSplits != that.useSpatialSplits)
            return false;
        if (presplitPostPass != that.presplitPostPass)
            return false;
        if (useFullSweep != that.useFullSweep)
            return false;
        if (useLBVH != that.useLBVH)
            return false;
        if (postOptimize != that.postOptimize)
            return false;
        if (useSIMDifavailable != that.useSIMDifavailable)
            return false;
        if (padding != that.padding)
            return false;

        if (!fsimilar(presplitFactor, that.presplitFactor))
            return false;
        if (optimizeIterations != that.optimizeIterations)
            return false;

        return true;
    }
};
static_assert(sizeof(xr::bvh_build_serdes) == 16);
XR_TRIVIAL_ASSERT(xr::bvh_build_serdes);

struct alignas(16) model_mid_hdr final
{
    xr::bvh_build_serdes serdes;

    u64 verts_num;
    xxh::XXH64_hash_t verts_xxh;

    u64 tris_num;
    xxh::XXH64_hash_t tris_xxh;
};
static_assert(sizeof(xr::model_mid_hdr) == 48);

struct alignas(16) model_end_hdr final
{
    u64 nodes_num;
    xxh::XXH64_hash_t nodes_xxh;

    u64 ids_num;
    xxh::XXH64_hash_t ids_xxh;
};
static_assert(sizeof(xr::model_end_hdr) == 32);

constexpr xr::bvh_build_serdes build_serdes{xr::build_settings};
} // namespace
} // namespace xr

void MODEL::serialize_tree(IWriter& stream) const
{
    const auto nodes_mem{tree->usedNodes * sizeof(*tree->bvhNode)};
    const auto ids_mem{tree->idxCount * sizeof(*tree->primIdx)};

    const xr::model_end_hdr hdr{.nodes_num = tree->usedNodes,
                                .nodes_xxh = xxh::XXH3_64bits(tree->bvhNode, nodes_mem),
                                .ids_num = tree->idxCount,
                                .ids_xxh = xxh::XXH3_64bits(tree->primIdx, ids_mem)};

    stream.w(&hdr, sizeof(hdr));
    stream.w(tree->bvhNode, gsl::narrow_cast<gsl::index>(nodes_mem));
    stream.w(tree->primIdx, gsl::narrow_cast<gsl::index>(xr::roundup(ids_mem, 16uz)));
}

void MODEL::serialize(gsl::czstring file, u64 xxh, serialize_callback callback) const
{
    auto& stream = *XR_ASSERT_VAL(FS.w_open(file) != nullptr);
    const auto _ = gsl::finally([&stream] {
        auto ws = &stream;
        FS.w_close(ws);
    });

    if (callback != nullptr)
        callback(stream);

    stream.w_u64(xxh);
    stream.seek(xr::roundup(stream.tell(), 16z));

    const auto verts_mem{xr::size_bytes(verts)};
    const auto tris_mem{xr::size_bytes(tris)};

    const xr::model_mid_hdr hdr{.serdes = xr::build_serdes,
                                .verts_num = verts.size(),
                                .verts_xxh = xxh::XXH3_64bits(verts.data(), verts_mem),
                                .tris_num = tris.size(),
                                .tris_xxh = xxh::XXH3_64bits(tris.data(), tris_mem)};

    stream.w(&hdr, sizeof(hdr));
    stream.w(verts.data(), gsl::narrow_cast<gsl::index>(verts_mem));
    stream.w(tris.data(), gsl::narrow_cast<gsl::index>(tris_mem));

    serialize_tree(stream);
}

bool MODEL::deserialize_tree(IReader& stream)
{
    tree = std::make_unique<tinybvh::BVH>();

    // Pointers to verts and indices inside the tree are intentionally left zeroed,
    // see comment at the bottom of MODEL::build_internal().

    xr::model_end_hdr hdr;

    if (stream.elapsed() < gsl::index{sizeof(hdr)})
        return false;

    xr_memcpy128(&hdr, stream.pointer(), sizeof(hdr));
    stream.advance(sizeof(hdr));

    const auto nodes_mem{hdr.nodes_num * sizeof(*tree->bvhNode)};
    if (stream.elapsed() < gsl::narrow_cast<gsl::index>(nodes_mem))
        return false;

    tree->bvhNode = static_cast<decltype(tree->bvhNode)>(xrMemory::mem_alloc_aligned(gsl::narrow_cast<gsl::index>(xr::roundup(nodes_mem, 64uz)), 64z));
    tree->usedNodes = gsl::narrow_cast<u32>(hdr.nodes_num);
    tree->allocatedNodes = tree->usedNodes;

    xr_memcpy128(tree->bvhNode, stream.pointer(), nodes_mem);
    stream.advance(gsl::narrow_cast<gsl::index>(nodes_mem));

    if (xxh::XXH3_64bits(tree->bvhNode, nodes_mem) != hdr.nodes_xxh)
        return false;

    const auto ids_mem{hdr.ids_num * sizeof(*tree->primIdx)};
    if (stream.elapsed() < gsl::narrow_cast<gsl::index>(xr::roundup(ids_mem, 16uz)))
        return false;

    tree->primIdx = static_cast<decltype(tree->primIdx)>(xrMemory::mem_alloc_aligned(gsl::narrow_cast<gsl::index>(xr::roundup(ids_mem, 64uz)), 64z));
    tree->idxCount = gsl::narrow_cast<u32>(hdr.ids_num);

    xr_memcpy128(tree->primIdx, stream.pointer(), xr::roundup(ids_mem, 16uz));
    stream.advance(gsl::narrow_cast<gsl::index>(xr::roundup(ids_mem, 16uz)));

    if (xxh::XXH3_64bits(tree->primIdx, ids_mem) != hdr.ids_xxh)
        return false;

    return true;
}

bool MODEL::deserialize(gsl::czstring file, u64 xxh, deserialize_callback callback)
{
    const auto stream = absl::WrapUnique(FS.r_open(file));
    if (!stream)
        return false;

    if (callback != nullptr && !callback(*stream))
        return false;

    if (stream->r_u64() != xxh)
        return false;

    stream->seek(xr::roundup(stream->tell(), 16z));

    xr::model_mid_hdr hdr;

    if (stream->elapsed() < gsl::index{sizeof(hdr)})
        return false;

    xr_memcpy128(&hdr, stream->pointer(), sizeof(hdr));
    stream->advance(sizeof(hdr));

    if (hdr.serdes != xr::build_serdes)
        return false;

    const auto verts_mem = hdr.verts_num * sizeof(Fvector4);
    if (stream->elapsed() < gsl::narrow_cast<gsl::index>(verts_mem))
        return false;

    verts.assign_range(std::span{static_cast<const Fvector4*>(stream->pointer()), hdr.verts_num});
    stream->advance(gsl::narrow_cast<gsl::index>(verts_mem));

    if (xxh::XXH3_64bits(verts.data(), verts_mem) != hdr.verts_xxh)
        return false;

    const auto tris_mem = hdr.tris_num * sizeof(TRI);
    if (stream->elapsed() < gsl::narrow_cast<gsl::index>(tris_mem))
        return false;

    tris.assign_range(std::span{static_cast<const TRI*>(stream->pointer()), hdr.tris_num});
    stream->advance(gsl::narrow_cast<gsl::index>(tris_mem));

    if (xxh::XXH3_64bits(tris.data(), tris_mem) != hdr.tris_xxh)
        return false;

    if (!deserialize_tree(*stream))
        return false;

    status = state::S_READY;

    return true;
}
