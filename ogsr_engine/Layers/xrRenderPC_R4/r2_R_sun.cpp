#include "stdafx.h"

#include <oneapi/tbb/parallel_invoke.h>
#include "xr_task.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/irenderable.h"
#include "../xrRender/FBasicVisual.h"

#include "r4_R_sun_support.h"

constexpr float tweak_COP_initial_offs = 1200.f;

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
static constexpr Fvector3 corners[8] = {{-1, -1, 0}, {-1, -1, +1}, {-1, +1, +1}, {-1, +1, 0}, {+1, +1, +1}, {+1, +1, 0}, {+1, -1, +1}, {+1, -1, 0}};
static constexpr u32 facetable[6][4] = {
    {6, 7, 5, 4},
    {1, 0, 7, 6},
    {1, 2, 3, 0},
    {3, 2, 4, 5},
    // near and far planes
    {0, 3, 5, 7},
    {1, 6, 4, 2},
};

//////////////////////////////////////////////////////////////////////////
Fvector3 wform(const Fmatrix& m, const Fvector3& v)
{
    Fvector4 r;
    r.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
    r.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
    r.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
    r.w = v.x * m._14 + v.y * m._24 + v.z * m._34 + m._44;
    float invW = 1.0f / r.w;

    return {r.x * invW, r.y * invW, r.z * invW};
}

