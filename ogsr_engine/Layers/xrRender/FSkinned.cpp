// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FSkinned.h"

#include "../../xr_3da/fmesh.h"
#include "SkeletonX.h"
#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../../xr_3da/EnnumerateVertices.h"

#include <Utilities/FlexibleVertexFormat.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace
{
[[nodiscard]] constexpr u8 q_N(float v)
{
    int _v = clampr(iFloor((v + 1.f) * 127.5f), 0, 255);
    return u8(_v);
}

constexpr D3DVERTEXELEMENT9 dwDecl_01W[] // 36bytes
    {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // : P						: 2	: -12..+12
     {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // : N, w=index(RC, 0..1)	: 1	:  -1..+1
     {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0}, // : T						: 1	:  -1..+1
     {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0}, // : B						: 1	:  -1..+1
     {0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // : tc						: 1	: -16..+16
     D3DDECL_END()};

struct vertHW_1W
{
    float _P[4];
    u32 _N_I;
    u32 _T;
    u32 _B;
    float _tc[2];

    void set(const Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, const Fvector2& tc, int index)
    {
        N.normalize_safe();
        T.normalize_safe();
        B.normalize_safe();
        _P[0] = P.x;
        _P[1] = P.y;
        _P[2] = P.z;
        _P[3] = 1.0f;
        _N_I = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(index));
        _T = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
        _B = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
        _tc[0] = tc.x;
        _tc[1] = tc.y;
    }
};

constexpr D3DVERTEXELEMENT9 dwDecl_2W[] // 44bytes
    {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // : p					: 2	: -12..+12
     {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // : n.xyz, w = weight	: 1	:  -1..+1, w=0..1
     {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0}, // : T						: 1	:  -1..+1
     {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0}, // : B						: 1	:  -1..+1
     {0, 28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // : xy(tc), zw(indices): 2	: -16..+16, zw[0..32767]
     D3DDECL_END()};

struct vertHW_2W
{
    float _P[4];
    u32 _N_w;
    u32 _T;
    u32 _B;
    float _tc_i[4];

    void set(const Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, const Fvector2& tc, int index0, int index1, float w)
    {
        N.normalize_safe();
        T.normalize_safe();
        B.normalize_safe();
        _P[0] = P.x;
        _P[1] = P.y;
        _P[2] = P.z;
        _P[3] = 1.0f;
        _N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w * 255.f + .5f), 0, 255)));
        _T = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
        _B = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
        _tc_i[0] = tc.x;
        _tc_i[1] = tc.y;
        _tc_i[2] = s16(index0);
        _tc_i[3] = s16(index1);
    }
};

constexpr D3DVERTEXELEMENT9 dwDecl_3W[] // 44bytes
    {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // : p					: 2	: -12..+12
     {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // : n.xyz, w = weight0	: 1	:  -1..+1, w=0..1
     {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0}, // : T.xyz, w = weight1	: 1	:  -1..+1, w=0..1
     {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0}, // : B.xyz,	w = index2	: 1	:  -1..+1, w=0..255
     {0, 28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // : xy(tc), zw(indices): 2	: -16..+16, zw[0..32767]
     D3DDECL_END()};

struct vertHW_3W
{
    float _P[4];
    u32 _N_w;
    u32 _T_w;
    u32 _B_i;
    float _tc_i[4];

    void set(const Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, const Fvector2& tc, int index0, int index1, int index2, float w0, float w1)
    {
        N.normalize_safe();
        T.normalize_safe();
        B.normalize_safe();
        _P[0] = P.x;
        _P[1] = P.y;
        _P[2] = P.z;
        _P[3] = 1.0f;
        _N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w0 * 255.f + .5f), 0, 255)));
        _T_w = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), u8(clampr(iFloor(w1 * 255.f + .5f), 0, 255)));
        _B_i = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), u8(index2));
        _tc_i[0] = tc.x;
        _tc_i[1] = tc.y;
        _tc_i[2] = s16(index0);
        _tc_i[3] = s16(index1);
    }
};

