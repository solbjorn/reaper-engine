#include "stdafx.h"

#include "Frustum.h"

//////////////////////////////////////////////////////////////////////
void CFrustum::fplane::cache()
{
    if (positive(n.x))
    {
        if (positive(n.y))
        {
            if (positive(n.z))
                aabb_overlap_id = 0;
            else
                aabb_overlap_id = 1;
        }
        else
        {
            if (positive(n.z))
                aabb_overlap_id = 2;
            else
                aabb_overlap_id = 3;
        }
    }
    else
    {
        if (positive(n.y))
        {
            if (positive(n.z))
                aabb_overlap_id = 4;
            else
                aabb_overlap_id = 5;
        }
        else
        {
            if (positive(n.z))
                aabb_overlap_id = 6;
            else
                aabb_overlap_id = 7;
        }
    }
}

void CFrustum::_add(const Fplane& P)
{
    VERIFY(p_count < FRUSTUM_MAXPLANES);

    auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];
    plane.set(P);
    plane.cache();

    ++p_count;
}

void CFrustum::_add(const Fvector& P1, const Fvector& P2, const Fvector& P3)
{
    VERIFY(p_count < FRUSTUM_MAXPLANES);

    auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];
    plane.build_precise(P1, P2, P3);
    plane.cache();

    ++p_count;
}

//////////////////////////////////////////////////////////////////////

EFC_Visible CFrustum::testSphere(const Fvector& c, float r, u32& test_mask) const
{
    u32 bit{1};

    for (auto& plane : plane_span())
    {
        if (test_mask & bit)
        {
            const f32 cls = plane.classify(c);
            if (cls > r)
            {
                // none  - return
                test_mask = 0;
                return fcvNone;
            }

            if (_abs(cls) >= r)
                // fully - no need to test this plane
                test_mask &= ~bit;
        }

        bit <<= 1;
    }

    return test_mask ? fcvPartial : fcvFully;
}

