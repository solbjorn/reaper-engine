#include "stdafx.h"

#include "xrCDB.h"

namespace CDB
{
gsl::index Collector::VPack(const Fvector& V, float eps)
{
    for (auto [id, vert] : xr::views_enumerate(std::as_const(verts)))
    {
        if (vert.similar(V, eps))
            return id;
    }

    verts.emplace_back(V);

    return std::ssize(verts) - 1;
}

void Collector::add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                           u32 dummy // misc
)
{
    const auto vs = gsl::narrow<u32>(verts.size());

    verts.emplace_back(v0);
    verts.emplace_back(v1);
    verts.emplace_back(v2);
    faces.emplace_back(vs, vs + 1, vs + 2, dummy);
}

void Collector::add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector)
{
    const auto vs = gsl::narrow<u32>(verts.size());

    verts.emplace_back(v0);
    verts.emplace_back(v1);
    verts.emplace_back(v2);
    faces.emplace_back(vs, vs + 1, vs + 2, material, sector);
}

void Collector::add_face_packed(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                u16 material, u16 sector, // misc
                                float eps)
{
    faces.emplace_back(gsl::narrow<u32>(VPack(v0, eps)), gsl::narrow<u32>(VPack(v1, eps)), gsl::narrow<u32>(VPack(v2, eps)), material, sector);
}

void Collector::add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                  u32 dummy, float eps)
{
    faces.emplace_back(gsl::narrow<u32>(VPack(v0, eps)), gsl::narrow<u32>(VPack(v1, eps)), gsl::narrow<u32>(VPack(v2, eps)), dummy);
}

namespace
{
struct XR_TRIVIAL alignas(16) edge
{
    u32 face_id;
    u32 edge_id;
    u32 vertex_id0;
    u32 vertex_id1;

    [[maybe_unused]] constexpr edge() = default;
    constexpr explicit edge(u32 fid, u32 eid, u32 vid0, u32 vid1) : face_id{fid}, edge_id{eid}, vertex_id0{vid0}, vertex_id1{vid1} {}