constexpr D3DVERTEXELEMENT9 dwDecl_4W[] // 40bytes
    {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // : p					: 2	: -12..+12
     {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // : n.xyz, w = weight0	: 1	:  -1..+1, w=0..1
     {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0}, // : T.xyz, w = weight1	: 1	:  -1..+1, w=0..1
     {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0}, // : B.xyz,	w = weight2	: 1	:  -1..+1, w=0..1
     {0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // : xy(tc)				: 2	: -16..+16
     {0, 36, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1}, // : indices			: 1	:  0..255
     D3DDECL_END()};

struct vertHW_4W
{
    float _P[4];
    u32 _N_w;
    u32 _T_w;
    u32 _B_w;
    float _tc[2];
    u32 _i;

    void set(const Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, const Fvector2& tc, int index0, int index1, int index2, int index3, float w0, float w1,
             float w2)
    {
        N.normalize_safe();
        T.normalize_safe();
        B.normalize_safe();
        _P[0] = P.x;
        _P[1] = P.y;
        _P[2] = P.z;
        _P[3] = 1.0f;
        _N_w = color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w0 * 255.f + .5f), 0, 255)));
        _T_w = color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), u8(clampr(iFloor(w1 * 255.f + .5f), 0, 255)));
        _B_w = color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), u8(clampr(iFloor(w2 * 255.f + .5f), 0, 255)));
        _tc[0] = tc.x;
        _tc[1] = tc.y;
        _i = color_rgba(u8(index0), u8(index1), u8(index2), u8(index3));
    }
};
} // namespace

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Copy(dxRender_Visual* V)
{
    inherited1::Copy(V);
    CSkeletonX_PM* X = (CSkeletonX_PM*)(V);
    _Copy((CSkeletonX*)X);
}

