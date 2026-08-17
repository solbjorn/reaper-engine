// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WallmarksEngine.h"

#include "../../xr_3da/xr_object.h"
#include "../../xr_3da/x_ray.h"
#include "../../xr_3da/GameFont.h"
#include "SkeletonCustom.h"

namespace WallmarksEngine
{
struct wm_slot final
{
    typedef CWallmarksEngine::StaticWMVec StaticWMVec;
    ref_shader shader;
    StaticWMVec static_items;
    xr_vector<intrusive_ptr<CSkeletonWallmark>> skeleton_items;
    wm_slot(ref_shader sh)
    {
        shader = sh;
        static_items.reserve(256);
        skeleton_items.reserve(256);
    }
};
} // namespace WallmarksEngine

IC bool operator==(const CWallmarksEngine::wm_slot* slot, const ref_shader& shader) { return slot->shader == shader; }

CWallmarksEngine::wm_slot* CWallmarksEngine::FindSlot(const ref_shader& shader)
{
    WMSlotVecIt it = std::find(marks.begin(), marks.end(), shader);
    return (it != marks.end()) ? *it : nullptr;
}

CWallmarksEngine::wm_slot* CWallmarksEngine::AppendSlot(const ref_shader& shader)
{
    marks.push_back(xr_new<wm_slot>(shader));
    return marks.back();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallmarksEngine::CWallmarksEngine()
{
    static_pool.reserve(256);
    marks.reserve(256);

    hGeom.create(FVF::F_LIT, SGeometry::default_vb(), nullptr);
    XR_ASSERT(hGeom.stride() == sizeof(FVF::LIT));
}

CWallmarksEngine::~CWallmarksEngine()
{
    clear();
    hGeom.destroy();
}

void CWallmarksEngine::clear()
{
    for (auto& p_it : marks)
    {
        for (auto& m_it : p_it->static_items)
            static_wm_destroy(m_it);
        xr_delete(p_it);
    }
    marks.clear();

    for (u32 it = 0; it < static_pool.size(); it++)
        xr_delete(static_pool[it]);
    static_pool.clear();
}

// allocate
CWallmarksEngine::static_wallmark* CWallmarksEngine::static_wm_allocate()
{
    static_wallmark* W = nullptr;
    if (static_pool.empty())
        W = xr_new<static_wallmark>();
    else
    {
        W = static_pool.back();
        static_pool.pop_back();
    }

    W->ttl = ps_r__WallmarkTTL;
    W->verts.clear();
    return W;
}

// destroy
void CWallmarksEngine::static_wm_destroy(CWallmarksEngine::static_wallmark* W) { static_pool.push_back(W); }

// render
void CWallmarksEngine::static_wm_render(const static_wallmark& W, std::span<FVF::LIT> verts)
{
    const auto color = color_rgba(128, 128, 128, std::clamp(iFloor((1.0f - (W.ttl / ps_r__WallmarkTTL)) * 255.0f), 0, 255));

    for (auto [V, el] : std::views::zip(verts, W.verts))
    {
        V.p.set(el.p);
        V.color = color;
        V.t.set(el.t);
    }
}

//--------------------------------------------------------------------------------

void CWallmarksEngine::RecurseTri(u32 t, Fmatrix& mView, CWallmarksEngine::static_wallmark& W)
{
    CDB::TRI* T = sml_collector.getT() + t;
    if (T->dummy)
        return;

    XR_TRACY_ZONE_SCOPED();

    T->dummy = 0xffffffff;

    // Some vars
    u32* v_ids = T->verts;
    Fvector* v_data = sml_collector.getV();
    sml_poly_src.clear();
    sml_poly_src.push_back(v_data[v_ids[0]]);
    sml_poly_src.push_back(v_data[v_ids[1]]);
    sml_poly_src.push_back(v_data[v_ids[2]]);
    sml_poly_dest.clear();

    sPoly* P = sml_clipper.ClipPoly(sml_poly_src, sml_poly_dest);

    //. todo
    // uv_gen = mView * []
    // UV = pos*uv_gen

    if (P)
    {
        // Create vertices and triangulate poly (tri-fan style triangulation)
        FVF::LIT V0, V1, V2;
        Fvector UV;

        mView.transform_tiny(UV, (*P)[0]);
        V0.set((*P)[0], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);
        mView.transform_tiny(UV, (*P)[1]);
        V1.set((*P)[1], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);

        for (gsl::index i{2}; i < std::ssize(*P); ++i)
        {
            mView.transform_tiny(UV, (*P)[i]);
            V2.set((*P)[i], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);
            W.verts.push_back(V0);
            W.verts.push_back(V1);
            W.verts.push_back(V2);
            V1 = V2;
        }

        // recurse
        for (u32 i = 0; i < 3; i++)
        {
            u32 adj = sml_adjacency[3 * t + i];
            if (0xffffffff == adj)
                continue;
            CDB::TRI* SML = sml_collector.getT() + adj;
            v_ids = SML->verts;

            Fvector test_normal;
            test_normal.mknormal(v_data[v_ids[0]], v_data[v_ids[1]], v_data[v_ids[2]]);
            float cosa = test_normal.dotproduct(sml_normal);
            if (cosa < 0.034899f)
                continue; // cos(88)
            RecurseTri(adj, mView, W);
        }
    }
}

void CWallmarksEngine::BuildMatrix(Fmatrix& mView, float invsz, const Fvector& from)
{
    // build projection
    Fmatrix mScale;
    Fvector at, up, right, y;
    at.sub(from, sml_normal);
    y.set(0, 1, 0);
    if (_abs(sml_normal.y) > .99f)
        y.set(1, 0, 0);
    right.crossproduct(y, sml_normal);
    up.crossproduct(sml_normal, right);
    mView.build_camera(from, at, up);
    mScale.scale(invsz, invsz, invsz);
    mView.mulA_43(mScale);
}

void CWallmarksEngine::AddWallmark_internal(const CDB::TRI& pTri, std::span<const Fvector4> pVerts, const Fvector& contact_point, const ref_shader& hShader,
                                            f32 sz)
{
    XR_TRACY_ZONE_SCOPED();

    // query for polygons in bounding box
    // calculate adjacency
    Fbox bb_query;
    Fvector bbc, bbd;

    bb_query.set(contact_point, contact_point);
    bb_query.grow(sz * 2.5f);
    bb_query.get_CD(bbc, bbd);

    xrc.box_query(CDB::OPT_FULL_TEST, g_pGameLevel->ObjectSpace.GetStaticModel(), bbc, bbd);
    const auto triCount = xrc.r_count();
    if (0 == triCount)
        return;

    const auto tris = g_pGameLevel->ObjectSpace.GetStaticTris();
    const std::array<Fvector3, 3> vs{pVerts[pTri.verts[0]].xyz(), pVerts[pTri.verts[1]].xyz(), pVerts[pTri.verts[2]].xyz()};

    sml_collector.clear();
    sml_collector.add_face_packed_D(vs[0], vs[1], vs[2], 0);

    for (gsl::index t{0}; t < triCount; ++t)
    {
        auto& T = tris[xrc.r_begin()[t].id];
        if (&T == &pTri)
            continue;

        sml_collector.add_face_packed_D(pVerts[T.verts[0]].xyz(), pVerts[T.verts[1]].xyz(), pVerts[T.verts[2]].xyz(), 0);
    }

    sml_collector.calc_adjacency(sml_adjacency);

    // calc face normal
    Fvector N;
    N.mknormal(vs[0], vs[1], vs[2]);
    sml_normal.set(N);

    // build 3D ortho-frustum
    Fmatrix mView, mRot;
    BuildMatrix(mView, 1 / sz, contact_point);
    mRot.rotateZ(::Random.randF(deg2rad(-20.f), deg2rad(20.f)));
    mView.mulA_43(mRot);
    sml_clipper.CreateFromMatrix(mView, FRUSTUM_P_LRTB);

    // create wallmark
    static_wallmark* W = static_wm_allocate();
    RecurseTri(0, mView, *W);

    // calc sphere
    if (W->verts.size() < 3)
    {
        static_wm_destroy(W);
        return;
    }

    Fbox bb;
    bb.invalidate();

    for (const auto& el : W->verts)
        bb.modify(el.p);

    bb.getsphere(W->bounds.P, W->bounds.R);

    // search if similar wallmark exists
    wm_slot* slot = FindSlot(hShader);
    if (slot)
    {
        for (auto& it : slot->static_items)
        {
            static_wallmark* wm = it;
            if (wm->bounds.P.similar(W->bounds.P, 0.02f))
            { // replace
                static_wm_destroy(wm);
                it = W;
                return;
            }
        }
    }
    else
    {
        slot = AppendSlot(hShader);
    }

    // no similar - register _new_
    slot->static_items.push_back(W);
}

void CWallmarksEngine::AddStaticWallmark(const CDB::TRI& pTri, std::span<const Fvector4> pVerts, const Fvector& contact_point, const ref_shader& hShader,
                                         f32 sz)
{
    // optimization cheat: don't allow wallmarks more than 100 m from viewer/actor
    if (contact_point.distance_to_sqr(Device.vCameraPosition) > _sqr(100.f))
        return;

    // Physics may add wallmarks in parallel with rendering
    lock.Enter();
    AddWallmark_internal(pTri, pVerts, contact_point, hShader, sz);
    lock.Leave();
}

void CWallmarksEngine::AddSkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
    XR_ASSERT(xf != nullptr && obj != nullptr && size > EPS_L, "", size);

    // optimization cheat: don't allow wallmarks more than 50 m from viewer/actor
    if (xf->c.distance_to_sqr(Device.vCameraPosition) > _sqr(50.f))
        return;

    lock.Enter();
    obj->AddWallmark(xf, start, dir, sh, size);
    lock.Leave();
}