void CRender::calculate_sun(sun::cascade& cascade)
{
    light* sun = (light*)Lights.sun._get();

    CFrustum& cull_frustum = cascade.cull_frustum;
    Fmatrix& cull_xform = cascade.cull_xform;
    Fvector3& cull_COP = cascade.cull_COP;

    {
        // calculate view-frustum bounds in world space
        Fmatrix ex_full{}, ex_full_inverse{};
        {
            ex_full.mul(Device.mProject, Device.mView);
            ex_full_inverse.invert_44(ex_full);
        }

        xr_vector<Fplane> cull_planes;

        // Lets begin from base frustum
        Fmatrix fullxform_inv = ex_full_inverse;

        // COP - 100 km away
        cull_COP.mad(Device.vCameraPosition, sun->direction, -tweak_COP_initial_offs);

        // Create approximate ortho-xform
        // view: auto find 'up' and 'right' vectors
        Fmatrix mdir_View, mdir_Project;
        Fvector L_dir, L_up, L_right, L_pos;
        L_pos.set(sun->position);
        L_dir.set(sun->direction).normalize();
        L_right.set(1, 0, 0);
        if (_abs(L_right.dotproduct(L_dir)) > .99f)
            L_right.set(0, 0, 1);
        L_up.crossproduct(L_dir, L_right).normalize();
        L_right.crossproduct(L_up, L_dir).normalize();
        mdir_View.build_camera_dir(L_pos, L_dir, L_up);

        //////////////////////////////////////////////////////////////////////////
        typedef FixedConvexVolume<false> t_cuboid;

        t_cuboid light_cuboid;
        {
            // Initialize the first cascade rays, then each cascade will initialize rays for next one.
            if (cascade.cascade_ind == 0 || cascade.reset_chain)
            {
                Fvector3 near_p, edge_vec;
                light_cuboid.view_frustum_rays.reserve(4);
                for (u32 p = 0; p < 4; p++)
                {
                    near_p = wform(fullxform_inv, corners[facetable[4][p]]);

                    edge_vec = wform(fullxform_inv, corners[facetable[5][p]]);
                    edge_vec.sub(near_p);
                    edge_vec.normalize();

                    light_cuboid.view_frustum_rays.emplace_back(near_p, edge_vec);
                }
            }
            else
                light_cuboid.view_frustum_rays = cascade.rays;

            light_cuboid.view_ray.P = Device.vCameraPosition;
            light_cuboid.view_ray.D = Device.vCameraDirection;
            light_cuboid.light_ray.P = L_pos;
            light_cuboid.light_ray.D = L_dir;
        }

        // THIS NEED TO BE A CONSTATNT
        Fplane light_top_plane;
        light_top_plane.build_unit_normal(L_pos, L_dir);
        float dist = light_top_plane.classify(Device.vCameraPosition);

        float map_size = cascade.size;
        mdir_Project.mm = DirectX::XMMatrixOrthographicOffCenterLH(-map_size * 0.5f, map_size * 0.5f, -map_size * 0.5f, map_size * 0.5f, 0.1f, dist + map_size);

        //////////////////////////////////////////////////////////////////////////

        /**/

        // build viewport xform
        float view_dim = float(o.smapsize);
        Fmatrix m_viewport = {view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, view_dim / 2.f, view_dim / 2.f, 0.0f, 1.0f};
        Fmatrix m_viewport_inv{};
        m_viewport_inv.invert_44(m_viewport);

        // snap view-position to pixel
        cull_xform.mul(mdir_Project, mdir_View);
        Fmatrix cull_xform_inv;
        cull_xform_inv.invert(cull_xform);

        //		light_cuboid.light_cuboid_points.reserve		(9);
        for (int p = 0; p < 8; p++)
        {
            const Fvector3 xf = wform(cull_xform_inv, corners[p]);
            light_cuboid.light_cuboid_points[p] = xf;
        }

        // only side planes
        for (u32 plane = 0; plane < 4; plane++)
        {
            for (u32 pt = 0; pt < 4; pt++)
            {
                u32 asd = facetable[plane][pt];
                light_cuboid.light_cuboid_polys[plane].points[pt] = asd;
            }
        }

        Fvector lightXZshift;
        light_cuboid.compute_caster_model_fixed(cull_planes, lightXZshift, cascade.size, cascade.reset_chain);
        Fvector proj_view = Device.vCameraDirection;
        proj_view.y = 0;
        proj_view.normalize();

        // Initialize rays for the next cascade
        if (cascade.cascade_ind < SE_SUN_FAR)
            m_sun_cascades[cascade.cascade_ind + 1].rays = light_cuboid.view_frustum_rays;

#ifdef DEBUG
        constexpr bool draw_debug = false;
        if (draw_debug && cascade.cascade_ind == SE_SUN_NEAR)
            for (u32 it = 0; it < cull_planes.size(); it++)
                Target->dbg_addplane(cull_planes[it], it * 0xFFF);
#endif

        Fvector cam_shifted = L_pos;
        cam_shifted.add(lightXZshift);

        // rebuild the view transform with the shift.
        mdir_View.identity();
        mdir_View.build_camera_dir(cam_shifted, L_dir, L_up);
        cull_xform.identity();
        cull_xform.mul(mdir_Project, mdir_View);
        cull_xform_inv.invert(cull_xform);

        // Create frustum for query
        cull_frustum._clear();
        for (auto& plane : cull_planes)
            cull_frustum._add(plane);

        {
            Fvector cam_proj = Device.vCameraPosition;
            constexpr float align_aim_step_coef = 4.f;
            cam_proj.set(floorf(cam_proj.x / align_aim_step_coef) + align_aim_step_coef / 2, floorf(cam_proj.y / align_aim_step_coef) + align_aim_step_coef / 2,
                         floorf(cam_proj.z / align_aim_step_coef) + align_aim_step_coef / 2);
            cam_proj.mul(align_aim_step_coef);
            Fvector cam_pixel = wform(cull_xform, cam_proj);
            cam_pixel = wform(m_viewport, cam_pixel);
            Fvector shift_proj = lightXZshift;
            cull_xform.transform_dir(shift_proj);
            m_viewport.transform_dir(shift_proj);

            constexpr float align_granularity = 4.f;
            shift_proj.x = shift_proj.x > 0 ? align_granularity : -align_granularity;
            shift_proj.y = shift_proj.y > 0 ? align_granularity : -align_granularity;
            shift_proj.z = 0;

            cam_pixel.x = cam_pixel.x / align_granularity - floorf(cam_pixel.x / align_granularity);
            cam_pixel.y = cam_pixel.y / align_granularity - floorf(cam_pixel.y / align_granularity);
            cam_pixel.x *= align_granularity;
            cam_pixel.y *= align_granularity;
            cam_pixel.z = 0;

            cam_pixel.sub(shift_proj);

            m_viewport_inv.transform_dir(cam_pixel);
            cull_xform_inv.transform_dir(cam_pixel);
            Fvector diff = cam_pixel;
            constexpr float sign_test = -1.f;
            diff.mul(sign_test);
            Fmatrix adjust;
            adjust.translate(diff);
            cull_xform.mulB_44(adjust);
        }

        s32 limit = o.smapsize - 1;
        sun->X.D.minX = 0;
        sun->X.D.maxX = limit;
        sun->X.D.minY = 0;
        sun->X.D.maxY = limit;

        // full-xform
    }
}

void CRender::accumulate_cascade(u32 cascade_ind)
{
    auto& cascade = m_sun_cascades[cascade_ind];
    auto& cmd_list = get_context(cascade.context_id).cmd_list;

    PIX_EVENT_CTX(cmd_list, accumulate_cascade);

    // Accumulate
    Target->phase_accumulator(cmd_list);
    Target->rt_smap_depth->set_slice_read(cmd_list.context_id);
    Target->accum_direct_cascade(cmd_list, cascade_ind, cascade.cull_xform, m_sun_cascades[cascade_ind == SE_SUN_NEAR ? cascade_ind : (cascade_ind - 1)].cull_xform, cascade.bias);
}