void CSkeletonX_ST::Copy(dxRender_Visual* P)
{
    inherited1::Copy(P);
    CSkeletonX_ST* X = (CSkeletonX_ST*)P;
    _Copy((CSkeletonX*)X);
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Render(CBackend& cmd_list, float LOD, bool)
{
    int lod_id = inherited1::last_lod;
    if (LOD >= 0.f)
    {
        clamp(LOD, 0.f, 1.f);
        lod_id = iFloor((1.f - LOD) * float(nSWI.count - 1) + 0.5f);
        inherited1::last_lod = lod_id;
    }

    XR_ASSERT(lod_id >= 0 && lod_id < s64{nSWI.count}, "", lod_id, nSWI.count);

    FSlideWindow& SW = nSWI.sw[lod_id];
    _Render(cmd_list, rm_geom, SW.num_verts, SW.offset, SW.num_tris);
}

void CSkeletonX_ST::Render(CBackend& cmd_list, float, bool) { _Render(cmd_list, rm_geom, vCount, 0, dwPrimitives); }

//////////////////////////////////////////////////////////////////////
void CSkeletonX_PM::Release() { inherited1::Release(); }
void CSkeletonX_ST::Release() { inherited1::Release(); }
//////////////////////////////////////////////////////////////////////

void CSkeletonX_PM::Load(const char* N, IReader* data, u32 dwFlags)
{
    _Load(N, data, vCount);
    const void* _verts_ = data->pointer();
    inherited1::Load(N, data, dwFlags | VLOAD_NOVERTICES);
    RImplementation.shader_option_skinning(-1);
    _DuplicateIndices(data);
    vBase = 0;
    _Load_hw(*this, _verts_);
}

void CSkeletonX_ST::Load(const char* N, IReader* data, u32 dwFlags)
{
    _Load(N, data, vCount);
    const void* _verts_ = data->pointer();
    inherited1::Load(N, data, dwFlags | VLOAD_NOVERTICES);
    RImplementation.shader_option_skinning(-1);
    _DuplicateIndices(data);
    vBase = 0;
    _Load_hw(*this, _verts_);
}

void CSkeletonX_ext::_Load_hw(Fvisual& V, const void* _verts_)
{
    // Create HW VB in case this is possible
    switch (RenderMode)
    {
    case RM_SKINNING_SOFT: V.rm_geom.create(vertRenderFVF, RImplementation.Vertex.Buffer(), V.p_rm_Indices); break;
    case RM_SINGLE:
    case RM_SKINNING_1B: {
        // Back up vertex data since we can't read vertex buffer in DX10
        Vertices1W.create(V.vCount, (const vertBoned1W*)_verts_);

        constexpr auto vStride = FVF::ComputeVertexSize(dwDecl_01W, 0);
        static_assert(vStride == sizeof(vertHW_1W));

        XR_ASSERT(V.p_rm_Vertices == nullptr);

        //	TODO: DX10: Check for memory fragmentation
        vertHW_1W* dstOriginal = xr_alloc<vertHW_1W>(V.vCount);
        vertHW_1W* dst = dstOriginal;
        const vertBoned1W* src = (const vertBoned1W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, src->matrix * 3);
            dst++;
            src++;
        }

        XR_ASSERT(xr::hr(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride)));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_01W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_2B: {
        // Back up vertex data since we can't read vertex buffer in DX10
        Vertices2W.create(V.vCount, (const vertBoned2W*)_verts_);

        constexpr auto vStride = FVF::ComputeVertexSize(dwDecl_2W, 0);
        static_assert(vStride == sizeof(vertHW_2W));

        XR_ASSERT(V.p_rm_Vertices == nullptr);

        //	TODO: DX10: Check for memory fragmentation
        vertHW_2W* dstOriginal = xr_alloc<vertHW_2W>(V.vCount);
        vertHW_2W* dst = dstOriginal;
        const vertBoned2W* src = (const vertBoned2W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->matrix0) * 3, int(src->matrix1) * 3, src->w);
            dst++;
            src++;
        }

        XR_ASSERT(xr::hr(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride)));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_2W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_3B: {
        // Back up vertex data since we can't read vertex buffer in DX10
        Vertices3W.create(V.vCount, (const vertBoned3W*)_verts_);

        constexpr auto vStride = FVF::ComputeVertexSize(dwDecl_3W, 0);
        static_assert(vStride == sizeof(vertHW_3W));

        XR_ASSERT(V.p_rm_Vertices == nullptr);

        //	TODO: DX10: Check for memory fragmentation
        vertHW_3W* dstOriginal = xr_alloc<vertHW_3W>(V.vCount);
        vertHW_3W* dst = dstOriginal;
        const vertBoned3W* src = (const vertBoned3W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->m[0]) * 3, int(src->m[1]) * 3, int(src->m[2]) * 3, src->w[0], src->w[1]);
            dst++;
            src++;
        }

        XR_ASSERT(xr::hr(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride)));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_3W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_4B: {
        // Back up vertex data since we can't read vertex buffer in DX10
        Vertices4W.create(V.vCount, (const vertBoned4W*)_verts_);

        constexpr auto vStride = FVF::ComputeVertexSize(dwDecl_4W, 0);
        static_assert(vStride == sizeof(vertHW_4W));

        XR_ASSERT(V.p_rm_Vertices == nullptr);

        //	TODO: DX10: Check for memory fragmentation
        vertHW_4W* dstOriginal = xr_alloc<vertHW_4W>(V.vCount);
        vertHW_4W* dst = dstOriginal;
        const vertBoned4W* src = (const vertBoned4W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->m[0]) * 3, int(src->m[1]) * 3, int(src->m[2]) * 3, int(src->m[3]) * 3, src->w[0], src->w[1],
                     src->w[2]);
            dst++;
            src++;
        }

        XR_ASSERT(xr::hr(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride)));

        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_4W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    }
}

//-----------------------------------------------------------------------------------------------------
// Wallmarks
//-----------------------------------------------------------------------------------------------------

#ifdef DEBUG
namespace
{
template <typename vertex_type>
void verify_vertex(const vertex_type& v, const Fvisual* V, const CKinematics* Parent, u32 iBase, u32 iCount, const u16* indices, u32 vertex_idx, u32 idx)
{
    XR_ASSERT(Parent != nullptr);

    for (u8 i = 0; i < vertex_type::bones_count; ++i)
    {
        if (v.get_bone_id(i) >= Parent->LL_BoneCount())
        {
            Msg("v.get_bone_id(i): {}, Parent->LL_BoneCount() {} ", v.get_bone_id(i), Parent->LL_BoneCount());
            Msg("&v: {}, &V: {}, indices: {}", &v, V, indices);
            Msg(" iBase: {}, iCount: {}, V->iBase {}, V->iCount {}, V->vBase: {},  V->vCount  {}, vertex_idx: {}, idx: {}", iBase, iCount, V->iBase, V->iCount,
                V->vBase, V->vCount, vertex_idx, idx);
            Msg(" v.P: {} , v.N: {}, v.T: {}, v.B: {}", get_string(v.P), get_string(v.N), get_string(v.T), get_string(v.B));
            Msg("Parent->dbg_name: {} ", Parent->dbg_name);
            FlushLog();

            XR_ASSERT(v.get_bone_id(i) < Parent->LL_BoneCount(), "", i);
        }
    }
}
} // namespace
#endif

