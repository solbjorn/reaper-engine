// xrCDB.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <Opcode.h>

using namespace CDB;
using namespace Opcode;

#ifndef XRCDB_STATIC
BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
#endif

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

COLLIDER::~COLLIDER() { r_free(); }

RESULT& COLLIDER::r_add() { return rd.emplace_back(RESULT()); }

void COLLIDER::r_free() { rd.clear(); }