    constexpr edge(const edge& that) { xr_memcpy16(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr edge(edge&&) = default;
#else
    constexpr edge(edge&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr edge& operator=(const edge& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr edge& operator=(edge&&) = default;
#else
    constexpr edge& operator=(edge&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif
};
static_assert(std::is_same_v<decltype(edge::vertex_id0), std::remove_all_extents_t<decltype(TRI::verts)>>);
static_assert(std::is_same_v<decltype(edge::vertex_id1), std::remove_all_extents_t<decltype(TRI::verts)>>);
XR_TRIVIAL_ASSERT(edge);
} // namespace

void Collector::calc_adjacency(xr_vector<u32>& dest) const
{
    const auto edge_count = faces.size() * 3;
    xr_inlined_vector<edge, 24> edges;
    edges.reserve(edge_count);

    for (auto [id, face] : xr::views_enumerate(faces))
    {
        const auto face_id = gsl::narrow<u32>(id);

        auto& e1 = edges.emplace_back(face_id, 0, face.verts[0], face.verts[1]);
        if (e1.vertex_id0 > e1.vertex_id1)
            std::swap(e1.vertex_id0, e1.vertex_id1);

        auto& e2 = edges.emplace_back(face_id, 1, face.verts[1], face.verts[2]);
        if (e2.vertex_id0 > e2.vertex_id1)
            std::swap(e2.vertex_id0, e2.vertex_id1);

        auto& e3 = edges.emplace_back(face_id, 2, face.verts[2], face.verts[0]);
        if (e3.vertex_id0 > e3.vertex_id1)
            std::swap(e3.vertex_id0, e3.vertex_id1);
    }

    std::ranges::sort(edges, [](const edge& edge0, const edge& edge1) {
        if (edge0.vertex_id0 < edge1.vertex_id0)
            return true;

        if (edge1.vertex_id0 < edge0.vertex_id0)
            return false;

        if (edge0.vertex_id1 < edge1.vertex_id1)
            return true;

        if (edge1.vertex_id1 < edge0.vertex_id1)
            return false;

        return edge0.face_id < edge1.face_id;
    });

    dest.assign(edge_count, std::numeric_limits<u32>::max());

    auto I2 = edges.cbegin();
    auto E2 = edges.cend();
    for (; I2 != E2; ++I2)
    {
        if (I2 + 1 == E2)
            continue;

        auto& I = *I2;
        auto& J = *(I2 + 1);

        if (I.vertex_id0 != J.vertex_id0)
            continue;

        if (I.vertex_id1 != J.vertex_id1)
            continue;

        dest[I.face_id * 3 + I.edge_id] = J.face_id;
        dest[J.face_id * 3 + J.edge_id] = I.face_id;
    }
}

CollectorPacked::CollectorPacked(const Fbox& bb, gsl::index apx_vertices, gsl::index apx_faces)
{
    // Params
    VMscale.set(bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z);
    VMmin.set(bb.min);

    VMeps.set(VMscale.x / clpMX / 2, VMscale.y / clpMY / 2, VMscale.z / clpMZ / 2);
    VMeps.x = std::min(VMeps.x, EPS_L);
    VMeps.y = std::min(VMeps.y, EPS_L);
    VMeps.z = std::min(VMeps.z, EPS_L);

    // Preallocate memory
    verts.reserve(gsl::narrow_cast<size_t>(apx_vertices));
    faces.reserve(gsl::narrow_cast<size_t>(apx_faces));

    const gsl::index _size = (clpMX + 1) * (clpMY + 1) * (clpMZ + 1);
    const auto _average = gsl::narrow_cast<size_t>((apx_vertices / _size) / 2);

    for (gsl::index ix{}; ix < clpMX + 1; ++ix)
    {
        for (gsl::index iy{}; iy < clpMY + 1; ++iy)
        {
            for (gsl::index iz{}; iz < clpMZ + 1; ++iz)
                VM[ix][iy][iz].reserve(_average);
        }
    }
}

void CollectorPacked::add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                               u16 material, u16 sector // misc
)
{
    faces.emplace_back(VPack(v0), VPack(v1), VPack(v2), material, sector);
}

void CollectorPacked::add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                 u32 dummy // misc
)
{
    faces.emplace_back(VPack(v0), VPack(v1), VPack(v2), dummy);
}

u32 CollectorPacked::VPack(const Fvector& V)
{
    auto ix = gsl::narrow_cast<gsl::index>(std::floor((V.x - VMmin.x) / VMscale.x * clpMX));
    auto iy = gsl::narrow_cast<gsl::index>(std::floor((V.y - VMmin.y) / VMscale.y * clpMY));
    auto iz = gsl::narrow_cast<gsl::index>(std::floor((V.z - VMmin.z) / VMscale.z * clpMZ));

    clamp(ix, 0z, clpMX);
    clamp(iy, 0z, clpMY);
    clamp(iz, 0z, clpMZ);

    for (auto it : VM[ix][iy][iz])
    {
        if (verts[it].similar(V))
            return it;
    }

    const auto P = gsl::narrow<u32>(verts.size());
    verts.emplace_back(V);
    VM[ix][iy][iz].emplace_back(P);

    auto ixE = gsl::narrow_cast<gsl::index>(std::floor((V.x + VMeps.x - VMmin.x) / VMscale.x * clpMX));
    auto iyE = gsl::narrow_cast<gsl::index>(std::floor((V.y + VMeps.y - VMmin.y) / VMscale.y * clpMY));
    auto izE = gsl::narrow_cast<gsl::index>(std::floor((V.z + VMeps.z - VMmin.z) / VMscale.z * clpMZ));

    clamp(ixE, 0z, clpMX);
    clamp(iyE, 0z, clpMY);
    clamp(izE, 0z, clpMZ);

    if (ixE != ix)
        VM[ixE][iy][iz].emplace_back(P);
    if (iyE != iy)
        VM[ix][iyE][iz].emplace_back(P);
    if (izE != iz)
        VM[ix][iy][izE].emplace_back(P);
    if (ixE != ix && iyE != iy)
        VM[ixE][iyE][iz].emplace_back(P);
    if (ixE != ix && izE != iz)
        VM[ixE][iy][izE].emplace_back(P);
    if (iyE != iy && izE != iz)
        VM[ix][iyE][izE].emplace_back(P);
    if (ixE != ix && iyE != iy && izE != iz)
        VM[ixE][iyE][izE].emplace_back(P);

    return P;
}

void CollectorPacked::clear()
{
    verts.clear();
    faces.clear();

    for (gsl::index _x{}; _x < clpMX + 1; ++_x)
    {
        for (gsl::index _y{}; _y < clpMY + 1; ++_y)
        {
            for (gsl::index _z{}; _z < clpMZ + 1; ++_z)
                VM[_x][_y][_z].clear();
        }
    }
}
} // namespace CDB