void CRender::sun_run()
{
    sun_active = is_sun();
    if (!sun_active)
        return;

    for (auto& cascade : m_sun_cascades)
    {
        cascade.context_id = alloc_context();
        VERIFY(cascade.context_id != R__INVALID_CTX_ID);
    }

    sun_tg->run([this] {
        constexpr float fBias = -0.0000025f;
        auto* cascade = &m_sun_cascades[SE_SUN_NEAR];

        cascade->cascade_ind = SE_SUN_NEAR;
        cascade->reset_chain = true;
        cascade->size = ps_ssfx_shadow_cascades.x; // 40;
        cascade->bias = cascade->size * fBias;

        cascade = &m_sun_cascades[SE_SUN_MIDDLE];
        cascade->cascade_ind = SE_SUN_MIDDLE;
        cascade->size = ps_ssfx_shadow_cascades.y; // 120;
        cascade->bias = cascade->size * fBias;

        cascade = &m_sun_cascades[SE_SUN_FAR];
        cascade->cascade_ind = SE_SUN_FAR;
        cascade->size = ps_ssfx_shadow_cascades.z; // 320;
        cascade->bias = cascade->size * fBias;

        bool need_to_render_sunshafts = Target->need_to_render_sunshafts(), last_cascade_chain_mode;
        if (need_to_render_sunshafts)
        {
            last_cascade_chain_mode = cascade->reset_chain;
            cascade->reset_chain = true;
        }

        // Compute volume(s) - something like a frustum for infinite directional light
        // Also compute virtual light position and sector it is inside

        for (auto& cascade : m_sun_cascades)
            calculate_sun(cascade);

        if (need_to_render_sunshafts)
            cascade->reset_chain = last_cascade_chain_mode;

        for (auto& cascade : m_sun_cascades)
        {
            sun_tg->run([this, &cascade] {
                auto& dsgraph = get_context(cascade.context_id);

                // Fill the database
                dsgraph.r_pmask(true, false);
                dsgraph.build_subspace(largest_sector_id, cascade.cull_frustum, cascade.cull_xform, cascade.cull_COP, true);

                // Finalize & Cleanup
                light& sun = *(light*)Lights.sun._get();
                sun.X.D.combine[cascade.cascade_ind] = cascade.cull_xform;

                sun_tg->run([this, cascade_ind = cascade.cascade_ind] {
                    // Begin SMAP-render
                    auto& dsgraph = get_context(m_sun_cascades[cascade_ind].context_id);

                    // Render shadow-map
                    //. !!! We should clip based on shrinked frustum (again)
                    bool empty = dsgraph.mapNormalPasses[0][0].empty() && dsgraph.mapMatrixPasses[0][0].empty();
                    if (empty)
                        return;

                    VERIFY(dsgraph.mapNormalPasses[1][0].empty() && dsgraph.mapMatrixPasses[1][0].empty() && dsgraph.mapSorted.empty());
                    const light& sun = *(const light*)Lights.sun._get();

                    Target->phase_smap_direct(dsgraph.cmd_list, &sun, cascade_ind);
                    dsgraph.cmd_list.set_xform_world(Fidentity);
                    dsgraph.cmd_list.set_xform_view(Fidentity);
                    dsgraph.cmd_list.set_xform_project(sun.X.D.combine[cascade_ind]);
                    dsgraph.render_graph(0);

                    if (ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS) && cascade_ind <= ps_ssfx_grass_shadows.x)
                        Details->Render(dsgraph.cmd_list, dm_fade * dm_fade * ps_ssfx_grass_shadows.y);
                });
            });
        }
    });
}

void CRender::sun_sync()
{
    PIX_EVENT(DEFER_SUN);
    sun_tg->wait();

    for (u32 i = 0; i < R__NUM_SUN_CASCADES + 1; i++)
    {
        // Accumulate the current cascade and flush the previous one in parallel
        oneapi::tbb::parallel_invoke(
            [this, i] {
                if (i < R__NUM_SUN_CASCADES)
                    accumulate_cascade(i);
            },
            [this, i] {
                if (!i)
                    return;

                ctx_id_t context_id = m_sun_cascades[i - 1].context_id;
                get_context(context_id).cmd_list.submit();
                release_context(context_id);
            });
    }

    auto& cmd_list = get_imm_context().cmd_list;
    cmd_list.Invalidate();

    // Restore XForms
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}
