// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <Opcode.h>
#include <xxhash.h>

using namespace CDB;
using namespace Opcode;

// Model building
MODEL::MODEL()
#ifdef PROFILE_CRITICAL_SECTIONS
    : cs(MUTEX_PROFILE_ID(MODEL))
#endif // PROFILE_CRITICAL_SECTIONS
{
    tree = 0;
    tris = 0;
    tris_count = 0;
    verts = 0;
    verts_count = 0;
    status = S_INIT;
}
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

void MODEL::build(Fvector* V, size_t Vcnt, TRI* T, size_t Tcnt, build_callback* bc, void* bcp)
{
    R_ASSERT(S_INIT == status);
    R_ASSERT((Vcnt >= 4) && (Tcnt >= 2));

    build_internal(V, Vcnt, T, Tcnt, bc, bcp);
    status = S_READY;
}

void MODEL::build_internal(Fvector* V, size_t Vcnt, TRI* T, size_t Tcnt, build_callback* bc, void* bcp)
{
    // verts
    verts_count = Vcnt;
    verts = xr_alloc<Fvector>(verts_count);
    CopyMemory(verts, V, verts_count * sizeof(Fvector));

    // tris
    tris_count = Tcnt;
    tris = xr_alloc<TRI>(tris_count);
    CopyMemory(tris, T, tris_count * sizeof(TRI));

    // callback
    if (bc)
        bc(verts, Vcnt, tris, Tcnt, bcp);

    // Release data pointers
    status = S_BUILD;

    MeshInterface* mif = xr_new<MeshInterface>();
    mif->SetNbTriangles(tris_count);
    mif->SetNbVertices(verts_count);
    mif->SetPointers(reinterpret_cast<const IceMaths::IndexedTriangle*>(tris), reinterpret_cast<const IceMaths::Point*>(verts));
    mif->SetStrides(sizeof(TRI));

    // Build a non quantized no-leaf tree
    OPCODECREATE OPCC = OPCODECREATE();
    OPCC.mIMesh = mif;
    OPCC.mQuantized = false;

    tree = xr_new<Model>();
    if (!tree->Build(OPCC))
    {
        xr_free(verts);
        xr_free(tris);
        xr_delete(mif);
        return;
    };

    xr_delete(mif);
}

size_t MODEL::memory()
{
    if (S_BUILD == status)
    {
        Msg("! xrCDB: model still isn't ready");
        return 0;
    }
    const size_t V = verts_count * sizeof(Fvector);
    const size_t T = tris_count * sizeof(TRI);
    return tree->GetUsedBytes() + V + T + sizeof(*this) + sizeof(*tree);
}

struct alignas(16) model_mid_hdr
{
    u64 tris_count;
    u64 verts_count;
    XXH64_hash_t tris_xxh;
    XXH64_hash_t verts_xxh;
};

struct alignas(16) model_end_hdr
{
    u32 model_code;
    u32 nodes_num;
    u64 nodes;
    XXH64_hash_t nodes_xxh;
    u64 pad;
};

void MODEL::serialize_tree(IWriter* stream) const
{
    const u32 nodes_num = tree->GetNbNodes();
    const auto* root = ((const AABBNoLeafTree*)tree->GetTree())->GetNodes();
    const size_t size = roundup(nodes_num * sizeof(AABBNoLeafNode), 16);

    const model_end_hdr hdr = {
        .model_code = tree->GetModelCode(),
        .nodes_num = nodes_num,
        .nodes = (uintptr_t)root,
        .nodes_xxh = XXH3_64bits(root, size),
        .pad = 0,
    };

    stream->w(&hdr, sizeof(hdr));
    stream->w(root, size);
}

bool MODEL::serialize(const char* file, u64 xxh, serialize_callback callback) const
{
    IWriter* wstream = FS.w_open(file);
    if (!wstream)
        return false;

    if (callback)
        callback(*wstream);

    wstream->w_u64(xxh);
    wstream->seek(roundup(wstream->tell(), 16));

    const size_t trs = sizeof(TRI) * tris_count;
    const size_t vrs = roundup(sizeof(Fvector) * verts_count, 16);

    const model_mid_hdr hdr = {
        .tris_count = tris_count,
        .verts_count = verts_count,
        .tris_xxh = XXH3_64bits(tris, trs),
        .verts_xxh = XXH3_64bits(verts, vrs),
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
    R_ASSERT(mTree && ptr);

    const size_t size = hdr.nodes_num * sizeof(AABBNoLeafNode);
    xr_memcpy128(ptr, stream->pointer(), size);

    if (XXH3_64bits(ptr, size) != hdr.nodes_xxh)
    {
        xr_free(ptr);
        xr_delete(mTree);

        return false;
    }

    for (u32 i = 0; i < hdr.nodes_num; i++)
    {
        if (!ptr[i].HasPosLeaf())
        {
            ptr[i].mPosData -= hdr.nodes;
            ptr[i].mPosData += (uintptr_t)ptr;
        }
        if (!ptr[i].HasNegLeaf())
        {
            ptr[i].mNegData -= hdr.nodes;
            ptr[i].mNegData += (uintptr_t)ptr;
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

    rstream->seek(roundup(rstream->tell(), 16));

    xr_free(tris);
    xr_free(verts);
    xr_delete(tree);

    model_mid_hdr mid;
    xr_memcpy128(&mid, rstream->pointer(), sizeof(mid));
    rstream->advance(sizeof(mid));

    tris_count = mid.tris_count;
    verts_count = mid.verts_count;

    tris = xr_alloc<TRI>(tris_count);
    const size_t trisSize = tris_count * sizeof(TRI);
    xr_memcpy128(tris, rstream->pointer(), trisSize);
    rstream->advance(trisSize);

    if (XXH3_64bits(tris, trisSize) != mid.tris_xxh)
    {
    err_tris:
        xr_free(tris);
        goto err_close;
    }

    verts = xr_alloc<Fvector>(verts_count);
    const size_t vertsSize = roundup(verts_count * sizeof(Fvector), 16);
    xr_memcpy128(verts, rstream->pointer(), vertsSize);
    rstream->advance(vertsSize);

    if (XXH3_64bits(verts, vertsSize) != mid.verts_xxh)
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

COLLIDER::~COLLIDER() { r_free(); }

RESULT& COLLIDER::r_add() { return rd.emplace_back(RESULT()); }

void COLLIDER::r_free() { rd.clear(); }
