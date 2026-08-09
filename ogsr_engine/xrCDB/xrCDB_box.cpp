#include "stdafx.h"

#include "../xrExternal/tinybvh.h"

using namespace CDB;

namespace
{
// Was a part of IceMath
class Point final
{
public:
    f32 x, y, z;

    constexpr Point() = default;

    template <typename tof>
    constexpr explicit Point(tof xx, tof yy, tof zz) : x{gsl::narrow_cast<f32>(xx)}, y{gsl::narrow_cast<f32>(yy)}, z{gsl::narrow_cast<f32>(zz)}
    {}

    constexpr Point(const Point& p) : x{p.x}, y{p.y}, z{p.z} {}

    constexpr Point& Set(const Point& src)
    {
        x = src.x;
        y = src.y;
        z = src.z;
        return *this;
    }

    constexpr Point& operator=(const Point& p)
    {
        Set(p);
        return *this;
    }

    [[nodiscard]] constexpr Point operator-() const { return Point(-x, -y, -z); }
    [[nodiscard]] constexpr Point operator-(const Point& p) const { return Point(x - p.x, y - p.y, z - p.z); }

    [[nodiscard]] constexpr f32 operator|(const Point& p) const { return x * p.x + y * p.y + z * p.z; }
    [[nodiscard]] constexpr Point operator^(const Point& p) const { return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x); }

    [[nodiscard]] constexpr operator const f32*() const { return &x; }
    [[nodiscard]] constexpr operator f32*() { return &x; }
};

//! This macro quickly finds the min & max values among 3 variables
#define FINDMINMAX(x0, x1, x2, min, max) \
    min = max = x0; \
    if (x1 < min) \
        min = x1; \
    if (x1 > max) \
        max = x1; \
    if (x2 < min) \
        min = x2; \
    if (x2 > max) \
    max = x2

//! TO BE DOCUMENTED
[[nodiscard]] constexpr bool planeBoxOverlap(const Point& normal, float d, const Point& maxbox)
{
    Point vmin, vmax;
    const f32* anorm{normal};
    const f32* abox{maxbox};
    f32* amin{vmin};
    f32* amax{vmax};

    for (gsl::index q{}; q < 3; ++q)
    {
        if (anorm[q] > 0.0f)
        {
            amin[q] = -abox[q];
            amax[q] = abox[q];
        }
        else
        {
            amin[q] = abox[q];
            amax[q] = -abox[q];
        }
    }

    if ((normal | vmin) + d > 0.0f)
        return false;
    if ((normal | vmax) + d >= 0.0f)
        return true;

    return false;
}

//! TO BE DOCUMENTED
#define AXISTEST_X01(a, b, fa, fb) \
    min = a * v0.y - b * v0.z; \
    max = a * v2.y - b * v2.z; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.y + fb * extents.z; \
    if (min > rad || max < -rad) \
    return false

//! TO BE DOCUMENTED
#define AXISTEST_X2(a, b, fa, fb) \
    min = a * v0.y - b * v0.z; \
    max = a * v1.y - b * v1.z; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.y + fb * extents.z; \
    if (min > rad || max < -rad) \
    return false

//! TO BE DOCUMENTED
#define AXISTEST_Y02(a, b, fa, fb) \
    min = b * v0.z - a * v0.x; \
    max = b * v2.z - a * v2.x; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.x + fb * extents.z; \
    if (min > rad || max < -rad) \
    return false

//! TO BE DOCUMENTED
#define AXISTEST_Y1(a, b, fa, fb) \
    min = b * v0.z - a * v0.x; \
    max = b * v1.z - a * v1.x; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.x + fb * extents.z; \
    if (min > rad || max < -rad) \
    return false

//! TO BE DOCUMENTED
#define AXISTEST_Z12(a, b, fa, fb) \
    min = a * v1.x - b * v1.y; \
    max = a * v2.x - b * v2.y; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.x + fb * extents.y; \
    if (min > rad || max < -rad) \
    return false

//! TO BE DOCUMENTED
#define AXISTEST_Z0(a, b, fa, fb) \
    min = a * v0.x - b * v0.y; \
    max = a * v1.x - b * v1.y; \
    if (min > max) \
    { \
        const float tmp = max; \
        max = min; \
        min = tmp; \
    } \
    rad = fa * extents.x + fb * extents.y; \
    if (min > rad || max < -rad) \
    return false

template <bool bClass3, bool bFirst>
class box_collider final
{
private:
    COLLIDER* dest;
    std::span<const Fvector4> verts;
    std::span<const TRI> tris;

    Fvector b_min, b_max;
    Point center, extents;

    Point mLeafVerts[3];

public:
    constexpr explicit box_collider(COLLIDER* CL, std::span<const Fvector4> V, std::span<const TRI> T, const Fvector& C, const Fvector& E)
        : dest{CL}, verts{V}, tris{T}
    {
        b_min.sub(C, E);
        b_max.add(C, E);

        center = Point(C.x, C.y, C.z);
        extents = Point(E.x, E.y, E.z);
    }

private:
    [[nodiscard]] constexpr bool _box(const tinybvh::bvhvec3& min, const tinybvh::bvhvec3& max) const
    {
        if (b_max.x < min.x)
            return false;
        if (b_max.y < min.y)
            return false;
        if (b_max.z < min.z)
            return false;
        if (b_min.x > max.x)
            return false;
        if (b_min.y > max.y)
            return false;
        if (b_min.z > max.z)
            return false;

        return true;
    }

