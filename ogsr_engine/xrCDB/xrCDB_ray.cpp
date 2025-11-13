#include "stdafx.h"

#include "xrcpuid.h"

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

#include <xmmintrin.h>

using namespace CDB;
using namespace Opcode;

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

    constexpr aabb_t() = default;

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

[[nodiscard]] constexpr u32 uf(const float& x) { return std::bit_cast<u32>(x); }

[[nodiscard]] constexpr bool isect_fpu(const Fvector& min, const Fvector& max, const ray_t& ray, Fvector& coord)
{
    Fvector MaxT{-1.0f, -1.0f, -1.0f};
    BOOL Inside = TRUE;

    // Find candidate planes.
    if (ray.pos[0] < min[0])
    {
        coord[0] = min[0];
        Inside = FALSE;
        if (uf(ray.inv_dir[0]))
            MaxT[0] = (min[0] - ray.pos[0]) * ray.inv_dir[0]; // Calculate T distances to candidate planes
    }
    else if (ray.pos[0] > max[0])
    {
        coord[0] = max[0];
        Inside = FALSE;
        if (uf(ray.inv_dir[0]))
            MaxT[0] = (max[0] - ray.pos[0]) * ray.inv_dir[0]; // Calculate T distances to candidate planes
    }
    if (ray.pos[1] < min[1])
    {
        coord[1] = min[1];
        Inside = FALSE;
        if (uf(ray.inv_dir[1]))
            MaxT[1] = (min[1] - ray.pos[1]) * ray.inv_dir[1]; // Calculate T distances to candidate planes
    }
    else if (ray.pos[1] > max[1])
    {
        coord[1] = max[1];
        Inside = FALSE;
        if (uf(ray.inv_dir[1]))
            MaxT[1] = (max[1] - ray.pos[1]) * ray.inv_dir[1]; // Calculate T distances to candidate planes
    }
    if (ray.pos[2] < min[2])
    {
        coord[2] = min[2];
        Inside = FALSE;
        if (uf(ray.inv_dir[2]))
            MaxT[2] = (min[2] - ray.pos[2]) * ray.inv_dir[2]; // Calculate T distances to candidate planes
    }
    else if (ray.pos[2] > max[2])
    {
        coord[2] = max[2];
        Inside = FALSE;
        if (uf(ray.inv_dir[2]))
            MaxT[2] = (max[2] - ray.pos[2]) * ray.inv_dir[2]; // Calculate T distances to candidate planes
    }

    // Ray ray.pos inside bounding box
    if (Inside)
    {
        coord = ray.pos;
        return true;
    }

    // Get largest of the maxT's for final choice of intersection
    u32 WhichPlane = 0;
    if (MaxT[1] > MaxT[0])
        WhichPlane = 1;
    if (MaxT[2] > MaxT[WhichPlane])
        WhichPlane = 2;

    // Check final candidate actually inside box (if max < 0)
    if (uf(MaxT[WhichPlane]) & 0x80000000)
        return false;

    if (0 == WhichPlane)
    { // 1 & 2
        coord[1] = ray.pos[1] + MaxT[0] * ray.fwd_dir[1];
        if ((coord[1] < min[1]) || (coord[1] > max[1]))
            return false;
        coord[2] = ray.pos[2] + MaxT[0] * ray.fwd_dir[2];
        if ((coord[2] < min[2]) || (coord[2] > max[2]))
            return false;
        return true;
    }
    if (1 == WhichPlane)
    { // 0 & 2
        coord[0] = ray.pos[0] + MaxT[1] * ray.fwd_dir[0];
        if ((coord[0] < min[0]) || (coord[0] > max[0]))
            return false;
        coord[2] = ray.pos[2] + MaxT[1] * ray.fwd_dir[2];
        if ((coord[2] < min[2]) || (coord[2] > max[2]))
            return false;
        return true;
    }
    if (2 == WhichPlane)
    { // 0 & 1 //KRodin: это условие тоже всегда истинно. //-V547
        coord[0] = ray.pos[0] + MaxT[2] * ray.fwd_dir[0];
        if ((coord[0] < min[0]) || (coord[0] > max[0]))
            return false;
        coord[1] = ray.pos[1] + MaxT[2] * ray.fwd_dir[1];
        if ((coord[1] < min[1]) || (coord[1] > max[1]))
            return false;
        return true;
    }

    return false;
}

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

