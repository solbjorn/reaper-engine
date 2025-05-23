#include "stdafx.h"

namespace CDB
{
u32 Collector::VPack(const Fvector& V, float eps)
{
    for (auto I = verts.begin(); I != verts.end(); ++I)
        if (I->similar(V, eps))
            return u32(I - verts.begin());

    verts.emplace_back(V);
    return verts.size() - 1;
}

void Collector::add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                           u32 dummy // misc
)
{
    TRI T;
    T.verts[0] = verts.size();
    T.verts[1] = verts.size() + 1;
    T.verts[2] = verts.size() + 2;
    T.dummy = dummy;

    verts.emplace_back(v0);
    verts.emplace_back(v1);
    verts.emplace_back(v2);
    faces.emplace_back(T);
}

void Collector::add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector)
{
    TRI T;
    T.verts[0] = verts.size();
    T.verts[1] = verts.size() + 1;
    T.verts[2] = verts.size() + 2;
    T.material = material;
    T.sector = sector;

    verts.emplace_back(v0);
    verts.emplace_back(v1);
    verts.emplace_back(v2);
    faces.emplace_back(T);
}

void Collector::add_face_packed(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                u16 material, u16 sector, // misc
                                float eps)
{
    TRI T;
    T.verts[0] = VPack(v0, eps);
    T.verts[1] = VPack(v1, eps);
    T.verts[2] = VPack(v2, eps);
    T.material = material;
    T.sector = sector;
    faces.emplace_back(T);
}

void Collector::add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                  u32 dummy, float eps)
{
    TRI T;
    T.verts[0] = VPack(v0, eps);
    T.verts[1] = VPack(v1, eps);
    T.verts[2] = VPack(v2, eps);
    T.dummy = dummy;
    faces.emplace_back(std::move(T));
}

struct alignas(16) edge
{
    u32 face_id;
    u32 edge_id;
    u32 vertex_id0;
    u32 vertex_id1;

    constexpr inline edge() = default;
    constexpr inline edge(const edge& e) { xr_memcpy16(this, &e); }
    constexpr inline edge& operator=(const edge& e)
    {
        xr_memcpy16(this, &e);
        return *this;
    }
};
static_assert(std::is_same_v<decltype(edge::vertex_id0), std::remove_all_extents_t<decltype(TRI::verts)>>);
static_assert(std::is_same_v<decltype(edge::vertex_id1), std::remove_all_extents_t<decltype(TRI::verts)>>);

