// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../xr_3da/fmesh.h"
#include "FSkinned.h"
#include "SkeletonX.h"
#include "../xrRenderDX10/dx10BufferUtils.h"
#include "../../xr_3da/EnnumerateVertices.h"

#include <Utilities\FlexibleVertexFormat.h>

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

    void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index)
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

    void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
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

    void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, int index2, float w0, float w1)
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

    void set(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, int index2, int index3, float w0, float w1, float w2)
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
    VERIFY(lod_id >= 0 && lod_id < int(nSWI.count));
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
    void* _verts_ = data->pointer();
    inherited1::Load(N, data, dwFlags | VLOAD_NOVERTICES);
    RImplementation.shader_option_skinning(-1);
    _DuplicateIndices(data);
    vBase = 0;
    _Load_hw(*this, _verts_);
}

void CSkeletonX_ST::Load(const char* N, IReader* data, u32 dwFlags)
{
    _Load(N, data, vCount);
    void* _verts_ = data->pointer();
    inherited1::Load(N, data, dwFlags | VLOAD_NOVERTICES);
    RImplementation.shader_option_skinning(-1);
    _DuplicateIndices(data);
    vBase = 0;
    _Load_hw(*this, _verts_);
}

void CSkeletonX_ext::_Load_hw(Fvisual& V, void* _verts_)
{
    // Create HW VB in case this is possible
    //	BOOL	bSoft				= HW.Caps.geometry.bSoftware;
    //	u32		dwUsage				= /*D3DUSAGE_WRITEONLY |*/ (bSoft?D3DUSAGE_SOFTWAREPROCESSING:0);	// VB may be read by wallmarks code
    switch (RenderMode)
    {
    case RM_SKINNING_SOFT:
        // Msg					("skinning: software");
        V.rm_geom.create(vertRenderFVF, RImplementation.Vertex.Buffer(), V.p_rm_Indices);
        break;
    case RM_SINGLE:
    case RM_SKINNING_1B: {
        { //	Back up vertex data since we can't read vertex buffer in DX10
            // u32 size = V.vCount * sizeof(vertBoned1W);
            // u32 crc = crc32(_verts_, size);
            Vertices1W.create(V.vCount, (vertBoned1W*)_verts_);
        }

        u32 vStride = FVF::ComputeVertexSize(dwDecl_01W, 0);
        VERIFY(vStride == sizeof(vertHW_1W));
        //			BYTE*	bytes		= 0;
        VERIFY(NULL == V.p_rm_Vertices);

        //	TODO: DX10: Check for memory fragmentation
        vertHW_1W* dstOriginal = xr_alloc<vertHW_1W>(V.vCount);
        vertHW_1W* dst = dstOriginal;
        vertBoned1W* src = (vertBoned1W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, src->matrix * 3);
            dst++;
            src++;
        }
        // R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.p_rm_Vertices,0));
        R_CHK(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_01W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_2B: {
        { //	Back up vertex data since we can't read vertex buffer in DX10
            // u32 size = V.vCount * sizeof(vertBoned2W);
            // u32 crc = crc32(_verts_, size);
            Vertices2W.create(V.vCount, (vertBoned2W*)_verts_);
        }

        u32 vStride = FVF::ComputeVertexSize(dwDecl_2W, 0);
        VERIFY(vStride == sizeof(vertHW_2W));
        //			BYTE* bytes			= 0;
        VERIFY(NULL == V.p_rm_Vertices);

        // R_CHK				(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.p_rm_Vertices,0));
        // R_CHK				(V.p_rm_Vertices->Lock(0,0,(void**)&bytes,0));
        // vertHW_2W* dst		= (vertHW_2W*)bytes;
        // vertBoned2W* src	= (vertBoned2W*)_verts_;

        // for (u32 it=0; it<V.vCount; ++it)
        //{
        //	Fvector2	uv; uv.set(src->u,src->v);
        //	dst->set	(src->P,src->N,src->T,src->B,uv,int(src->matrix0)*3,int(src->matrix1)*3,src->w);
        //	dst++;		src++;
        // }
        // V.p_rm_Vertices->Unlock	();

        //	TODO: DX10: Check for memory fragmentation
        vertHW_2W* dstOriginal = xr_alloc<vertHW_2W>(V.vCount);
        vertHW_2W* dst = dstOriginal;
        vertBoned2W* src = (vertBoned2W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->matrix0) * 3, int(src->matrix1) * 3, src->w);
            dst++;
            src++;
        }
        R_CHK(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_2W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_3B: {
        { //	Back up vertex data since we can't read vertex buffer in DX10
            // u32 size = V.vCount * sizeof(vertBoned3W);
            // u32 crc = crc32(_verts_, size);
            Vertices3W.create(V.vCount, (vertBoned3W*)_verts_);
        }

        u32 vStride = FVF::ComputeVertexSize(dwDecl_3W, 0);
        VERIFY(vStride == sizeof(vertHW_3W));
        //			BYTE*	bytes			= 0;
        VERIFY(NULL == V.p_rm_Vertices);

        // R_CHK					(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.p_rm_Vertices,0));
        // R_CHK					(V.p_rm_Vertices->Lock(0,0,(void**)&bytes,0));
        // vertHW_3W* dst			= (vertHW_3W*)bytes;
        // vertBoned3W* src		= (vertBoned3W*)_verts_;

        // for (u32 it=0; it<V.vCount; ++it)
        //{
        //	Fvector2	uv; uv.set(src->u,src->v);
        //	dst->set	(src->P,src->N,src->T,src->B,uv,int(src->m[0])*3,int(src->m[1])*3,int(src->m[2])*3,src->w[0],src->w[1]);
        //	dst++;
        //	src++;
        // }
        // V.p_rm_Vertices->Unlock	();

        //	TODO: DX10: Check for memory fragmentation
        vertHW_3W* dstOriginal = xr_alloc<vertHW_3W>(V.vCount);
        vertHW_3W* dst = dstOriginal;
        vertBoned3W* src = (vertBoned3W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->m[0]) * 3, int(src->m[1]) * 3, int(src->m[2]) * 3, src->w[0], src->w[1]);
            dst++;
            src++;
        }
        R_CHK(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride));
        HW.stats_manager.increment_stats_vb(V.p_rm_Vertices);
        xr_free(dstOriginal);

        V.rm_geom.create(dwDecl_3W, V.p_rm_Vertices, V.p_rm_Indices);
    }
    break;
    case RM_SKINNING_4B: {
        { //	Back up vertex data since we can't read vertex buffer in DX10
            // u32 size = V.vCount * sizeof(vertBoned4W);
            // u32 crc = crc32(_verts_, size);
            Vertices4W.create(V.vCount, (vertBoned4W*)_verts_);
        }

        u32 vStride = FVF::ComputeVertexSize(dwDecl_4W, 0);
        VERIFY(vStride == sizeof(vertHW_4W));
        //			BYTE*	bytes			= 0;
        VERIFY(NULL == V.p_rm_Vertices);

        // R_CHK					(HW.pDevice->CreateVertexBuffer(V.vCount*vStride,dwUsage,0,D3DPOOL_MANAGED,&V.p_rm_Vertices,0));
        // R_CHK					(V.p_rm_Vertices->Lock(0,0,(void**)&bytes,0));
        // vertHW_4W* dst			= (vertHW_4W*)bytes;
        // vertBoned4W* src		= (vertBoned4W*)_verts_;

        // for (u32 it=0; it<V.vCount; ++it)
        //{
        //	Fvector2	uv; uv.set(src->u,src->v);
        //	dst->set	(src->P,src->N,src->T,src->B,uv,int(src->m[0])*3,int(src->m[1])*3,int(src->m[2])*3,int(src->m[3])*3,src->w[0],src->w[1],src->w[2]);
        //	dst++;
        //	src++;
        // }
        // V.p_rm_Vertices->Unlock	();

        //	TODO: DX10: Check for memory fragmentation
        vertHW_4W* dstOriginal = xr_alloc<vertHW_4W>(V.vCount);
        vertHW_4W* dst = dstOriginal;
        vertBoned4W* src = (vertBoned4W*)_verts_;
        for (u32 it = 0; it < V.vCount; it++)
        {
            Fvector2 uv;
            uv.set(src->u, src->v);
            dst->set(src->P, src->N, src->T, src->B, uv, int(src->m[0]) * 3, int(src->m[1]) * 3, int(src->m[2]) * 3, int(src->m[3]) * 3, src->w[0], src->w[1], src->w[2]);
            dst++;
            src++;
        }
        R_CHK(dx10BufferUtils::CreateVertexBuffer(&V.p_rm_Vertices, dstOriginal, V.vCount * vStride));
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
    VERIFY(Parent);

    for (u8 i = 0; i < vertex_type::bones_count; ++i)
    {
        if (v.get_bone_id(i) >= Parent->LL_BoneCount())
        {
            Msg("v.get_bone_id(i): %d, Parent->LL_BoneCount() %d ", v.get_bone_id(i), Parent->LL_BoneCount());
            Msg("&v: %p, &V: %p, indices: %p", &v, V, indices);
            Msg(" iBase: %d, iCount: %d, V->iBase %d, V->iCount %d, V->vBase: %d,  V->vCount  %d, vertex_idx: %d, idx: %d", iBase, iCount, V->iBase, V->iCount, V->vBase, V->vCount,
                vertex_idx, idx);
            Msg(" v.P: %s , v.N: %s, v.T: %s, v.B: %s", get_string(v.P).c_str(), get_string(v.N).c_str(), get_string(v.T).c_str(), get_string(v.B).c_str());
            Msg("Parent->dbg_name: %s ", Parent->dbg_name.c_str());
            FlushLog();
            FATAL("v.get_bone_id(i) >= Parent->LL_BoneCount()");
        }
    }
}
} // namespace
#endif