constexpr auto flt_plus_inf{std::numeric_limits<float>::infinity()};
constexpr float __declspec(align(16)) ps_cst_plus_inf[]{flt_plus_inf, flt_plus_inf, flt_plus_inf, flt_plus_inf};
constexpr float __declspec(align(16)) ps_cst_minus_inf[]{-flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf};

[[nodiscard]] constexpr bool isect_sse(const aabb_t& box, const ray_t& ray, float& dist)
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

    const bool ret = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax, lmin);

    storess(lmin, &dist);
    // storess	(lmax, &rs.t_far);

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

template <bool bUseSSE, bool bCull, bool bFirst, bool bNearest>
class XR_TRIVIAL alignas(16) ray_collider
{
public:
    COLLIDER* dest;
    TRI* tris;
    Fvector* verts;

    ray_t ray;
    float rRange;
    float rRange2;

    constexpr ray_collider() = default;

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

    constexpr void _init(COLLIDER* CL, Fvector* V, TRI* T, const Fvector& C, const Fvector& D, float R)
    {
        dest = CL;
        tris = T;
        verts = V;
        ray.pos.set(C);
        ray.inv_dir.set(1.f, 1.f, 1.f).div(D);
        ray.fwd_dir.set(D);
        rRange = R;
        rRange2 = R * R;
        if constexpr (!bUseSSE)
        {
            // for FPU - zero out inf
            if (_abs(D.x) > flt_eps)
            {
            }
            else
                ray.inv_dir.x = 0;
            if (_abs(D.y) > flt_eps)
            {
            }
            else
                ray.inv_dir.y = 0;
            if (_abs(D.z) > flt_eps)
            {
            }
            else
                ray.inv_dir.z = 0;
        }
    }

    // fpu
    [[nodiscard]] constexpr bool _box_fpu(const Fvector& bCenter, const Fvector& bExtents, Fvector& coord) const
    {
        Fbox BB;
        BB.min.sub(bCenter, bExtents);
        BB.max.add(bCenter, bExtents);

        return isect_fpu(BB.min, BB.max, ray, coord);
    }

    // sse
    [[nodiscard]] constexpr bool _box_sse(const Fvector& bCenter, const Fvector& bExtents, float& dist) const
    {
        constexpr auto init = [](const Fvector& bCenter, const Fvector& bExtents) -> aabb_t {
            __m128 CN = _mm_unpacklo_ps(_mm_load_ss(&bCenter.x), _mm_load_ss(&bCenter.y));
            CN = _mm_movelh_ps(CN, _mm_load_ss(&bCenter.z));

            __m128 EX = _mm_unpacklo_ps(_mm_load_ss(&bExtents.x), _mm_load_ss(&bExtents.y));
            EX = _mm_movelh_ps(EX, _mm_load_ss(&bExtents.z));

            aabb_t box;
            _mm_store_ps(reinterpret_cast<f32*>(&box.min), _mm_sub_ps(CN, EX));
            _mm_store_ps(reinterpret_cast<f32*>(&box.max), _mm_add_ps(CN, EX));
            return box;
        };

        return isect_sse(init(bCenter, bExtents), ray, dist);
    }

