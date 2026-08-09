#include "stdafx.h"

#include "../xrExternal/tinybvh.h"

using namespace CDB;

namespace
{
struct XR_TRIVIAL alignas(16) vec_t : public Fvector3
{
    f32 pad;

    constexpr vec_t() = default;

    constexpr vec_t(const vec_t& that) { xr_memcpy16(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr vec_t(vec_t&&) = default;
#else
    [[maybe_unused]] constexpr vec_t(vec_t&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr vec_t& operator=(const vec_t& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr vec_t& operator=(vec_t&&) = default;
#else
    [[maybe_unused]] constexpr vec_t& operator=(vec_t&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(vec_t);

struct XR_TRIVIAL alignas(16) aabb_t
{
    vec_t min;
    vec_t max;

    constexpr aabb_t(const aabb_t& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr aabb_t(aabb_t&&) = default;
#else
    [[maybe_unused]] constexpr aabb_t(aabb_t&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr aabb_t& operator=(const aabb_t& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr aabb_t& operator=(aabb_t&&) = default;
#else
    [[maybe_unused]] constexpr aabb_t& operator=(aabb_t&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(aabb_t);

struct XR_TRIVIAL alignas(16) ray_t
{
    vec_t pos;
    vec_t inv_dir;
    vec_t fwd_dir;

    constexpr ray_t() = default;

    constexpr ray_t(const ray_t& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr ray_t(ray_t&&) = default;
#else
    [[maybe_unused]] constexpr ray_t(ray_t&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr ray_t& operator=(const ray_t& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    [[maybe_unused]] constexpr ray_t& operator=(ray_t&&) = default;
#else
    [[maybe_unused]] constexpr ray_t& operator=(ray_t&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(ray_t);

// turn those verbose intrinsics into something readable.
#define loadps(mem) _mm_load_ps(reinterpret_cast<const f32*>(mem))
#define storess(ss, mem) _mm_store_ss(reinterpret_cast<f32*>(mem), (ss))
#define minss _mm_min_ss
#define maxss _mm_max_ss
#define minps _mm_min_ps
#define maxps _mm_max_ps
#define mulps _mm_mul_ps
#define subps _mm_sub_ps
#define rotatelps(ps) _mm_shuffle_ps((ps), (ps), 0x39) // a,b,c,d -> b,c,d,a
#define muxhps(low, high) _mm_movehl_ps((low), (high)) // low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}

constexpr auto flt_plus_inf{std::numeric_limits<f32>::max()};
constexpr float __declspec(align(16)) ps_cst_plus_inf[]{flt_plus_inf, flt_plus_inf, flt_plus_inf, flt_plus_inf};
constexpr float __declspec(align(16)) ps_cst_minus_inf[]{-flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf};

[[nodiscard]] constexpr std::pair<bool, f32> isect_sse(const aabb_t& box, const ray_t& ray)
{
    // you may already have those values hanging around somewhere
    const __m128 plus_inf = loadps(ps_cst_plus_inf), minus_inf = loadps(ps_cst_minus_inf);

    // use whatever's appropriate to load.
    const __m128 box_min = loadps(&box.min), box_max = loadps(&box.max), pos = loadps(&ray.pos), inv_dir = loadps(&ray.inv_dir);

    // use a div if inverted directions aren't available
    const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
    const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

    // the order we use for those min/max is vital to filter out
    // NaNs that happens when an inv_dir is +/- inf and
    // (box_min - pos) is 0. inf * 0 = NaN
    const __m128 filtered_l1a = minps(l1, plus_inf);
    const __m128 filtered_l2a = minps(l2, plus_inf);

    const __m128 filtered_l1b = maxps(l1, minus_inf);
    const __m128 filtered_l2b = maxps(l2, minus_inf);

    // now that we're back on our feet, test those slabs.
    __m128 lmax = maxps(filtered_l1a, filtered_l2a);
    __m128 lmin = minps(filtered_l1b, filtered_l2b);

    // unfold back. try to hide the latency of the shufps & co.
    const __m128 lmax0 = rotatelps(lmax);
    const __m128 lmin0 = rotatelps(lmin);
    lmax = minss(lmax, lmax0);
    lmin = maxss(lmin, lmin0);

    const __m128 lmax1 = muxhps(lmax, lmax);
    const __m128 lmin1 = muxhps(lmin, lmin);
    lmax = minss(lmax, lmax1);
    lmin = maxss(lmin, lmin1);

    std::pair<bool, f32> ret;
    ret.first = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin);
    storess(lmin, &ret.second);

    return ret;
}

#undef loadps
#undef storess
#undef minss
#undef maxss
#undef minps
#undef maxps
#undef mulps
#undef subps
#undef rotatelps
#undef muxhps

template <bool bCull, bool bFirst, bool bNearest>
class XR_TRIVIAL alignas(16) ray_collider
{
private:
    COLLIDER* dest;
    std::span<const Fvector4> verts;
    std::span<const TRI> tris;

    ray_t ray;
    float rRange;

public:
    constexpr ray_collider(const ray_collider& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr ray_collider(ray_collider&&) = default;
#else
    constexpr ray_collider(ray_collider&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr ray_collider& operator=(const ray_collider& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr ray_collider& operator=(ray_collider&&) = default;
#else
    constexpr ray_collider& operator=(ray_collider&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif

    constexpr explicit ray_collider(COLLIDER* CL, std::span<const Fvector4> V, std::span<const TRI> T, const Fvector& C, const Fvector& D, float R)
        : dest{CL}, verts{V}, tris{T}
    {
        ray.pos.set(C);
        ray.inv_dir.x = (_abs(D.x) > flt_eps) ? (1.0f / D.x) : flt_plus_inf;
        ray.inv_dir.y = (_abs(D.y) > flt_eps) ? (1.0f / D.y) : flt_plus_inf;
        ray.inv_dir.z = (_abs(D.z) > flt_eps) ? (1.0f / D.z) : flt_plus_inf;
        ray.fwd_dir.set(D);

        rRange = R;
    }

private:
    // sse
    [[nodiscard]] constexpr std::pair<bool, f32> _box_sse(const tinybvh::BVH::BVHNode& node) const
    {
        static_assert(sizeof(aabb_t) == sizeof(tinybvh::BVH::BVHNode));

        return isect_sse(*reinterpret_cast<const aabb_t*>(std::assume_aligned<sizeof(tinybvh::BVH::BVHNode)>(&node)), ray);
    }

    [[nodiscard]] constexpr bool _tri(std::span<const Fvector3, 3> vs, float& u, float& v, float& range) const
    {
        Fvector edge1, edge2, tvec, pvec, qvec;
        float det, inv_det;

        // find vectors for two edges sharing vert0
        auto& p0 = vs[0];
        auto& p1 = vs[1];
        auto& p2 = vs[2];
        edge1.sub(p1, p0);
        edge2.sub(p2, p0);

        // begin calculating determinant - also used to calculate U parameter
        // if determinant is near zero, ray lies in plane of triangle
        pvec.crossproduct(ray.fwd_dir, edge2);
        det = edge1.dotproduct(pvec);

        if constexpr (bCull)
        {
            if (det < EPS)
                return false;
            tvec.sub(ray.pos, p0); // calculate distance from vert0 to ray origin
            u = tvec.dotproduct(pvec); // calculate U parameter and test bounds
            if (u < 0.f || u > det)
                return false;
            qvec.crossproduct(tvec, edge1); // prepare to test V parameter
            v = ray.fwd_dir.dotproduct(qvec); // calculate V parameter and test bounds
            if (v < 0.f || u + v > det)
                return false;
            range = edge2.dotproduct(qvec); // calculate t, scale parameters, ray intersects triangle
            inv_det = 1.0f / det;
            range *= inv_det;
            u *= inv_det;
            v *= inv_det;
        }
        else
        {
            if (det > -EPS && det < EPS)
                return false;
            inv_det = 1.0f / det;
            tvec.sub(ray.pos, p0); // calculate distance from vert0 to ray origin
            u = tvec.dotproduct(pvec) * inv_det; // calculate U parameter and test bounds
            if (u < 0.0f || u > 1.0f)
                return false;
            qvec.crossproduct(tvec, edge1); // prepare to test V parameter
            v = ray.fwd_dir.dotproduct(qvec) * inv_det; // calculate V parameter and test bounds
            if (v < 0.0f || u + v > 1.0f)
                return false;
            range = edge2.dotproduct(qvec) * inv_det; // calculate t, ray intersects triangle
        }

        return true;
    }

    constexpr void _prim(u32 prim)
    {
        auto& tri = tris[prim];
        const std::array<Fvector3, 3> vs{verts[tri.verts[0]].xyz(), verts[tri.verts[1]].xyz(), verts[tri.verts[2]].xyz()};
        f32 u, v, r;

        if (!_tri(vs, u, v, r))
            return;
        if (r <= 0.0f || r > rRange)
            return;

        if constexpr (bNearest)
        {
            if (!dest->r_empty())
            {
                RESULT& R = *dest->r_begin();
                if (r < R.range)
                {
                    R.id = gsl::narrow<s32>(prim);
                    R.range = r;
                    R.u = u;
                    R.v = v;
                    R.verts[0] = vs[0];
                    R.verts[1] = vs[1];
                    R.verts[2] = vs[2];
                    R.dummy = tri.dummy;
                    rRange = r;
                }
            }
            else
            {
                dest->r_add(gsl::narrow<s32>(prim), r, u, v, vs[0], vs[1], vs[2], tri.dummy);
                rRange = r;
            }
        }
        else
        {
            dest->r_add(gsl::narrow<s32>(prim), r, u, v, vs[0], vs[1], vs[2], tri.dummy);
        }
    }

public:
    constexpr void _stab(std::span<const tinybvh::BVH::BVHNode> nodes, std::span<const u32> prim_ids)
    {
        const auto root = _box_sse(nodes[0]);
        if (!root.first || root.second > rRange)
            return;

        xr::unordered_set<u32> prims;
        xr::inlined_vector<std::pair<u32, f32>, 32> stack;

        stack.emplace_back(0, root.second);

        while (!stack.empty())
        {
            const auto [idx, dist] = stack.back();
            stack.pop_back();

            if (dist > rRange)
                continue;

            auto& node = nodes[idx];
            if (node.isLeaf())
            {
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

                continue;
            }

            const auto left_idx = node.leftFirst;
            const auto right_idx = left_idx + 1;

            const auto [hit_left, d_left] = _box_sse(nodes[left_idx]);
            const auto [hit_right, d_right] = _box_sse(nodes[right_idx]);

            switch ((u32{hit_left && d_left <= rRange} << 1) | u32{hit_right && d_right <= rRange})
            {
            case 3:
                if (d_left < d_right)
                {
                    stack.emplace_back(right_idx, d_right);
                    stack.emplace_back(left_idx, d_left);
                }
                else
                {
                    stack.emplace_back(left_idx, d_left);
                    stack.emplace_back(right_idx, d_right);
                }

                break;
            case 2: stack.emplace_back(left_idx, d_left); break;
            case 1: stack.emplace_back(right_idx, d_right); break;
            case 0: break;
            default: xr::unreachable();
            }
        }
    }
};
XR_TRIVIAL_ASSERT(ray_collider<true, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, false>);
} // namespace

void COLLIDER::ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range)
{
    m_def->syncronize();
    r_clear();

    const auto& bvh = *m_def->get_tree();

    // SSE
    // Binary dispatcher
    if (ray_mode & OPT_CULL)
    {
        if (ray_mode & OPT_ONLYFIRST)
        {
            if (ray_mode & OPT_ONLYNEAREST)
                ray_collider<true, true, true>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
            else
                ray_collider<true, true, false>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
        }
        else
        {
            if (ray_mode & OPT_ONLYNEAREST)
                ray_collider<true, false, true>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
            else
                ray_collider<true, false, false>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
        }
    }
    else
    {
        if (ray_mode & OPT_ONLYFIRST)
        {
            if (ray_mode & OPT_ONLYNEAREST)
                ray_collider<false, true, true>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
            else
                ray_collider<false, true, false>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
        }
        else
        {
            if (ray_mode & OPT_ONLYNEAREST)
                ray_collider<false, false, true>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
            else
                ray_collider<false, false, false>{this, m_def->get_verts(), m_def->get_tris(), r_start, r_dir, r_range}._stab(
                    std::span{bvh.bvhNode, bvh.usedNodes}, std::span{bvh.primIdx, bvh.idxCount});
        }
    }
}
