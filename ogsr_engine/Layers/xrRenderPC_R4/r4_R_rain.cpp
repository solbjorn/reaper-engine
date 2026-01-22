#include "stdafx.h"

#include "r4_R_sun_support.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/irenderable.h"
#include "../xrRender/FBasicVisual.h"

namespace
{
constexpr f32 tweak_rain_COP_initial_offs{1200.0f};
constexpr f32 tweak_rain_ortho_xform_initial_offs{1000.0f}; //. ?

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
constexpr Fvector corners[8]{Fvector{-1.0f, -1.0f, 0.0f},  Fvector{-1.0f, -1.0f, +1.0f}, Fvector{-1.0f, +1.0f, +1.0f}, Fvector{-1.0f, +1.0f, 0.0f},
                             Fvector{+1.0f, +1.0f, +1.0f}, Fvector{+1.0f, +1.0f, 0.0f},  Fvector{+1.0f, -1.0f, +1.0f}, Fvector{+1.0f, -1.0f, 0.0f}};
constexpr u32 facetable[6][4]{{0, 3, 5, 7}, {1, 2, 3, 0}, {6, 7, 5, 4}, {4, 2, 1, 6}, {3, 2, 4, 5}, {1, 0, 7, 6}};
} // namespace

//////////////////////////////////////////////////////////////////////////

bool CRender::rain_prepare()
{
    if (!ps_r2_ls_flags.test(R3FLAG_DYN_WET_SURF))
        return false;

    const auto& env = g_pGamePersistent->Environment();
    const float fRainFactor = ps_ssfx_gloss_method == 0 ? env.CurrentEnv->rain_density : env.wetness_factor;
    if (fRainFactor < EPS_L)
        return false;

    rain_context_id = alloc_context();
    VERIFY(rain_context_id != R__INVALID_CTX_ID);

    return true;
}

tmc::task<void> CRender::rain_run()
{
    XR_TRACY_ZONE_SCOPED();

    //	Use light as placeholder for rain data.
    auto& RainLight = *smart_cast<light*>(Lights.rain._get());

    constexpr float source_offset = 10000.f;
    RainLight.direction.set(0.0f, -1.0f, 0.0f);
    RainLight.position.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);

    float fBoundingSphereRadius = 0;

    // calculate view-frustum bounds in world space
    Fmatrix ex_project{}, ex_full{}, ex_full_inverse{};

    const float fRainFar = ps_ssfx_gloss_method == 0 ? ps_r3_dyn_wet_surf_far : 250.f;
    ex_project.build_projection(deg2rad(Device.fFOV /* *Device.fASPECT*/), Device.fASPECT, VIEWPORT_NEAR, fRainFar);
    ex_full.mul(ex_project, Device.mView);
    ex_full_inverse.invert_44(ex_full);

    //	Calculate view frustum were we can see dynamic rain radius
    //	b^2 = 2RH, B - side enge of the pyramid, h = height
    //	R = b^2/(2*H)
    const float H = fRainFar;
    const float a = tanf(deg2rad(Device.fFOV) / 2);
    const float c = tanf(deg2rad(Device.fFOV * Device.fASPECT) / 2);
    const float b_2 = H * H * (1.0f + a * a + c * c);
    fBoundingSphereRadius = b_2 / (2.0f * H);

    // Compute volume(s) - something like a frustum for infinite directional light
    // Also compute virtual light position and sector it is inside
    CFrustum cull_frustum;
    xr_vector<Fplane> cull_planes;
    Fvector3 cull_COP;
    Fmatrix cull_xform;

    // Lets begin from base frustum
    Fmatrix fullxform_inv = ex_full_inverse;
#ifdef DEBUG
    typedef DumbConvexVolume<true> t_volume;
#else
    typedef DumbConvexVolume<false> t_volume;
#endif
    t_volume hull;
    {
        hull.points.reserve(9);
        for (u32 p = 0; p < 8; p++)
        {
            const Fvector3 xf = wform(fullxform_inv, corners[p]);
            hull.points.push_back(xf);
        }
        for (u32 plane = 0; plane < 6; plane++)
        {
            hull.polys.push_back(t_volume::_poly());
            for (u32 pt = 0; pt < 4; pt++)
                hull.polys.back().points.push_back(facetable[plane][pt]);
        }
    }

    hull.compute_caster_model(cull_planes, RainLight.direction);

#ifdef DEBUG
    for (u32 it = 0; it < cull_planes.size(); it++)
        Target->dbg_addplane(cull_planes[it], 0xffffffff);