bool CFrustum::testSphere_dirty(const Fvector& c, float r) const
{
    switch (p_count)
    {
    case 12:
        if (planes[11].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 11:
        if (planes[10].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 10:
        if (planes[9].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 9:
        if (planes[8].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 8:
        if (planes[7].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 7:
        if (planes[6].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 6:
        if (planes[5].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 5:
        if (planes[4].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 4:
        if (planes[3].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 3:
        if (planes[2].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 2:
        if (planes[1].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 1:
        if (planes[0].classify(c) > r)
            return FALSE;

        [[fallthrough]];
    case 0: break;
    default: NODEFAULT;
    }

    return TRUE;
}

EFC_Visible CFrustum::testAABB(const float* mM, u32& test_mask) const
{
    // go for trivial rejection or acceptance using "faster overlap test"
    u32 bit{1};

    for (auto& plane : plane_span())
    {
        if (test_mask & bit)
        {
            const EFC_Visible r = AABB_OverlapPlane(plane, mM);
            if (r == fcvFully)
            {
                // fully - no need to test this plane
                test_mask &= ~bit;
            }
            else if (r == fcvNone)
            {
                // none - return
                test_mask = 0;
                return fcvNone;
            }
        }

        bit <<= 1;
    }

    return test_mask ? fcvPartial : fcvFully;
}

EFC_Visible CFrustum::testSAABB(const Fvector& c, float r, const float* mM, u32& test_mask) const
{
    u32 bit{1};

    for (auto& plane : plane_span())
    {
        if (test_mask & bit)
        {
            const f32 cls = plane.classify(c);
            if (cls > r)
            {
                // none  - return
                test_mask = 0;
                return fcvNone;
            }

            if (_abs(cls) >= r)
            {
                // fully - no need to test this plane
                test_mask &= ~bit;
            }
            else
            {
                const EFC_Visible r2 = AABB_OverlapPlane(plane, mM);
                if (r2 == fcvFully)
                {
                    // fully - no need to test this plane
                    test_mask &= ~bit;
                }
                else if (r2 == fcvNone)
                {
                    // none - return
                    test_mask = 0;
                    return fcvNone;
                }
            }
        }

        bit <<= 1;
    }

    return test_mask ? fcvPartial : fcvFully;
}

//////////////////////////////////////////////////////////////////////

void CFrustum::CreateFromPoints(std::span<const Fvector> p, const Fvector& COP)
{
    const auto count = p.size();
    VERIFY(count < FRUSTUM_MAXPLANES);
    VERIFY(count >= 3);

    _clear();

    for (size_t i{1}; i < count; ++i)
        _add(COP, p[i - 1], p[i]);

    _add(COP, p[count - 1], p[0]);
}

void CFrustum::CreateFromPlanes(std::span<const Fplane> p)
{
    for (auto [dst, src] : std::views::zip(planes, p))
        dst.set(src);

    p_count = std::ssize(p);

    for (auto& plane : plane_span())
    {
        const f32 denom = 1.0f / plane.n.magnitude(); // Get magnitude of Vector

        plane.n.x *= denom;
        plane.n.y *= denom;
        plane.n.z *= denom;
        plane.d *= denom;
        plane.cache();
    }
}

void CFrustum::CreateFromPortal(sPoly& poly, const Fvector& vBase, const Fmatrix& mFullXFORM)
{
    Fplane P;
    P.build_precise(poly[0], poly[1], poly[2]);

    if (poly.size() > 6)
    {
        SimplifyPoly_AABB(poly, P);
        P.build_precise(poly[0], poly[1], poly[2]);
    }

    // Check plane orientation relative to viewer
    // and reverse if needed
    if (P.classify(vBase) < 0)
    {
        std::ranges::reverse(poly);
        P.build_precise(poly[0], poly[1], poly[2]);
    }

    // Base creation
    CreateFromPoints(poly, vBase);

    // Near clipping plane
    _add(P);

    // Far clipping plane
    const Fmatrix& M = mFullXFORM;
    P.n.x = -(M._14 - M._13);
    P.n.y = -(M._24 - M._23);
    P.n.z = -(M._34 - M._33);
    P.d = -(M._44 - M._43);

    const f32 denom = 1.0f / P.n.magnitude();
    P.n.x *= denom;
    P.n.y *= denom;
    P.n.z *= denom;
    P.d *= denom;

    _add(P);
}

void CFrustum::SimplifyPoly_AABB(sPoly& poly, const Fplane& plane)
{
    Fvector y{0.0f, 1.0f, 0.0f};
    if (_abs(plane.n.y) > 0.99f)
        y.set(1.0f, 0.0f, 0.0f);

    Fvector right, up;
    right.crossproduct(y, plane.n);
    up.crossproduct(plane.n, right);

    Fmatrix mView;
    mView.build_camera_dir(poly[0], plane.n, up);

    // Project and find extents
    Fvector2 min{flt_max, flt_max};
    Fvector2 max{flt_min, flt_min};

    for (const auto& v : poly)
    {
        Fvector2 tmp;
        mView.transform_tiny32(tmp, v);

        min.min(tmp.x, tmp.y);
        max.max(tmp.x, tmp.y);
    }

    // Build other 2 points and inverse project
    const Fvector2 p1{min.x, max.y};
    const Fvector2 p2{max.x, min.y};

    Fmatrix mInv;
    mInv.invert(mView);
    poly.clear();

    mInv.transform_tiny23(poly.last(), min);
    poly.inc();
    mInv.transform_tiny23(poly.last(), p1);
    poly.inc();
    mInv.transform_tiny23(poly.last(), max);
    poly.inc();
    mInv.transform_tiny23(poly.last(), p2);
    poly.inc();
}

sPoly* CFrustum::ClipPoly(sPoly& S, sPoly& D) const
{
    sPoly* src = &D;
    sPoly* dest = &S;

    for (auto& P : plane_span())
    {
        // cache plane and swap lists
        std::swap(src, dest);
        dest->clear();

        // classify all points relative to plane #i
        float cls[FRUSTUM_SAFE];

        for (auto [j, pt] : xr::views_enumerate(std::as_const(*src)))
            cls[j] = P.classify(pt);

        // clip everything to this plane
        cls[src->size()] = cls[0];
        src->push_back((*src)[0]);

        Fvector D2;
        float denum, t;

        for (gsl::index j{}; j < src->size() - 1; ++j)
        {
            const auto& sj = (*src)[gsl::narrow_cast<typename sPoly::size_type>(j)];
            const auto& sjn = (*src)[gsl::narrow_cast<typename sPoly::size_type>(j + 1)];

            if (sj.similar(sjn, EPS_S))
                continue;

            if (negative(cls[j]))
            {
                dest->push_back(sj);
                if (positive(cls[j + 1]))
                {
                    // segment intersects plane
                    D2.sub(sjn, sj);
                    denum = P.n.dotproduct(D2);
                    if (!fis_zero(denum))
                    {
                        t = -cls[j] / denum; // VERIFY(t<=1.f && t>=0);
                        dest->last().mad(sj, D2, t);
                        dest->inc();
                    }
                }
            }
            else
            {
                // J - outside
                if (negative(cls[j + 1]))
                {
                    // J+1  - inside
                    // segment intersects plane
                    D2.sub(sjn, sj);
                    denum = P.n.dotproduct(D2);
                    if (!fis_zero(denum))
                    {
                        t = -cls[j] / denum; // VERIFY(t<=1.f && t>=0);
                        dest->last().mad(sj, D2, t);
                        dest->inc();
                    }
                }
            }
        }

        // here we end up with complete polygon in 'dest' which is inside plane #i
        if (dest->size() < 3)
            return nullptr;
    }

    return dest;
}

void CFrustum::CreateFromMatrix(const Fmatrix& M, u32 mask)
{
    VERIFY(_valid(M));
    p_count = 0;

    // Left clipping plane
    if (mask & FRUSTUM_P_LEFT)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 + M._11);
        plane.n.y = -(M._24 + M._21);
        plane.n.z = -(M._34 + M._31);
        plane.d = -(M._44 + M._41);

        ++p_count;
    }

    // Right clipping plane
    if (mask & FRUSTUM_P_RIGHT)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 - M._11);
        plane.n.y = -(M._24 - M._21);
        plane.n.z = -(M._34 - M._31);
        plane.d = -(M._44 - M._41);

        ++p_count;
    }

    // Top clipping plane
    if (mask & FRUSTUM_P_TOP)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 - M._12);
        plane.n.y = -(M._24 - M._22);
        plane.n.z = -(M._34 - M._32);
        plane.d = -(M._44 - M._42);

        ++p_count;
    }

    // Bottom clipping plane
    if (mask & FRUSTUM_P_BOTTOM)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 + M._12);
        plane.n.y = -(M._24 + M._22);
        plane.n.z = -(M._34 + M._32);
        plane.d = -(M._44 + M._42);

        ++p_count;
    }

    // Far clipping plane
    if (mask & FRUSTUM_P_FAR)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 - M._13);
        plane.n.y = -(M._24 - M._23);
        plane.n.z = -(M._34 - M._33);
        plane.d = -(M._44 - M._43);

        ++p_count;
    }

    // Near clipping plane
    if (mask & FRUSTUM_P_NEAR)
    {
        auto& plane = planes[gsl::narrow_cast<size_t>(p_count)];

        plane.n.x = -(M._14 + M._13);
        plane.n.y = -(M._24 + M._23);
        plane.n.z = -(M._34 + M._33);
        plane.d = -(M._44 + M._43);

        ++p_count;
    }

    for (auto& plane : plane_span())
    {
        const f32 denom = 1.0f / plane.n.magnitude(); // Get magnitude of Vector

        plane.n.x *= denom;
        plane.n.y *= denom;
        plane.n.z *= denom;
        plane.d *= denom;
        plane.cache();
    }
}
