#include "stdafx.h"

#include "../../xrCDB/cl_intersect.h"
#include "../xrRender/du_cone.h"

static BOOL tri_vs_sphere_intersect(const Fvector& SC, float R, const Fvector& v0, const Fvector& v1, const Fvector& v2)
{
    Fvector e0, e1;
    return CDB::TestSphereTri(SC, R, v0, e0.sub(v1, v0), e1.sub(v2, v0));
}

BOOL CRenderTarget::enable_scissor(light* L) // true if intersects near plane
{
    // Near plane intersection
    BOOL near_intersect = FALSE;

    const Fmatrix& M = Device.mFullTransform;
    Fvector4 plane{-(M._14 + M._13), -(M._24 + M._23), -(M._34 + M._33), -(M._44 + M._43)};
    float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
    plane.mul(denom);
    Fplane P;
    P.n.set(plane.x, plane.y, plane.z);
    P.d = plane.w;
    float p_dist = P.classify(L->spatial.sphere.P) - L->spatial.sphere.R;
    near_intersect = (p_dist <= 0);

#ifdef DEBUG
    Fsphere S;
    S.set(L->spatial.sphere.P, L->spatial.sphere.R);
    dbg_spheres.emplace_back(S, L->color);
#endif

    // Scissor
    return near_intersect;
}
