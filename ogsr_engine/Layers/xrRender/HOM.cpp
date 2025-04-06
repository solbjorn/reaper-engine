// HOM.cpp: implementation of the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HOM.h"
#include "occRasterizer.h"
#include "../../xr_3da/GameFont.h"

#include "dxRenderDeviceRender.h"

#include <oneapi/tbb/parallel_for.h>
#include "xr_task.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOM::CHOM()
{
    tg = &xr_task_group_get();
    bEnabled = FALSE;

    m_pModel = nullptr;
    m_pTris = nullptr;

#ifdef DEBUG
    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);
#endif
}

CHOM::~CHOM()
{
    tg->cancel_put();

#ifdef DEBUG
    Device.seqRender.Remove(this);
#endif
}

struct HOM_poly
{
    Fvector v1, v2, v3;
    u32 flags;
};

IC float Area(Fvector& v0, Fvector& v1, Fvector& v2)
{
    const float e1 = v0.distance_to(v1);
    const float e2 = v0.distance_to(v2);
    const float e3 = v1.distance_to(v2);

    const float p = (e1 + e2 + e3) / 2.f;
    return _sqrt(p * (p - e1) * (p - e2) * (p - e3));
}

void CHOM::Load()
{
    // Find and open file
    string_path fName;
    FS.update_path(fName, "$level$", "level.hom");
    if (!FS.exist(fName))
    {
        Msg(" WARNING: Occlusion map '%s' not found.", fName);
        return;
    }
    Msg("* Loading HOM: %s", fName);

    IReader* fs = FS.r_open(fName);

    // Load tris and merge them
    CDB::Collector CL;
    {
        IReader* S = fs->open_chunk(1);
        const auto begin = static_cast<HOM_poly*>(S->pointer());
        const auto end = static_cast<HOM_poly*>(S->end());
        for (HOM_poly* poly = begin; poly != end; ++poly)
        {
            CL.add_face_packed_D(poly->v1, poly->v2, poly->v3, poly->flags, 0.01f);
        }
        S->close();
    }

    // Determine adjacency
    xr_vector<u32> adjacency;
    CL.calc_adjacency(adjacency);

    // Create RASTER-triangles
    m_pTris = xr_alloc<occTri>(CL.getTS());

    tbb::parallel_for(tbb::blocked_range<size_t>(0, CL.getTS()), [&](const auto& range) {
        for (size_t it = range.begin(); it != range.end(); ++it)
        {
            const CDB::TRI& clT = CL.getT()[it];
            occTri& rT = m_pTris[it];
            Fvector& v0 = CL.getV()[clT.verts[0]];
            Fvector& v1 = CL.getV()[clT.verts[1]];
            Fvector& v2 = CL.getV()[clT.verts[2]];
            rT.adjacent[0] = (0xffffffff == adjacency[3 * it + 0]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 0]);
            rT.adjacent[1] = (0xffffffff == adjacency[3 * it + 1]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 1]);
            rT.adjacent[2] = (0xffffffff == adjacency[3 * it + 2]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 2]);
            rT.flags = clT.dummy;
            rT.area = Area(v0, v1, v2);

            if (rT.area < EPS_L)
                Msg("! Invalid HOM triangle (%f,%f,%f)-(%f,%f,%f)-(%f,%f,%f)", VPUSH(v0), VPUSH(v1), VPUSH(v2));

            rT.plane.build(v0, v1, v2);
            rT.skip = 0;
            rT.center.add(v0, v1).add(v2).div(3.f);
        }
    });

    // Create AABB-tree
    m_pModel = xr_new<CDB::MODEL>();
    m_pModel->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()));

    bEnabled = TRUE;
    FS.r_close(fs);
}

void CHOM::Unload()
{
    xr_delete(m_pModel);
    xr_free(m_pTris);
    bEnabled = FALSE;
}

void CHOM::Render_DB(CFrustum& base)
{
    // Update projection matrices on every frame to ensure valid HOM culling
    float view_dim = occ_dim_0;
    const Fmatrix m_viewport = {view_dim / 2.f,         0.0f, 0.0f, 0.0f, 0.0f, -view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, view_dim / 2.f + 0 + 0,
                                view_dim / 2.f + 0 + 0, 0.0f, 1.0f};
    constexpr Fmatrix m_viewport_01 = {1.f / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -1.f / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.f / 2.f + 0 + 0, 1.f / 2.f + 0 + 0, 0.0f, 1.0f};
    m_xform.mul(m_viewport, Device.mFullTransform);
    m_xform_01.mul(m_viewport_01, Device.mFullTransform);

    // Query DB
    xrc.frustum_query(0, m_pModel, base);
    if (0 == xrc.r_count())
        return;

    // Prepare
    auto it = xrc.r_get()->begin();
    auto end = xrc.r_get()->end();

    Fvector COP = Device.vCameraPosition;
    end = std::remove_if(it, end, [this](const CDB::RESULT& _1) {
        const occTri& T = m_pTris[_1.id];
        return T.skip > Device.dwFrame;
    });
    std::sort(it, end, [this, &COP](const CDB::RESULT& _1, const CDB::RESULT& _2) {
        const occTri& t0 = m_pTris[_1.id];
        const occTri& t1 = m_pTris[_2.id];
        return COP.distance_to_sqr(t0.center) < COP.distance_to_sqr(t1.center);
    });

    // Build frustum with near plane only
    CFrustum clip;
    clip.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_NEAR);
    sPoly src, dst;
    u32 _frame = Device.dwFrame;
