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

using namespace CDB;
using namespace Opcode;

namespace
{
template <bool bClass3, bool bFirst>
class frustum_collider
{
public:
    COLLIDER* dest;
    TRI* tris;
    Fvector* verts;

    const CFrustum* F;

    constexpr void _init(COLLIDER* CL, Fvector* V, TRI* T, const CFrustum* _F)
    {
        dest = CL;
        tris = T;
        verts = V;
        F = _F;
    }

    [[nodiscard]] constexpr EFC_Visible _box(const Fvector& C, const Fvector& E, u32 mask) const
    {
        Fvector mM[2];
        mM[0].sub(C, E);
        mM[1].add(C, E);

        return F->testAABB(&mM[0].x, mask);
    }

    constexpr void _prim(u32 prim) const
    {
        const auto id = gsl::narrow<s32>(prim);
        const auto& tri = tris[id];

        if constexpr (bClass3)
        {
            sPoly src, dst;
            src.resize(3);
            src[0] = verts[tri.verts[0]];
            src[1] = verts[tri.verts[1]];
            src[2] = verts[tri.verts[2]];

            if (F->ClipPoly(src, dst))
                dest->r_add(id, verts[tri.verts[0]], verts[tri.verts[1]], verts[tri.verts[2]], tri.dummy);
        }
        else
        {
            dest->r_add(id, verts[tri.verts[0]], verts[tri.verts[1]], verts[tri.verts[2]], tri.dummy);
        }
    }

    constexpr void _stab(const AABBNoLeafNode* node, u32 mask) const
    {
        // Actual frustum/aabb test
        if (_box(*reinterpret_cast<const Fvector*>(&node->mAABB.mCenter), *reinterpret_cast<const Fvector*>(&node->mAABB.mExtents), mask) == fcvNone)
            return;

        // 1st chield
        if (node->HasPosLeaf())
            _prim(node->GetPosPrimitive());
        else
            _stab(node->GetPos(), mask);

        // Early exit for "only first"
        if constexpr (bFirst)
        {
            if (!dest->r_empty())
                return;
        }

        // 2nd chield
        if (node->HasNegLeaf())
            _prim(node->GetNegPrimitive());
        else
            _stab(node->GetNeg(), mask);
    }
};
} // namespace

void COLLIDER::frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F)
{
    m_def->syncronize();

    // This should be smart_cast<>()/dynamic_cast<>(), but OpCoDe doesn't use our custom RTTI.
    // So we just rely on that `AABBOptimizedTree` starts at offset 0 inside `AABBNoLeafTree`.
    // OpCoDe itself uses C-style casts for downcasting, which is roughly the same.
    const AABBNoLeafTree* T = reinterpret_cast<const AABBNoLeafTree*>(m_def->tree->GetTree());
    const AABBNoLeafNode* N = T->GetNodes();
    const DWORD mask = F.getMask();

    r_clear();

    // Binary dispatcher
    if (frustum_mode & OPT_FULL_TEST)
    {
        if (frustum_mode & OPT_ONLYFIRST)
        {
            frustum_collider<true, true> BC;
            BC._init(this, m_def->verts, m_def->tris, &F);
            BC._stab(N, mask);
        }
        else
        {
            frustum_collider<true, false> BC;
            BC._init(this, m_def->verts, m_def->tris, &F);
            BC._stab(N, mask);
        }
    }
    else
    {
        if (frustum_mode & OPT_ONLYFIRST)
        {
            frustum_collider<false, true> BC;
            BC._init(this, m_def->verts, m_def->tris, &F);
            BC._stab(N, mask);
        }
        else
        {
            frustum_collider<false, false> BC;
            BC._init(this, m_def->verts, m_def->tris, &F);
            BC._stab(N, mask);
        }
    }
}