void CSkeletonX_ext::_CollectBoneFaces(Fvisual* V, u32 iBase, u32 iCount)
{
    u16* indices{*m_Indices};
    indices += iBase;

    if (*Vertices1W)
    {
        vertBoned1W* vertices = *Vertices1W;
        for (u32 idx = 0; idx < iCount; idx++)
        {
            vertBoned1W& v = vertices[V->vBase + indices[idx]];

#ifdef DEBUG
            verify_vertex(v, V, Parent, iBase, iCount, indices, V->vBase + indices[idx], idx);
#endif

            CBoneData& BD = Parent->LL_GetData((u16)v.matrix);
            BD.AppendFace(ChildIDX, (u16)(idx / 3));
        }
    }
    else if (*Vertices2W)
    {
        vertBoned2W* vertices = *Vertices2W;
        for (u32 idx = 0; idx < iCount; ++idx)
        {
            vertBoned2W& v = vertices[V->vBase + indices[idx]];

#ifdef DEBUG
            verify_vertex(v, V, Parent, iBase, iCount, indices, V->vBase + indices[idx], idx);
#endif

            CBoneData& BD0 = Parent->LL_GetData((u16)v.matrix0);
            BD0.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD1 = Parent->LL_GetData((u16)v.matrix1);
            BD1.AppendFace(ChildIDX, (u16)(idx / 3));
        }
    }
    else if (*Vertices3W)
    {
        vertBoned3W* vertices = *Vertices3W;
        for (u32 idx = 0; idx < iCount; ++idx)
        {
            vertBoned3W& v = vertices[V->vBase + indices[idx]];

#ifdef DEBUG
            verify_vertex(v, V, Parent, iBase, iCount, indices, V->vBase + indices[idx], idx);
#endif

            CBoneData& BD0 = Parent->LL_GetData((u16)v.m[0]);
            BD0.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD1 = Parent->LL_GetData((u16)v.m[1]);
            BD1.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD2 = Parent->LL_GetData((u16)v.m[2]);
            BD2.AppendFace(ChildIDX, (u16)(idx / 3));
        }
    }
    else if (*Vertices4W)
    {
        vertBoned4W* vertices = *Vertices4W;
        for (u32 idx = 0; idx < iCount; ++idx)
        {
            vertBoned4W& v = vertices[V->vBase + indices[idx]];

#ifdef DEBUG
            verify_vertex(v, V, Parent, iBase, iCount, indices, V->vBase + indices[idx], idx);
#endif

            CBoneData& BD0 = Parent->LL_GetData((u16)v.m[0]);
            BD0.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD1 = Parent->LL_GetData((u16)v.m[1]);
            BD1.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD2 = Parent->LL_GetData((u16)v.m[2]);
            BD2.AppendFace(ChildIDX, (u16)(idx / 3));
            CBoneData& BD3 = Parent->LL_GetData((u16)v.m[3]);
            BD3.AppendFace(ChildIDX, (u16)(idx / 3));
        }
    }
    else
    {
        XR_PANIC("invalid model");
    }
}

void CSkeletonX_ST::AfterLoad(CKinematics* parent, u16 child_idx)
{
    inherited2::AfterLoad(parent, child_idx);
    inherited2::_CollectBoneFaces(this, iBase, iCount);
}

void CSkeletonX_PM::AfterLoad(CKinematics* parent, u16 child_idx)
{
    inherited2::AfterLoad(parent, child_idx);
    FSlideWindow& SW = nSWI.sw[0]; // max LOD
    inherited2::_CollectBoneFaces(this, iBase + SW.offset, SW.num_tris * 3);
}