#ifdef DEBUG
    tris_in_frame = xrc.r_count();
    tris_in_frame_visible = 0;
#endif

    // Perfrom selection, sorting, culling
    for (auto& it : *xrc.r_get())
    {
        // Control skipping
        occTri& T = m_pTris[it.id];
        u32 next = _frame + ::Random.randI(3, 10);

        // Test for good occluder - should be improved :)
        if (!(T.flags || (T.plane.classify(COP) > 0)))
        {
            T.skip = next;
            continue;
        }

        // Access to triangle vertices
        CDB::TRI& t = m_pModel->get_tris()[it.id];
        Fvector* v = m_pModel->get_verts();
        src.clear();
        dst.clear();
        src.push_back(v[t.verts[0]]);
        src.push_back(v[t.verts[1]]);
        src.push_back(v[t.verts[2]]);
        sPoly* P = clip.ClipPoly(src, dst);
        if (nullptr == P)
        {
            T.skip = next;
            continue;
        }

        // XForm and Rasterize
#ifdef DEBUG
        tris_in_frame_visible++;
#endif
        u32 pixels = 0;
        int limit = int(P->size()) - 1;
        for (int v2 = 1; v2 < limit; v2++)
        {
            m_xform.transform(T.raster[0], (*P)[0]);
            m_xform.transform(T.raster[1], (*P)[v2 + 0]);
            m_xform.transform(T.raster[2], (*P)[v2 + 1]);
            pixels += Raster.rasterize(&T);
        }
        if (0 == pixels)
        {
            T.skip = next;
            continue;
        }
    }
}

void CHOM::Render(CFrustum& base)
{
    if (!bEnabled || ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return;

    Device.Statistic->RenderCALC_HOM.Begin();
    Raster.clear();
    Render_DB(base);
    Raster.propagade();
    Device.Statistic->RenderCALC_HOM.End();
}

void CHOM::run_async()
{
    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return;

    tg->run([this] {
        CFrustum ViewBase;
        ViewBase.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);
        Enable();
        Render(ViewBase);
    });
}

void CHOM::wait_async() const { tg->wait(); }

static ICF BOOL xform_b0(Fvector2& min, Fvector2& max, float& minz, const Fmatrix& X, float _x, float _y, float _z)
{
    const float z = _x * X._13 + _y * X._23 + _z * X._33 + X._43;
    if (z < EPS)
        return TRUE;
    const float iw = 1.f / (_x * X._14 + _y * X._24 + _z * X._34 + X._44);
    min.x = max.x = (_x * X._11 + _y * X._21 + _z * X._31 + X._41) * iw;
    min.y = max.y = (_x * X._12 + _y * X._22 + _z * X._32 + X._42) * iw;
    minz = 0.f + z * iw;
    return FALSE;
}

static ICF BOOL xform_b1(Fvector2& min, Fvector2& max, float& minz, const Fmatrix& X, float _x, float _y, float _z)
{
    const float z = _x * X._13 + _y * X._23 + _z * X._33 + X._43;
    if (z < EPS)
        return TRUE;
    const float iw = 1.f / (_x * X._14 + _y * X._24 + _z * X._34 + X._44);
    float t = (_x * X._11 + _y * X._21 + _z * X._31 + X._41) * iw;
    if (t < min.x)
        min.x = t;
    else if (t > max.x)
        max.x = t;
    t = (_x * X._12 + _y * X._22 + _z * X._32 + X._42) * iw;
    if (t < min.y)
        min.y = t;
    else if (t > max.y)
        max.y = t;
    t = 0.f + z * iw;
    if (t < minz)
        minz = t;
    return FALSE;
}

static IC BOOL _visible(const Fbox& B, const Fmatrix& m_xform_01)
{
    // Find min/max points of xformed-box
    Fvector2 min, max;
    float z;
    if (xform_b0(min, max, z, m_xform_01, B.min.x, B.min.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.min.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.min.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.min.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.max.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.max.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.max.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.max.y, B.min.z))
        return TRUE;
    return Raster.test(min.x, min.y, max.x, max.y, z);
}