void CWallmarksEngine::AddSkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
    lock.Enter();
    // search if similar wallmark exists
    wm_slot* slot = FindSlot(wm->Shader());
    if (!slot)
        slot = AppendSlot(wm->Shader());
    // no similar - register _new_
    slot->skeleton_items.push_back(wm);
#ifdef DEBUG
    wm->used_in_render = Device.dwFrame;
#endif
    lock.Leave();
}

namespace
{
[[nodiscard]] ICF auto BeginStream(CBackend& cmd_list) { return cmd_list.Vertex.Lock<FVF::LIT>(cmd_list.Vertex.GetSize() / sizeof(FVF::LIT)); }

void FlushStream(CBackend& cmd_list, ref_geom hGeom, ref_shader shader, std::size_t w_count, bool bSuppressCull)
{
    const auto w_offset = cmd_list.Vertex.Unlock<FVF::LIT>(w_count);

    if (w_count == 0)
        return;

    cmd_list.set_Shader(shader);
    cmd_list.set_Geometry(hGeom);

    if (bSuppressCull)
        cmd_list.set_CullMode(CULL_NONE);

    cmd_list.Render(D3DPT_TRIANGLELIST, w_offset, w_count / 3);

    if (bSuppressCull)
        cmd_list.set_CullMode(CULL_CCW);

    Device.Statistic->RenderDUMP_WMT_Count += w_count / 3;
}
} // namespace