    [[nodiscard]] constexpr bool _tri() const
    {
        // move everything so that the boxcenter is in (0,0,0)
        Point v0, v1, v2;
        v0.x = mLeafVerts[0].x - center.x;
        v1.x = mLeafVerts[1].x - center.x;
        v2.x = mLeafVerts[2].x - center.x;

        // First, test overlap in the {x,y,z}-directions
        float min, max;
        // Find min, max of the triangle in x-direction, and test for overlap in X
        FINDMINMAX(v0.x, v1.x, v2.x, min, max);
        if (min > extents.x || max < -extents.x)
            return false;

        // Same for Y
        v0.y = mLeafVerts[0].y - center.y;
        v1.y = mLeafVerts[1].y - center.y;
        v2.y = mLeafVerts[2].y - center.y;

        FINDMINMAX(v0.y, v1.y, v2.y, min, max);
        if (min > extents.y || max < -extents.y)
            return false;

        // Same for Z
        v0.z = mLeafVerts[0].z - center.z;
        v1.z = mLeafVerts[1].z - center.z;
        v2.z = mLeafVerts[2].z - center.z;

        FINDMINMAX(v0.z, v1.z, v2.z, min, max);
        if (min > extents.z || max < -extents.z)
            return false;

        // 2) Test if the box intersects the plane of the triangle
        // compute plane equation of triangle: normal*x+d=0
        // ### could be precomputed since we use the same leaf triangle several times
        const Point e0 = v1 - v0;
        const Point e1 = v2 - v1;
        const Point normal = e0 ^ e1;
        const float d = -normal | v0;
        if (!planeBoxOverlap(normal, d, extents))
            return false;

        // 3) "Class III" tests
        if constexpr (bClass3)
        {
            float rad;
            float min, max;
            // compute triangle edges
            // - edges lazy evaluated to take advantage of early exits
            // - fabs precomputed (half less work, possible since extents are always >0)
            // - customized macros to take advantage of the null component
            // - axis vector3 discarded, possibly saves useless movs

            const float fey0 = _abs(e0.y);
            const float fez0 = _abs(e0.z);
            AXISTEST_X01(e0.z, e0.y, fez0, fey0);
            const float fex0 = _abs(e0.x);
            AXISTEST_Y02(e0.z, e0.x, fez0, fex0);
            AXISTEST_Z12(e0.y, e0.x, fey0, fex0);

            const float fey1 = _abs(e1.y);
            const float fez1 = _abs(e1.z);
            AXISTEST_X01(e1.z, e1.y, fez1, fey1);
            const float fex1 = _abs(e1.x);
            AXISTEST_Y02(e1.z, e1.x, fez1, fex1);
            AXISTEST_Z0(e1.y, e1.x, fey1, fex1);

            const Point e2 = mLeafVerts[0] - mLeafVerts[2];
            const float fey2 = _abs(e2.y);
            const float fez2 = _abs(e2.z);
            AXISTEST_X2(e2.z, e2.y, fez2, fey2);
            const float fex2 = _abs(e2.x);
            AXISTEST_Y1(e2.z, e2.x, fez2, fex2);
            AXISTEST_Z12(e2.y, e2.x, fey2, fex2);
        }

        return true;
    }

    constexpr void _prim(u32 prim)
    {
        auto& T = tris[prim];

        auto& v0 = verts[T.verts[0]];
        mLeafVerts[0].x = v0.x;
        mLeafVerts[0].y = v0.y;
        mLeafVerts[0].z = v0.z;

        auto& v1 = verts[T.verts[1]];
        mLeafVerts[1].x = v1.x;
        mLeafVerts[1].y = v1.y;
        mLeafVerts[1].z = v1.z;

        auto& v2 = verts[T.verts[2]];
        mLeafVerts[2].x = v2.x;
        mLeafVerts[2].y = v2.y;
        mLeafVerts[2].z = v2.z;

        if (!_tri())
            return;

        dest->r_add(gsl::narrow<s32>(prim), v0.xyz(), v1.xyz(), v2.xyz(), T.dummy);
    }

public:
    constexpr void _stab(std::span<const tinybvh::BVH::BVHNode> nodes, std::span<const u32> prim_ids)
    {
        xr::unordered_set<u32> prims;
        xr::inlined_vector<u32, 32> stack;

        stack.emplace_back(0);

        while (!stack.empty())
        {
            auto& node = nodes[stack.back()];
            stack.pop_back();

            // Actual box-box test
            if (!_box(node.aabbMin, node.aabbMax))
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

void COLLIDER::box_query(u32 box_mode, const MODEL* m_def, const Fvector& b_center, const Fvector& b_dim)
{
    m_def->syncronize();
    r_clear();

    const auto& bvh = *m_def->get_tree();

    // Binary dispatcher
    if (box_mode & OPT_FULL_TEST)
    {
        if (box_mode & OPT_ONLYFIRST)
            box_collider<true, true>{this, m_def->get_verts(), m_def->get_tris(), b_center, b_dim}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                         std::span{bvh.primIdx, bvh.idxCount});
        else
            box_collider<true, false>{this, m_def->get_verts(), m_def->get_tris(), b_center, b_dim}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                          std::span{bvh.primIdx, bvh.idxCount});
    }
    else
    {
        if (box_mode & OPT_ONLYFIRST)
            box_collider<false, true>{this, m_def->get_verts(), m_def->get_tris(), b_center, b_dim}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                          std::span{bvh.primIdx, bvh.idxCount});
        else
            box_collider<false, false>{this, m_def->get_verts(), m_def->get_tris(), b_center, b_dim}._stab(std::span{bvh.bvhNode, bvh.usedNodes},
                                                                                                           std::span{bvh.primIdx, bvh.idxCount});
    }
}