void CSkeletonX_ext::_CollectBoneFaces(Fvisual* V, u32 iBase, u32 iCount)
{
    u16* indices{*m_Indices};
    indices += iBase;

    {
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
            R_ASSERT2(0, "not implemented yet");
        }
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

BOOL CSkeletonX_ext::_PickBoneHW1W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
    return pick_bone<vertHW_1W>(Parent, r, dist, S, D, V, indices, faces);
}
BOOL CSkeletonX_ext::_PickBoneHW2W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
    return pick_bone<vertHW_2W>(Parent, r, dist, S, D, V, indices, faces);
}

BOOL CSkeletonX_ext::_PickBoneHW3W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
    return pick_bone<vertHW_3W>(Parent, r, dist, S, D, V, indices, faces);
}

BOOL CSkeletonX_ext::_PickBoneHW4W(IKinematics::pick_result& r, float dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
    return pick_bone<vertHW_4W>(Parent, r, dist, S, D, V, indices, faces);
}

BOOL CSkeletonX_ext::_PickBone(IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id, u32 iBase)
{
    VERIFY(Parent && (ChildIDX != u16(-1)));
    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    BOOL result{};
    u16* indices{*m_Indices};

    if (*Vertices1W)
        result = _PickBoneSoft1W(r, dist, start, dir, indices + iBase, *faces);
    else if (*Vertices2W)
        result = _PickBoneSoft2W(r, dist, start, dir, indices + iBase, *faces);
    else if (*Vertices3W)
        result = _PickBoneSoft3W(r, dist, start, dir, indices + iBase, *faces);
    else
    {
        VERIFY(!!(*Vertices4W));
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

void CSkeletonX_ext::_FillVerticesHW1W(const Fmatrix&, CSkeletonWallmark&, const Fvector&, float, Fvisual*, u16*, CBoneData::FacesVec&)
{
    R_ASSERT2(false, "Should use _FillVerticesSoft1W on R3!");
}

void CSkeletonX_ext::_FillVerticesHW2W(const Fmatrix&, CSkeletonWallmark&, const Fvector&, float, Fvisual*, u16*, CBoneData::FacesVec&)
{
    R_ASSERT2(false, "Should use _FillVerticesSoft2W on R3!");
}

void CSkeletonX_ext::_FillVerticesHW3W(const Fmatrix&, CSkeletonWallmark&, const Fvector&, float, Fvisual*, u16*, CBoneData::FacesVec&)
{
    R_ASSERT2(false, "Should use _FillVerticesSoft3W on R3!");
}

void CSkeletonX_ext::_FillVerticesHW4W(const Fmatrix&, CSkeletonWallmark&, const Fvector&, float, Fvisual*, u16*, CBoneData::FacesVec&)
{
    R_ASSERT2(false, "Should use _FillVerticesSoft4W on R3!");
}

void CSkeletonX_ext::_FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id, u32 iBase)
{
    VERIFY(Parent && (ChildIDX != u16(-1)));
    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    u16* indices{*m_Indices};

    if (*Vertices1W)
        _FillVerticesSoft1W(view, wm, normal, size, indices + iBase, *faces);
    else if (*Vertices2W)
        _FillVerticesSoft2W(view, wm, normal, size, indices + iBase, *faces);
    else if (*Vertices3W)
        _FillVerticesSoft3W(view, wm, normal, size, indices + iBase, *faces);
    else
    {
        VERIFY(!!(*Vertices4W));
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
void TEnumBoneVertices(vertex_buffer_type vertices, u16* indices, CBoneData::FacesVec& faces, SEnumVerticesCallback& C)
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
    VERIFY(Parent && (ChildIDX != u16(-1)));
    CBoneData& BD = Parent->LL_GetData(bone_id);
    CBoneData::FacesVec* faces = &BD.child_faces[ChildIDX];
    u16* indices{};
    //.	R_CHK				(V->pIndices->Lock(iBase,iCount,		(void**)&indices,	D3DLOCK_READONLY));

    VERIFY(*m_Indices);
    indices = *m_Indices;

    if (*Vertices1W)
        TEnumBoneVertices(Vertices1W, indices + iBase, *faces, C);
    else if (*Vertices2W)
        TEnumBoneVertices(Vertices2W, indices + iBase, *faces, C);
    else if (*Vertices3W)
        TEnumBoneVertices(Vertices3W, indices + iBase, *faces, C);
    else
    {
        VERIFY(!!(*Vertices4W));
        TEnumBoneVertices(Vertices4W, indices + iBase, *faces, C);
    }
}