    [[nodiscard]] constexpr bool _tri(const u32* p, float& u, float& v, float& range) const
    {
        Fvector edge1, edge2, tvec, pvec, qvec;
        float det, inv_det;

        // find vectors for two edges sharing vert0
        Fvector& p0 = verts[p[0]];
        Fvector& p1 = verts[p[1]];
        Fvector& p2 = verts[p[2]];
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
        const auto id = gsl::narrow<s32>(prim);
        const auto& tri = tris[id];
        f32 u, v, r;

        if (!_tri(tri.verts, u, v, r))
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
                    R.id = id;
                    R.range = r;
                    R.u = u;
                    R.v = v;
                    R.verts[0] = verts[tri.verts[0]];
                    R.verts[1] = verts[tri.verts[1]];
                    R.verts[2] = verts[tri.verts[2]];
                    R.dummy = tri.dummy;
                    rRange = r;
                    rRange2 = r * r;
                }
            }
            else
            {
                dest->r_add(id, r, u, v, verts[tri.verts[0]], verts[tri.verts[1]], verts[tri.verts[2]], tri.dummy);
                rRange = r;
                rRange2 = r * r;
            }
        }
        else
        {
            dest->r_add(id, r, u, v, verts[tri.verts[0]], verts[tri.verts[1]], verts[tri.verts[2]], tri.dummy);
        }
    }

    constexpr void _stab(const AABBNoLeafNode* node)
    {
        // Should help
        _mm_prefetch(reinterpret_cast<const char*>(node->GetNeg()), _MM_HINT_NTA);

        // Actual ray/aabb test
        if constexpr (bUseSSE)
        {
            // use SSE
            f32 d;
            if (!_box_sse(*reinterpret_cast<const Fvector*>(&node->mAABB.mCenter), *reinterpret_cast<const Fvector*>(&node->mAABB.mExtents), d))
                return;
            if (d > rRange)
                return;
        }
        else
        {
            // use FPU
            Fvector P;
            if (!_box_fpu(*reinterpret_cast<const Fvector*>(&node->mAABB.mCenter), *reinterpret_cast<const Fvector*>(&node->mAABB.mExtents), P))
                return;
            if (P.distance_to_sqr(ray.pos) > rRange2)
                return;
        }

        // 1st chield
        if (node->HasPosLeaf())
            _prim(node->GetPosPrimitive());
        else
            _stab(node->GetPos());

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
            _stab(node->GetNeg());
    }
};
XR_TRIVIAL_ASSERT(ray_collider<true, true, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, true, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<true, true, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, true, false, false>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<true, false, false, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, true, false, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, true, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, true, false>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, false, true>);
XR_TRIVIAL_ASSERT(ray_collider<false, false, false, false>);
} // namespace

void COLLIDER::ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range)
{
    m_def->syncronize();

    // This should be smart_cast<>()/dynamic_cast<>(), but OpCoDe doesn't use our custom RTTI.
    // So we just rely on that `AABBOptimizedTree` starts at offset 0 inside `AABBNoLeafTree`.
    // OpCoDe itself uses C-style casts for downcasting, which is roughly the same.
    const AABBNoLeafTree* T = reinterpret_cast<const AABBNoLeafTree*>(m_def->tree->GetTree());
    const AABBNoLeafNode* N = T->GetNodes();

    r_clear();

    if (CPU::ID.hasSSE())
    {
        // SSE
        // Binary dispatcher
        if (ray_mode & OPT_CULL)
        {
            if (ray_mode & OPT_ONLYFIRST)
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<true, true, true, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<true, true, true, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
            else
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<true, true, false, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<true, true, false, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
        }
        else
        {
            if (ray_mode & OPT_ONLYFIRST)
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<true, false, true, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<true, false, true, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
            else
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<true, false, false, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<true, false, false, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
        }
    }
    else
    {
        // FPU
        // Binary dispatcher
        if (ray_mode & OPT_CULL)
        {
            if (ray_mode & OPT_ONLYFIRST)
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<false, true, true, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<false, true, true, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
            else
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<false, true, false, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<false, true, false, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
        }
        else
        {
            if (ray_mode & OPT_ONLYFIRST)
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<false, false, true, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<false, false, true, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
            else
            {
                if (ray_mode & OPT_ONLYNEAREST)
                {
                    ray_collider<false, false, false, true> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
                else
                {
                    ray_collider<false, false, false, false> RC;
                    RC._init(this, m_def->verts, m_def->tris, r_start, r_dir, r_range);
                    RC._stab(N);
                }
            }
        }
    }
}
