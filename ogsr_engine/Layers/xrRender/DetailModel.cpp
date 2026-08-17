#include "stdafx.h"

#include "DetailModel.h"

#include "xrStripify.h"

CDetail::~CDetail() = default;

void CDetail::Unload()
{
    vertices.clear();
    indices.clear();

    shader.destroy();
}

void CDetail::transfer(Fmatrix& mXform, std::span<fvfVertexOut> vDest, u32 C, std::span<u16> iDest, u32 iOffset, f32 du, f32 dv) const
{
    const auto vsize = vertices.size();
    XR_ASSERT(vDest.size() >= vsize && iDest.size() >= indices.size(), "", vDest.size(), vsize, iDest.size(), indices.size());

    // Transfer vertices
    for (auto [dst, src] : std::views::zip(vDest, vertices))
    {
        mXform.transform_tiny(dst.P, src.P);
        dst.C = C;
        dst.u = src.u + du;
        dst.v = src.v + dv;
    }

    // Transfer indices
    // If this passes, `iOffset + src` can never overflow due to index validation in CDetail::Load()
    XR_ASSERT(iOffset + vsize <= std::numeric_limits<u16>::max(), "", iOffset, vsize);

    for (auto [dst, src] : std::views::zip(iDest, indices))
        dst = gsl::narrow_cast<u16>(iOffset + src);
}

void CDetail::Load(IReader* S)
{
    // Shader
    string256 fnT, fnS;
    S->r_stringZ(fnS, sizeof(fnS));
    S->r_stringZ(fnT, sizeof(fnT));
    shader.create(fnS, fnT);

    // Params
    m_Flags.assign(S->r_u32());
    m_fMinScale = S->r_float();
    m_fMaxScale = S->r_float();

    const auto number_vertices = S->r_u32();
    const auto number_indices = S->r_u32();
    XR_ASSERT(xr::is_aligned(number_indices, 3u), "", fnS, fnT);

    // Vertices
    const auto vertices_size = XR_ASSERT_VAL(gsl::index{number_vertices} * gsl::index{sizeof(fvfVertexIn)} <= S->elapsed());
    vertices.assign_range(std::span{static_cast<const fvfVertexIn*>(S->pointer()), number_vertices});
    S->advance(vertices_size);

    // Indices
    const auto indices_size = XR_ASSERT_VAL(gsl::index{number_indices} * gsl::index{sizeof(u16)} <= S->elapsed());
    indices.assign_range(std::span{static_cast<const u16*>(S->pointer()), number_indices});
    S->advance(indices_size);

    // Validate indices
    for (auto [idx, ind] : std::views::enumerate(indices))
        XR_ASSERT(ind < number_vertices, "", fnS, fnT, idx);

    // Calc BB & SphereRadius
    bv_bb.invalidate();

    for (auto& vert : vertices)
        bv_bb.modify(vert.P);

    bv_bb.getsphere(bv_sphere.P, bv_sphere.R);

    Optimize();
}

void CDetail::Optimize()
{
    const s32 cache = HW.Caps.geometry.dwVertexCache;

    // Stripify
    xr_vector<u16> vec_indices{indices};
    xr_vector<u16> vec_permute(vertices.size());

    const auto vt_old = xrSimulate(vec_indices, cache);
    xrStripify(vec_indices, vec_permute, cache, 0);

    if (xrSimulate(vec_indices, cache) >= vt_old)
        return;

    // Copy faces
    indices.assign_range(vec_indices);

    // Permute vertices
    xr_vector<fvfVertexIn> verts;
    verts.assign_range(vertices);

    for (auto [vert, perm] : std::views::zip(vertices, vec_permute))
        vert = verts[perm];
}
