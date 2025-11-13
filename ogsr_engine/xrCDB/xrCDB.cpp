// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wclass-conversion");
XR_DIAG_IGNORE("-Wfloat-conversion");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wheader-hygiene");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wshorten-64-to-32");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wunknown-pragmas");
XR_DIAG_IGNORE("-Wunused-parameter");

#include <Opcode.h>

XR_DIAG_POP();

namespace xxh
{
#include <xxhash.h>
}

using namespace CDB;
using namespace Opcode;

// Model building
MODEL::MODEL()
#ifdef PROFILE_CRITICAL_SECTIONS
    : cs(MUTEX_PROFILE_ID(MODEL))
#endif // PROFILE_CRITICAL_SECTIONS
{}

MODEL::~MODEL()
{
    syncronize(); // maybe model still in building
    status = S_INIT;

    xr_delete(tree);
    xr_free(tris);
    tris_count = 0;
    xr_free(verts);
    verts_count = 0;
}

void MODEL::build(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc, void* bcp)
{
    R_ASSERT(S_INIT == status);
    R_ASSERT(V.size() >= 4 && T.size() >= 2);

    build_internal(V, T, bc, bcp);
    status = S_READY;
}

void MODEL::build_internal(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc, void* bcp)
{
    // verts
    verts_count = std::ssize(V);
    verts = xr_alloc<Fvector>(verts_count);
    std::memcpy(verts, V.data(), gsl::narrow_cast<size_t>(verts_count * gsl::index{sizeof(Fvector)}));

    // tris
    tris_count = std::ssize(T);
    tris = xr_alloc<TRI>(tris_count);

#ifdef XR_TRIVIAL_BROKEN
    XR_DIAG_PUSH();
    XR_DIAG_IGNORE("-Wnontrivial-memcall");
#endif

    std::memcpy(tris, T.data(), gsl::narrow_cast<size_t>(tris_count * gsl::index{sizeof(TRI)}));

#ifdef XR_TRIVIAL_BROKEN
    XR_DIAG_POP();
#endif

    // callback
    if (bc)
        bc(std::span{verts, gsl::narrow_cast<size_t>(verts_count)}, std::span{tris, gsl::narrow_cast<size_t>(tris_count)}, bcp);

    // Release data pointers
    status = S_BUILD;

    MeshInterface mif;
    mif.SetNbTriangles(gsl::narrow_cast<u32>(tris_count));
    mif.SetNbVertices(gsl::narrow_cast<u32>(verts_count));
    mif.SetPointers(reinterpret_cast<const IceMaths::IndexedTriangle*>(tris), reinterpret_cast<const IceMaths::Point*>(verts));
    mif.SetStrides(sizeof(TRI));

    // Build a non quantized no-leaf tree
    OPCODECREATE OPCC;
    OPCC.mIMesh = &mif;
    OPCC.mQuantized = false;

    tree = xr_new<Model>();
    if (!tree->Build(OPCC))
    {
        xr_free(verts);
        xr_free(tris);
    }
}

gsl::index MODEL::memory() const
{
    if (S_BUILD == status)
    {
        Msg("! xrCDB: model still isn't ready");
        return 0;
    }

    const auto V = verts_count * gsl::index{sizeof(Fvector)};
    const auto T = tris_count * gsl::index{sizeof(TRI)};

    return tree->GetUsedBytes() + V + T + gsl::index{sizeof(*this)} + gsl::index{sizeof(*tree)};
}

struct alignas(16) model_mid_hdr
{
    s64 tris_count;
    s64 verts_count;
    xxh::XXH64_hash_t tris_xxh;
    xxh::XXH64_hash_t verts_xxh;
};

struct alignas(16) model_end_hdr
{
    u32 model_code;
    u32 nodes_num;
    u64 nodes;
    xxh::XXH64_hash_t nodes_xxh;
    u64 pad;
};

void MODEL::serialize_tree(IWriter* stream) const
{
    // This should be smart_cast<>()/dynamic_cast<>(), but OpCoDe doesn't use our custom RTTI.
    // So we just rely on that `AABBOptimizedTree` starts at offset 0 inside `AABBNoLeafTree`.
    // OpCoDe itself uses C-style casts for downcasting, which is roughly the same.
    const auto* root = reinterpret_cast<const AABBNoLeafTree*>(tree->GetTree())->GetNodes();
    const auto nodes_num = tree->GetNbNodes();
    const auto size = xr::roundup(nodes_num * sizeof(AABBNoLeafNode), 16uz);

    const model_end_hdr hdr = {
        .model_code = tree->GetModelCode(),
        .nodes_num = nodes_num,
        .nodes = u64{reinterpret_cast<uintptr_t>(root)},
        .nodes_xxh = xxh::XXH3_64bits(root, size),
        .pad = 0,
    };

    stream->w(&hdr, sizeof(hdr));
    stream->w(root, gsl::narrow_cast<gsl::index>(size));
}