BOOL CSkeletonX_ext::_PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id, u32 iBase)
{
    XR_TRACY_ZONE_SCOPED();
    XR_ASSERT(Parent != nullptr && ChildIDX != std::numeric_limits<u16>::max(), "", ChildIDX);

    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    BOOL result{};
    u16* indices{*m_Indices};

    if (*Vertices1W)
    {
        result = _PickBoneSoft1W(r, dist, start, dir, indices + iBase, *faces);
    }
    else if (*Vertices2W)
    {
        result = _PickBoneSoft2W(r, dist, start, dir, indices + iBase, *faces);
    }
    else if (*Vertices3W)
    {
        result = _PickBoneSoft3W(r, dist, start, dir, indices + iBase, *faces);
    }
    else
    {
        XR_ASSERT(*Vertices4W != nullptr);
        result = _PickBoneSoft4W(r, dist, start, dir, indices + iBase, *faces);
    }

    return result;
}

BOOL CSkeletonX_ST::PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
    return inherited2::_PickBone(r, dist, start, dir, bone_id, iBase);
}

BOOL CSkeletonX_PM::PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
    FSlideWindow& SW = nSWI.sw[0];
    return inherited2::_PickBone(r, dist, start, dir, bone_id, iBase + SW.offset);
}

void CSkeletonX_ST::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) { inherited2::_EnumBoneVertices(C, bone_id, iBase); }

void CSkeletonX_PM::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id)
{
    FSlideWindow& SW = nSWI.sw[0];
    inherited2::_EnumBoneVertices(C, bone_id, iBase + SW.offset);
}

void CSkeletonX_ext::_FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id, u32 iBase)
{
    XR_ASSERT(Parent != nullptr && ChildIDX != std::numeric_limits<u16>::max(), "", ChildIDX);

    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    u16* indices{*m_Indices};

    if (*Vertices1W)
    {
        _FillVerticesSoft1W(view, wm, normal, size, indices + iBase, *faces);
    }
    else if (*Vertices2W)
    {
        _FillVerticesSoft2W(view, wm, normal, size, indices + iBase, *faces);
    }
    else if (*Vertices3W)
    {
        _FillVerticesSoft3W(view, wm, normal, size, indices + iBase, *faces);
    }
    else
    {
        XR_ASSERT(*Vertices4W != nullptr);
        _FillVerticesSoft4W(view, wm, normal, size, indices + iBase, *faces);
    }
}

void CSkeletonX_ST::FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)
{
    inherited2::_FillVertices(view, wm, normal, size, bone_id, iBase);
}

void CSkeletonX_PM::FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)
{
    FSlideWindow& SW = nSWI.sw[0];
    inherited2::_FillVertices(view, wm, normal, size, bone_id, iBase + SW.offset);
}

namespace
{
template <typename vertex_buffer_type>
void TEnumBoneVertices(vertex_buffer_type vertices, const u16* indices, CBoneData::FacesVec& faces, SEnumVerticesCallback& C)
{
    for (u16 face : faces)
    {
        u32 idx = face * 3;
        for (u32 k = 0; k < 3; k++)
        {
            Fvector P;
            vertices[indices[idx + k]].get_pos(P);
            C(P);
        }
    }
}
} // namespace

void CSkeletonX_ext::_EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id, u32 iBase) const
{
    XR_ASSERT(Parent != nullptr && ChildIDX != std::numeric_limits<u16>::max(), "", ChildIDX);

    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    const u16* indices = XR_ASSERT_VAL(*m_Indices != nullptr);

    if (*Vertices1W)
    {
        TEnumBoneVertices(Vertices1W, indices + iBase, *faces, C);
    }
    else if (*Vertices2W)
    {
        TEnumBoneVertices(Vertices2W, indices + iBase, *faces, C);
    }
    else if (*Vertices3W)
    {
        TEnumBoneVertices(Vertices3W, indices + iBase, *faces, C);
    }
    else
    {
        XR_ASSERT(*Vertices4W != nullptr);
        TEnumBoneVertices(Vertices4W, indices + iBase, *faces, C);
    }
}
