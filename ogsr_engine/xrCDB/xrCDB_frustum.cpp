#include "stdafx.h"

#include "../xrExternal/tinybvh.h"

using namespace CDB;

namespace
{
template <bool bClass3, bool bFirst>
class frustum_collider
{
private:
    COLLIDER* dest;
    std::span<const Fvector4> verts;
    std::span<const TRI> tris;

    const CFrustum* F;

public:
    constexpr explicit frustum_collider(COLLIDER* CL, std::span<const Fvector4> V, std::span<const TRI> T, const CFrustum* _F)
        : dest{CL}, verts{V}, tris{T}, F{_F}
    {}

private:
    [[nodiscard]] constexpr EFC_Visible _box(const tinybvh::bvhvec3& min, const tinybvh::bvhvec3& max, u32 mask) const
    {
        // CFrustum::testAABB() takes a pointer to 6 f32s (min.{x,y,z}, max.{x,y,z})
        static_assert(sizeof(std::array<tinybvh::bvhvec3, 2>) == 6 * sizeof(f32));

        return F->testAABB(reinterpret_cast<const f32*>(std::array<tinybvh::bvhvec3, 2>{min, max}.data()), mask);
    }

    constexpr void _prim(u32 prim) const
    {
        auto& tri = tris[prim];
        const std::array<Fvector3, 3> vs{verts[tri.verts[0]].xyz(), verts[tri.verts[1]].xyz(), verts[tri.verts[2]].xyz()};

        if constexpr (bClass3)
        {
            sPoly src, dst;
            src.assign_range(vs);

            if (F->ClipPoly(src, dst))
                dest->r_add(gsl::narrow<s32>(prim), vs[0], vs[1], vs[2], tri.dummy);
        }
        else
        {
            dest->r_add(gsl::narrow<s32>(prim), vs[0], vs[1], vs[2], tri.dummy);
        }
    }

public:
    constexpr void _stab(std::span<const tinybvh::BVH::BVHNode> nodes, std::span<const u32> prim_ids, u32 mask)
    {
        xr::unordered_set<u32> prims;
        xr::inlined_vector<u32, 32> stack;

        stack.emplace_back(0);

        while (!stack.empty())
        {
            auto& node = nodes[stack.back()];
            stack.pop_back();

            // Actual frustum/aabb test
            if (_box(node.aabbMin, node.aabbMax, mask) == fcvNone)
                continue;

            if (!node.isLeaf())
            {
                // 2nd child
                stack.emplace_back(node.leftFirst + 1);
                // 1st child
                stack.emplace_back(node.leftFirst);

                continue;
            }

            for (auto id : prim_ids.subspan(node.leftFirst, node.triCount))
            {
                // Early exit for "only first"
                if constexpr (bFirst)
                {
                    _prim(id);

                    if (!dest->r_empty())
                        return;
                }
                else if (prims.emplace(id).second)
                {
                    _prim(id);
                }
            }
        }
    }
};
} // namespace

void COLLIDER::frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F)
{
    m_def->syncronize();
    r_clear();

    const auto& bvh = *m_def->get_tree();
    const auto mask = F.getMask();

    // Binary dispatcher
    if (frustum_mode & OPT_FULL_TEST)
    {
        if (frustum_mode & OPT_ONLYFIRST)
            frustum_collider<true, true>{this, m_def->get_verts(), m_def->get_tris(), &F}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                std::span{bvh.primIdx, bvh.idxCount}, mask);
        else
            frustum_collider<true, false>{this, m_def->get_verts(), m_def->get_tris(), &F}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                 std::span{bvh.primIdx, bvh.idxCount}, mask);
    }
    else
    {
        if (frustum_mode & OPT_ONLYFIRST)
            frustum_collider<false, true>{this, m_def->get_verts(), m_def->get_tris(), &F}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                 std::span{bvh.primIdx, bvh.idxCount}, mask);
        else
            frustum_collider<false, false>{this, m_def->get_verts(), m_def->get_tris(), &F}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                  std::span{bvh.primIdx, bvh.idxCount}, mask);
    }
}