#endif

    // COP - 100 km away
    cull_COP.mad(Device.vCameraPosition, RainLight.direction, -tweak_rain_COP_initial_offs);
    cull_COP.x += fBoundingSphereRadius * Device.vCameraDirection.x;
    cull_COP.z += fBoundingSphereRadius * Device.vCameraDirection.z;

    // Create frustum for query
    cull_frustum._clear();
    for (auto& plane : cull_planes)
        cull_frustum._add(plane);

    // Create approximate ortho-xform
    // view: auto find 'up' and 'right' vectors
    Fmatrix mdir_View, mdir_Project;
    Fvector L_dir, L_up, L_right, L_pos;
    L_pos.set(RainLight.position);
    L_dir.set(RainLight.direction).normalize();
    L_right.set(1, 0, 0);
    if (_abs(L_right.dotproduct(L_dir)) > .99f)
        L_right.set(0, 0, 1);
    L_up.crossproduct(L_dir, L_right).normalize();
    L_right.crossproduct(L_up, L_dir).normalize();
    mdir_View.build_camera_dir(L_pos, L_dir, L_up);

    // projection: box
    //	Simple
    Fbox frustum_bb;
    frustum_bb.invalidate();
    for (u32 it = 0; it < 8; it++)
    {
        const Fvector xf = wform(mdir_View, hull.points[it]);
        frustum_bb.modify(xf);
    }
    Fbox& bb = frustum_bb;
    bb.grow(EPS);

    //	Offset RainLight position to center rain shadowmap
    Fvector3 vRectOffset;
    vRectOffset.set(fBoundingSphereRadius * Device.vCameraDirection.x, 0, fBoundingSphereRadius * Device.vCameraDirection.z);
    bb.min.x = -fBoundingSphereRadius + vRectOffset.x;
    bb.max.x = fBoundingSphereRadius + vRectOffset.x;
    bb.min.y = -fBoundingSphereRadius + vRectOffset.z;
    bb.max.y = fBoundingSphereRadius + vRectOffset.z;

    mdir_Project.mm = DirectX::XMMatrixOrthographicOffCenterLH(bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z - tweak_rain_ortho_xform_initial_offs,
                                                               bb.min.z + 2 * tweak_rain_ortho_xform_initial_offs);
    cull_xform.mul(mdir_Project, mdir_View);

    const s32 limit = std::min(s32{o.smapsize}, ps_r3_dyn_wet_surf_sm_res);

    // build viewport xform
    const float view_dim = float(limit);
    const float fTexelOffs = (0.5f / o.smapsize);
    const Fmatrix m_viewport{view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, view_dim / 2.f + fTexelOffs, view_dim / 2.f + fTexelOffs,
                             0.0f,           1.0f};
    Fmatrix m_viewport_inv;
    m_viewport_inv.invert_44(m_viewport);

    // snap view-position to pixel
    //	snap zero point to pixel
    const Fvector cam_proj = wform(cull_xform, {});
    Fvector cam_pixel = wform(m_viewport, cam_proj);
    cam_pixel.x = floorf(cam_pixel.x);
    cam_pixel.y = floorf(cam_pixel.y);
    const Fvector cam_snapped = wform(m_viewport_inv, cam_pixel);
    Fvector diff;
    diff.sub(cam_snapped, cam_proj);
    Fmatrix adjust;
    adjust.translate(diff);
    cull_xform.mulA_44(adjust);

    RainLight.X.D.minX = 0;
    RainLight.X.D.maxX = limit;
    RainLight.X.D.minY = 0;
    RainLight.X.D.maxY = limit;

    // full-xform

    XR_TRACY_ZONE_SCOPED();

    auto& dsgraph = get_context(rain_context_id);

    // Fill the database
    dsgraph.r_pmask(true, false);
    dsgraph.build_subspace(largest_sector_id, cull_frustum, cull_xform, cull_COP, false);

    // Finalize & Cleanup
    RainLight.X.D.combine[0] = cull_xform;

    // Begin SMAP-render
    XR_TRACY_ZONE_SCOPED();
    PIX_EVENT_CTX(dsgraph.cmd_list, render_rain);

    // Render shadow-map
    //. !!! We should clip based on shrinked frustum (again)
    if (dsgraph.mapNormalPasses[0][0].empty() && dsgraph.mapMatrixPasses[0][0].empty())
        co_return;

    VERIFY(dsgraph.mapNormalPasses[1][0].empty() && dsgraph.mapMatrixPasses[1][0].empty() && dsgraph.mapSorted.empty());

    Target->phase_smap_direct(dsgraph.cmd_list, &RainLight, SE_SUN_RAIN_SMAP);
    dsgraph.cmd_list.set_xform_world(Fidentity);
    dsgraph.cmd_list.set_xform_view(Fidentity);
    dsgraph.cmd_list.set_xform_project(RainLight.X.D.combine[0]);
    dsgraph.render_graph(0);
}

void CRender::rain_sync()
{
    XR_TRACY_ZONE_SCOPED();
    PIX_EVENT(DEFER_RAIN);

    get_context(rain_context_id).cmd_list.submit();
    release_context(rain_context_id);

    auto& cmd_list = get_imm_context().cmd_list;
    cmd_list.Invalidate();

    // Restore XForms
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);

    // Accumulate
    Target->phase_rain();
    Target->rt_smap_depth->set_slice_read(rain_context_id);
    Target->draw_rain(*smart_cast<const light*>(Lights.rain._get()));
}