void Collector::calc_adjacency(xr_vector<u32>& dest) const
{
    const auto edge_count = faces.size() * 3;
    u8* buf = (u8*)_alloca(edge_count * sizeof(edge) + 16);
    edge* const edges = reinterpret_cast<edge*>((reinterpret_cast<size_t>(buf) + 15) & ~size_t(0xf));
    edge* i = edges;

    const auto B = faces.cbegin();
    auto I = B, E = faces.cend();
    for (; I != E; ++I)
    {
        const u32 face_id = u32(I - B);

        (*i).face_id = face_id;
        (*i).edge_id = 0;
        (*i).vertex_id0 = (*I).verts[0];
        (*i).vertex_id1 = (*I).verts[1];
        if ((*i).vertex_id0 > (*i).vertex_id1)
            std::swap((*i).vertex_id0, (*i).vertex_id1);
        ++i;

        (*i).face_id = face_id;
        (*i).edge_id = 1;
        (*i).vertex_id0 = (*I).verts[1];
        (*i).vertex_id1 = (*I).verts[2];
        if ((*i).vertex_id0 > (*i).vertex_id1)
            std::swap((*i).vertex_id0, (*i).vertex_id1);
        ++i;

        (*i).face_id = face_id;
        (*i).edge_id = 2;
        (*i).vertex_id0 = (*I).verts[2];
        (*i).vertex_id1 = (*I).verts[0];
        if ((*i).vertex_id0 > (*i).vertex_id1)
            std::swap((*i).vertex_id0, (*i).vertex_id1);
        ++i;
    }

    std::sort(edges, edges + edge_count, [](const edge& edge0, const edge& edge1) {
        if (edge0.vertex_id0 < edge1.vertex_id0)
            return (true);

        if (edge1.vertex_id0 < edge0.vertex_id0)
            return (false);

        if (edge0.vertex_id1 < edge1.vertex_id1)
            return (true);

        if (edge1.vertex_id1 < edge0.vertex_id1)
            return (false);

        return (edge0.face_id < edge1.face_id);
    });

    dest.assign(edge_count, u32(-1));

    {
        edge *I2 = edges, *J;
        edge* E2 = edges + edge_count;
        for (; I2 != E2; ++I2)
        {
            if (I2 + 1 == E2)
                continue;

            J = I2 + 1;

            if ((*I2).vertex_id0 != (*J).vertex_id0)
                continue;

            if ((*I2).vertex_id1 != (*J).vertex_id1)
                continue;

            dest[(*I2).face_id * 3 + (*I2).edge_id] = (*J).face_id;
            dest[(*J).face_id * 3 + (*J).edge_id] = (*I2).face_id;
        }
    }
}
IC BOOL similar(TRI& T1, TRI& T2)
{
    if ((T1.verts[0] == T2.verts[0]) && (T1.verts[1] == T2.verts[1]) && (T1.verts[2] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    if ((T1.verts[0] == T2.verts[0]) && (T1.verts[2] == T2.verts[1]) && (T1.verts[1] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    if ((T1.verts[2] == T2.verts[0]) && (T1.verts[0] == T2.verts[1]) && (T1.verts[1] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    if ((T1.verts[2] == T2.verts[0]) && (T1.verts[1] == T2.verts[1]) && (T1.verts[0] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    if ((T1.verts[1] == T2.verts[0]) && (T1.verts[0] == T2.verts[1]) && (T1.verts[2] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    if ((T1.verts[1] == T2.verts[0]) && (T1.verts[2] == T2.verts[1]) && (T1.verts[0] == T2.verts[2]) && (T1.dummy == T2.dummy))
        return TRUE;
    return FALSE;
}
void Collector::remove_duplicate_T()
{
    for (u32 f = 0; f < faces.size(); f++)
    {
        for (u32 t = f + 1; t < faces.size();)
        {
            if (t == f)
                continue;
            TRI& T1 = faces[f];
            TRI& T2 = faces[t];
            if (similar(T1, T2))
            {
                faces[t] = faces.back();
                faces.pop_back();
            }
            else
            {
                t++;
            }
        }
    }
}

CollectorPacked::CollectorPacked(const Fbox& bb, int apx_vertices, int apx_faces)
{
    // Params
    VMscale.set(bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z);
    VMmin.set(bb.min);
    VMeps.set(VMscale.x / clpMX / 2, VMscale.y / clpMY / 2, VMscale.z / clpMZ / 2);
    VMeps.x = (VMeps.x < EPS_L) ? VMeps.x : EPS_L;
    VMeps.y = (VMeps.y < EPS_L) ? VMeps.y : EPS_L;
    VMeps.z = (VMeps.z < EPS_L) ? VMeps.z : EPS_L;

    // Preallocate memory
    verts.reserve(apx_vertices);
    faces.reserve(apx_faces);

    int _size = (clpMX + 1) * (clpMY + 1) * (clpMZ + 1);
    int _average = (apx_vertices / _size) / 2;
    for (u32 ix = 0; ix < clpMX + 1; ix++)
        for (u32 iy = 0; iy < clpMY + 1; iy++)
            for (u32 iz = 0; iz < clpMZ + 1; iz++)
                VM[ix][iy][iz].reserve(_average);
}

void CollectorPacked::add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                               u16 material, u16 sector // misc
)
{
    TRI T;
    T.verts[0] = VPack(v0);
    T.verts[1] = VPack(v1);
    T.verts[2] = VPack(v2);
    T.material = material;
    T.sector = sector;
    faces.emplace_back(T);
}

void CollectorPacked::add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                 u32 dummy // misc
)
{
    TRI T;
    T.verts[0] = VPack(v0);
    T.verts[1] = VPack(v1);
    T.verts[2] = VPack(v2);
    T.dummy = dummy;
    faces.emplace_back(T);
}

u32 CollectorPacked::VPack(const Fvector& V)
{
    u32 P = 0xffffffff;

    u32 ix, iy, iz;
    ix = iFloor(float(V.x - VMmin.x) / VMscale.x * clpMX);
    iy = iFloor(float(V.y - VMmin.y) / VMscale.y * clpMY);
    iz = iFloor(float(V.z - VMmin.z) / VMscale.z * clpMZ);

    //		R_ASSERT(ix<=clpMX && iy<=clpMY && iz<=clpMZ);
    clamp(ix, (u32)0, clpMX);
    clamp(iy, (u32)0, clpMY);
    clamp(iz, (u32)0, clpMZ);

    {
        DWORDList* vl = &(VM[ix][iy][iz]);
        for (u32 it : *vl)
            if (verts[it].similar(V))
            {
                P = it;
                break;
            }
    }
    if (0xffffffff == P)
    {
        P = verts.size();
        verts.emplace_back(V);

        VM[ix][iy][iz].emplace_back(P);

        u32 ixE, iyE, izE;
        ixE = iFloor(float(V.x + VMeps.x - VMmin.x) / VMscale.x * clpMX);
        iyE = iFloor(float(V.y + VMeps.y - VMmin.y) / VMscale.y * clpMY);
        izE = iFloor(float(V.z + VMeps.z - VMmin.z) / VMscale.z * clpMZ);

        //			R_ASSERT(ixE<=clpMX && iyE<=clpMY && izE<=clpMZ);
        clamp(ixE, (u32)0, clpMX);
        clamp(iyE, (u32)0, clpMY);
        clamp(izE, (u32)0, clpMZ);

        if (ixE != ix)
            VM[ixE][iy][iz].emplace_back(P);
        if (iyE != iy)
            VM[ix][iyE][iz].emplace_back(P);
        if (izE != iz)
            VM[ix][iy][izE].emplace_back(P);
        if ((ixE != ix) && (iyE != iy))
            VM[ixE][iyE][iz].emplace_back(P);
        if ((ixE != ix) && (izE != iz))
            VM[ixE][iy][izE].emplace_back(P);
        if ((iyE != iy) && (izE != iz))
            VM[ix][iyE][izE].emplace_back(P);
        if ((ixE != ix) && (iyE != iy) && (izE != iz))
            VM[ixE][iyE][izE].emplace_back(P);
    }
    return P;
}

void CollectorPacked::clear()
{
    verts.clear();
    faces.clear();
    for (u32 _x = 0; _x <= clpMX; _x++)
        for (u32 _y = 0; _y <= clpMY; _y++)
            for (u32 _z = 0; _z <= clpMZ; _z++)
                VM[_x][_y][_z].clear();
}
}; // namespace CDB