bool MODEL::serialize(const char* file, u64 xxh, serialize_callback callback) const
{
    IWriter* wstream = FS.w_open(file);
    if (!wstream)
        return false;

    if (callback)
        callback(*wstream);

    wstream->w_u64(xxh);
    wstream->seek(xr::roundup(wstream->tell(), 16z));

    const auto trs = tris_count * gsl::index{sizeof(TRI)};
    const auto vrs = xr::roundup(verts_count * gsl::index{sizeof(Fvector)}, 16z);

    const model_mid_hdr hdr = {
        .tris_count = tris_count,
        .verts_count = verts_count,
        .tris_xxh = xxh::XXH3_64bits(tris, gsl::narrow_cast<size_t>(trs)),
        .verts_xxh = xxh::XXH3_64bits(verts, gsl::narrow_cast<size_t>(vrs)),
    };

    wstream->w(&hdr, sizeof(hdr));
    wstream->w(tris, trs);
    wstream->w(verts, vrs);

    if (tree)
        serialize_tree(wstream);

    FS.w_close(wstream);

    return true;
}

bool MODEL::deserialize_tree(IReader* stream)
{
    model_end_hdr hdr;
    xr_memcpy128(&hdr, stream->pointer(), sizeof(hdr));
    stream->advance(sizeof(hdr));

    auto* mTree = xr_new<AABBNoLeafTree>();
    auto* ptr = xr_alloc<AABBNoLeafNode>(hdr.nodes_num);

    const auto size = hdr.nodes_num * sizeof(AABBNoLeafNode);
    xr_memcpy128(ptr, stream->pointer(), size);

    if (xxh::XXH3_64bits(ptr, size) != hdr.nodes_xxh)
    {
        xr_free(ptr);
        xr_delete(mTree);

        return false;
    }

    for (gsl::index i{}; i < hdr.nodes_num; ++i)
    {
        if (!ptr[i].HasPosLeaf())
        {
            ptr[i].mPosData -= hdr.nodes;
            ptr[i].mPosData += reinterpret_cast<uintptr_t>(ptr);
        }
        if (!ptr[i].HasNegLeaf())
        {
            ptr[i].mNegData -= hdr.nodes;
            ptr[i].mNegData += reinterpret_cast<uintptr_t>(ptr);
        }
    }

    struct faketree
    {
        u8 pad1[8];
        u32 mNbNodes;
        u8 pad2[4];
        AABBNoLeafNode* mNodes;
    }* ft = reinterpret_cast<faketree*>(mTree);

    ft->mNbNodes = hdr.nodes_num;
    ft->mNodes = ptr;

    struct fakemodel
    {
        u8 pad1[16];
        u32 mModelCode;
        u8 pad2[12];
        AABBOptimizedTree* mTree;
    }* fm = reinterpret_cast<fakemodel*>(tree);

    fm->mModelCode = hdr.model_code;
    fm->mTree = mTree;

    return true;
}

bool MODEL::deserialize(const char* file, u64 xxh, deserialize_callback callback)
{
    IReader* rstream = FS.r_open(file);
    if (!rstream)
        return false;

    if (callback && !callback(*rstream))
    {
    err_close:
        FS.r_close(rstream);
        return false;
    }

    if (rstream->r_u64() != xxh)
        goto err_close;

    rstream->seek(xr::roundup(rstream->tell(), 16z));

    xr_free(tris);
    xr_free(verts);
    xr_delete(tree);

    model_mid_hdr mid;
    xr_memcpy128(&mid, rstream->pointer(), sizeof(mid));
    rstream->advance(sizeof(mid));

    tris_count = mid.tris_count;
    verts_count = mid.verts_count;

    tris = xr_alloc<TRI>(tris_count);
    const auto trisSize = tris_count * gsl::index{sizeof(TRI)};
    xr_memcpy128(tris, rstream->pointer(), gsl::narrow_cast<size_t>(trisSize));
    rstream->advance(trisSize);

    if (xxh::XXH3_64bits(tris, gsl::narrow_cast<size_t>(trisSize)) != mid.tris_xxh)
    {
    err_tris:
        xr_free(tris);
        goto err_close;
    }

    verts = xr_alloc<Fvector>(verts_count);
    const auto vertsSize = xr::roundup(verts_count * gsl::index{sizeof(Fvector)}, 16z);
    xr_memcpy128(verts, rstream->pointer(), gsl::narrow_cast<size_t>(vertsSize));
    rstream->advance(vertsSize);

    if (xxh::XXH3_64bits(verts, gsl::narrow_cast<size_t>(vertsSize)) != mid.verts_xxh)
    {
    err_verts:
        xr_free(verts);
        goto err_tris;
    }

    if (rstream->eof())
        return true;

    tree = xr_new<Model>();
    if (!deserialize_tree(rstream))
    {
        xr_delete(tree);
        goto err_verts;
    }

    FS.r_close(rstream);
    status = S_READY;

    return true;
}