void CWallmarksEngine::Render()
{
    XR_TRACY_ZONE_SCOPED();

    auto& dsgraph = RImplementation.get_imm_context();
    auto& cmd_list = dsgraph.cmd_list;

    // Projection and xform
    Fmatrix mProject = Device.mProject;
    mProject._43 -= ps_r__WallmarkSHIFT;

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_project(mProject);

    Fvector mViewPos;
    Fmatrix mView;
    mViewPos.mad(Device.vCameraPosition, Device.vCameraDirection, ps_r__WallmarkSHIFT_V);
    mView.build_camera_dir(mViewPos, Device.vCameraDirection, Device.vCameraTop);

    cmd_list.set_xform_view(mView);

    Device.Statistic->RenderDUMP_WM.Begin();
    Device.Statistic->RenderDUMP_WMS_Count = 0;
    Device.Statistic->RenderDUMP_WMD_Count = 0;
    Device.Statistic->RenderDUMP_WMT_Count = 0;

    const float ssaCLIP = r_ssaDISCARD / 4;

    lock.Enter(); // Physics may add wallmarks in parallel with rendering

    for (auto slot : marks)
    {
        // static wallmarks
        auto verts = BeginStream(cmd_list);
        std::size_t written{0};

        for (auto w_it = slot->static_items.begin(), w_end = slot->static_items.end(); w_it != w_end;)
        {
            static_wallmark* W = *w_it;

            if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P, W->bounds.R))
            {
                Device.Statistic->RenderDUMP_WMS_Count++;

                if ((W->bounds.R * W->bounds.R) / Device.vCameraPosition.distance_to_sqr(W->bounds.P) >= ssaCLIP)
                {
                    const auto needed = W->verts.size();

                    if (written + needed > verts.size())
                    {
                        FlushStream(cmd_list, hGeom, slot->shader, written, false);

                        verts = BeginStream(cmd_list);
                        written = 0;

                        XR_ASSERT(needed <= verts.size());
                    }

                    static_wm_render(*W, verts.subspan(written, needed));
                    written += needed;
                }

                W->ttl -= 0.1f * Device.fTimeDelta; // visible wallmarks fade much slower
            }
            else
            {
                W->ttl -= Device.fTimeDelta;
            }

            if (W->ttl <= EPS)
            {
                static_wm_destroy(W);

                w_it = slot->static_items.erase(w_it);
                w_end = slot->static_items.end();
            }
            else
            {
                w_it++;
            }
        }

        // Flush stream
        FlushStream(cmd_list, hGeom, slot->shader, written, false); //. remove line if !(suppress cull needed)

        // dynamic wallmarks
        verts = BeginStream(cmd_list);
        written = 0;

        for (auto& W : slot->skeleton_items)
        {
            if (!W)
                continue;

#ifdef DEBUG
            XR_ASSERT(W->used_in_render == Device.dwFrame);
#endif

            if ((W->m_Bounds.R * W->m_Bounds.R) / Device.vCameraPosition.distance_to_sqr(W->m_Bounds.P) >= ssaCLIP)
            {
                Device.Statistic->RenderDUMP_WMD_Count++;

                const auto needed = W->VCount();

                if (written + needed > verts.size())
                {
                    FlushStream(cmd_list, hGeom, slot->shader, written, true);

                    verts = BeginStream(cmd_list);
                    written = 0;

                    XR_ASSERT(needed <= verts.size());
                }

                W->Parent()->RenderWallmark(*W, verts.subspan(written, needed));
                written += needed;
            }

#ifdef DEBUG
            W->used_in_render = u32(-1);
#endif
        }

        slot->skeleton_items.clear();

        // Flush stream
        FlushStream(cmd_list, hGeom, slot->shader, written, true);
    }

    lock.Leave(); // Physics may add wallmarks in parallel with rendering

    // Level-wmarks
    dsgraph.render_wmarks();
    Device.Statistic->RenderDUMP_WM.End();

    // Projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}