BOOL CHOM::visible(const Fbox3& B) const
{
    if (!bEnabled || ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return TRUE;
    if (B.contains(Device.vCameraPosition))
        return TRUE;
    return _visible(B, m_xform_01);
}

BOOL CHOM::visible(const Fbox2& B, float depth) const
{
    if (!bEnabled || ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return TRUE;
    return Raster.test(B.min.x, B.min.y, B.max.x, B.max.y, depth);
}

BOOL CHOM::visible(vis_data& vis) const
{
    if (vis.hom_tested == Device.dwFrame)
        return vis.hom_frame > vis.hom_tested;

    if (Device.dwFrame < vis.hom_frame)
        return TRUE; // not at this time :)

    if (!bEnabled || ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return TRUE; // return - everything visible

    // Now, the test time comes
    // 0. The object was hidden, and we must prove that each frame - test |
    // frame-old, tested-new, hom_res = false;
    // 1. The object was visible, but we must to re-check it - test | frame-new,
    // tested-???, hom_res = true;
    // 2. New object slides into view - delay test| frame-old, tested-old,
    // hom_res = ???;
    u32 frame_current = Device.dwFrame;
    // u32 frame_prev = frame_current-1;

#ifdef DEBUG
    Device.Statistic->RenderCALC_HOM.Begin();
#endif

    BOOL result = _visible(vis.box, m_xform_01);
    if (result)
        // visible - delay next test
        vis.hom_frame = frame_current + ::Random.randI(5 * 2, 5 * 5);
    else
        // hidden - shedule to next frame
        vis.hom_frame = frame_current;
    vis.hom_tested = frame_current;

#ifdef DEBUG
    Device.Statistic->RenderCALC_HOM.End();
#endif

    return result;
}

BOOL CHOM::visible(const sPoly& P) const
{
    if (!bEnabled || ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
        return TRUE;

    // Find min/max points of xformed-box
    Fvector2 min, max;
    float z;

    if (xform_b0(min, max, z, m_xform_01, P.front().x, P.front().y, P.front().z))
        return TRUE;
    for (u32 it = 1; it < P.size(); it++)
        if (xform_b1(min, max, z, m_xform_01, P[it].x, P[it].y, P[it].z))
            return TRUE;
    return Raster.test(min.x, min.y, max.x, max.y, z);
}

void CHOM::Disable() { bEnabled = FALSE; }
void CHOM::Enable() { bEnabled = m_pModel ? TRUE : FALSE; }

#ifdef DEBUG
void CHOM::OnRender()
{
    Raster.on_dbg_render();

    if (psDeviceFlags.is(rsOcclusionDraw))
    {
        if (m_pModel)
        {
            DEFINE_VECTOR(FVF::L, LVec, LVecIt);
            static LVec poly;
            poly.resize(m_pModel->get_tris_count() * 3);
            static LVec line;
            line.resize(m_pModel->get_tris_count() * 6);
            for (int it = 0; it < m_pModel->get_tris_count(); it++)
            {
                CDB::TRI* T = m_pModel->get_tris() + it;
                Fvector* verts = m_pModel->get_verts();
                poly[it * 3 + 0].set(*(verts + T->verts[0]), 0x80FFFFFF);
                poly[it * 3 + 1].set(*(verts + T->verts[1]), 0x80FFFFFF);
                poly[it * 3 + 2].set(*(verts + T->verts[2]), 0x80FFFFFF);
                line[it * 6 + 0].set(*(verts + T->verts[0]), 0xFFFFFFFF);
                line[it * 6 + 1].set(*(verts + T->verts[1]), 0xFFFFFFFF);
                line[it * 6 + 2].set(*(verts + T->verts[1]), 0xFFFFFFFF);
                line[it * 6 + 3].set(*(verts + T->verts[2]), 0xFFFFFFFF);
                line[it * 6 + 4].set(*(verts + T->verts[2]), 0xFFFFFFFF);
                line[it * 6 + 5].set(*(verts + T->verts[0]), 0xFFFFFFFF);
            }
            RCache.set_xform_world(Fidentity);
            // draw solid
            Device.SetNearer(TRUE);
            RCache.set_Shader(RImplementation.m_SelectionShader);
            RCache.set_c("tfactor", float(color_get_R(0x80FFFFFF)) / 255.f, float(color_get_G(0x80FFFFFF)) / 255.f, float(color_get_B(0x80FFFFFF)) / 255.f,
                         float(color_get_A(0x80FFFFFF)) / 255.f);
            RCache.dbg_Draw(D3DPT_TRIANGLELIST, &*poly.begin(), poly.size() / 3);
            Device.SetNearer(FALSE);
            // draw wire
            if (bDebug)
            {
                RImplementation.rmNear(RCache);
            }
            else
            {
                Device.SetNearer(TRUE);
            }
            RCache.set_Shader(RImplementation.m_SelectionShader);
            RCache.set_c("tfactor", 1.f, 1.f, 1.f, 1.f);
            RCache.dbg_Draw(D3DPT_LINELIST, &*line.begin(), line.size() / 2);
            if (bDebug)
                RImplementation.rmNormal(RCache);
            else
                Device.SetNearer(FALSE);
        }
    }
}
void CHOM::stats()
{
    if (m_pModel)
    {
        CGameFont& F = *Device.Statistic->Font();
        F.OutNext(" **** HOM-occ ****");
        F.OutNext("  visible:  %2d", tris_in_frame_visible);
        F.OutNext("  frustum:  %2d", tris_in_frame);
        F.OutNext("    total:  %2d", m_pModel->get_tris_count());
    }
}
#endif
