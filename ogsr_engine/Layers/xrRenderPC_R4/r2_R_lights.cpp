#include "stdafx.h"
#include "xr_task.h"

static bool check_grass_shadow(const light* L, const CFrustum& VB)
{
    // Grass shadows are allowed?
    if (ps_ssfx_grass_shadows.x < 1 || !ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS))
        return false;

    // Inside the range?
    if (L->vis.distance > ps_ssfx_grass_shadows.z)
        return false;

    // Is in view? L->vis.visible?
    u32 mask = 0xff;
    if (!VB.testSphere(L->position, L->range * 0.6f, mask))
        return false;

    return true;
}

struct light_ctx
{
    ctx_id_t context_id{R__INVALID_CTX_ID};
    u32 curr{};
    xr_vector<light*> lights;
};

void CRender::render_lights_shadowed_one(light_ctx& task)
{
    lights_tg->run([this, &task] {
        auto& dsgraph = get_context(task.context_id);
        light* L = task.lights[task.curr];

        PIX_EVENT(SHADOWED_LIGHTS_RENDER_SUBSPACE);
        L->svis[dsgraph.context_id].begin();

        CFrustum temp;
        temp.CreateFromMatrix(L->X.S.combine, FRUSTUM_P_ALL & ~FRUSTUM_P_NEAR);

        dsgraph.r_pmask(true, false);
        dsgraph.build_subspace(L->spatial.sector_id, temp, L->X.S.combine, L->position, true);

        bool empty = dsgraph.mapNormalPasses[0][0].empty() && dsgraph.mapMatrixPasses[0][0].empty();
        VERIFY(dsgraph.mapNormalPasses[1][0].empty() && dsgraph.mapMatrixPasses[1][0].empty() && dsgraph.mapSorted.empty());

        if (!empty)
        {
            lights_tg->run([this, &task] {
                auto& dsgraph = get_context(task.context_id);
                light* L = task.lights[task.curr];

                stats.s_merged++;

                Target->phase_smap_spot(dsgraph.cmd_list, L);
                dsgraph.cmd_list.set_xform_world(Fidentity);
                dsgraph.cmd_list.set_xform_view(L->X.S.view);
                dsgraph.cmd_list.set_xform_project(L->X.S.project);
                dsgraph.render_graph(0);

                if (Details && check_grass_shadow(L, ViewBase))
                    Details->Render(dsgraph.cmd_list, L->position);

                L->svis[dsgraph.context_id].end();

                if (++task.curr < task.lights.size())
                    render_lights_shadowed_one(task);
            });
        }
        else
        {
            stats.s_finalclip++;

            L->svis[dsgraph.context_id].end();
            task.lights.erase(task.lights.begin() + task.curr);

            if (task.curr < task.lights.size())
                render_lights_shadowed_one(task);
        }
    });
}

void CRender::render_lights_shadowed(light_Package& LP)
{
    auto& source = LP.v_shadowed;

    //////////////////////////////////////////////////////////////////////////
    // Refactor order based on ability to pack shadow-maps
    // 1. calculate area + sort in descending order
    {
        for (size_t it = 0; it < source.size(); it++)
        {
            light* L = source[it];
            if (!L->vis.visible || L->spatial.sector_id == INVALID_SECTOR_ID)
            {
                source.erase(source.begin() + it);
                it--;
            }
            else
            {
                LR.compute_xf_spot(L);
                Lights_LastFrame.push_back(L);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // sort lights by importance???
    // while (has_any_lights_that_cast_shadows) {
    //		if (has_point_shadowed)		->	generate point shadowmap
    //		if (has_spot_shadowed)		->	generate spot shadowmap
    //		switch-to-accumulator
    //		if (has_point_unshadowed)	-> 	accum point unshadowed
    //		if (has_spot_unshadowed)	-> 	accum spot unshadowed
    //		if (was_point_shadowed)		->	accum point shadowed
    //		if (was_spot_shadowed)		->	accum spot shadowed
    //	}
    //	if (left_some_lights_that_doesn't cast shadows)
    //		accumulate them

    // if (has_spot_shadowed)
    while (!source.empty())
    {
        PIX_EVENT(SHADOWED_LIGHTS);
        stats.s_used++;

        static xr_vector<light_ctx> light_tasks;
        light_tasks.reserve(R__NUM_PARALLEL_CONTEXTS);

        for (size_t i = 0; i < std::min<size_t>(R__NUM_PARALLEL_CONTEXTS, source.size()); i++)
        {
            ctx_id_t id = alloc_context();
            if (id != R__INVALID_CTX_ID)
                light_tasks.emplace_back().context_id = id;
        }

        const size_t num_tasks = light_tasks.size();

        // 2. refactor - infact we could go from the backside and sort in ascending order
        const auto light_cmp = [](const light* l1, const light* l2) {
            const u32 a0 = l1->X.S.size;
            const u32 a1 = l2->X.S.size;
            return a0 > a1; // reverse -> descending
        };

        std::ranges::sort(source, light_cmp);

        size_t curr_task = 0;
        bool init = true;

        for (size_t test = 0; test < source.size(); test++)
        {
            light* L = source[test];
            size_t fails = 0;

            while (true)
            {
                light_ctx& task = light_tasks[curr_task];

                auto& cmd_list = get_context(task.context_id).cmd_list;
                if (init)
                    cmd_list.LP_smap_pool.initialize(o.smapsize);

                if (++curr_task == num_tasks)
                {
                    curr_task = 0;
                    init = false;
                }

                SMAP_Rect R{};
                if (cmd_list.LP_smap_pool.push(R, L->X.S.size))
                {
                    // OK
                    L->X.S.posX = R.min.x;
                    L->X.S.posY = R.min.y;
                    L->vis.smap_ID = cmd_list.context_id;
                    task.lights.push_back(L);

                    source.erase(source.begin() + test);
                    test--;
                    break;
                }
                else if (++fails == num_tasks)
                {
                    goto calc;
                }
            }
        }

    calc:
        for (auto& task : light_tasks)
        {
            Target->phase_smap_spot_clear(get_context(task.context_id).cmd_list);
            render_lights_shadowed_one(task);
        }

        static xr_vector<light*> L_spot_s;

        lights_tg->wait();

        for (auto& task : light_tasks)
        {
            for (auto* light : task.lights)
                L_spot_s.push_back(light);

            get_context(task.context_id).cmd_list.submit();
            release_context(task.context_id);
        }

        auto& cmd_list = get_imm_context().cmd_list;
        cmd_list.Invalidate();

        light_tasks.clear();
        std::ranges::sort(L_spot_s, light_cmp);

        PIX_EVENT(UNSHADOWED_LIGHTS);

        //		switch-to-accumulator
        Target->phase_accumulator(cmd_list);

        PIX_EVENT(POINT_LIGHTS);

        //		if (has_point_unshadowed)	-> 	accum point unshadowed
        if (!LP.v_point.empty())
        {
            light* L2 = LP.v_point.back();
            LP.v_point.pop_back();
            if (L2->vis.visible)
                Target->accum_point(L2);
        }

        PIX_EVENT(SPOT_LIGHTS);

        //		if (has_spot_unshadowed)	-> 	accum spot unshadowed
        if (!LP.v_spot.empty())
        {
            light* L2 = LP.v_spot.back();
            LP.v_spot.pop_back();
            if (L2->vis.visible)
            {
                LR.compute_xf_spot(L2);
                Target->accum_spot(L2);
            }
        }

        PIX_EVENT(SPOT_LIGHTS_ACCUM_VOLUMETRIC);

        //		if (was_spot_shadowed)		->	accum spot shadowed
        if (!L_spot_s.empty())
        {
            PIX_EVENT(ACCUM_SPOT);
            for (light* p_light : L_spot_s)
            {
                Target->rt_smap_depth->set_slice_read(p_light->vis.smap_ID);
                Target->accum_spot(p_light);
            }

            PIX_EVENT(ACCUM_VOLUMETRIC);
            if (ps_r2_ls_flags.is(R2FLAG_VOLUMETRIC_LIGHTS))
            {
                // Current Resolution
                float w = float(Device.dwWidth);
                float h = float(Device.dwHeight);

                // Adjust resolution
                if (o.ssfx_volumetric)
                    cmd_list.set_viewport_size(w / 8, h / 8);

                for (light* p_light : L_spot_s)
                {
                    Target->rt_smap_depth->set_slice_read(p_light->vis.smap_ID);
                    Target->accum_volumetric(p_light);
                }

                // Restore resolution
                if (o.ssfx_volumetric)
                    cmd_list.set_viewport_size(w, h);
            }

            L_spot_s.clear();
        }
    }
}

void CRender::render_lights(light_Package& LP)
{
    if (!LP.v_shadowed.empty())
        render_lights_shadowed(LP);

    PIX_EVENT(POINT_LIGHTS_ACCUM);
    // Point lighting (unshadowed, if left)
    if (!LP.v_point.empty())
    {
        xr_vector<light*>& Lvec = LP.v_point;
        for (light* p_light : Lvec)
        {
            if (p_light->vis.visible)
                Target->accum_point(p_light);
        }
        Lvec.clear();
    }

    PIX_EVENT(SPOT_LIGHTS_ACCUM);
    // Spot lighting (unshadowed, if left)
    if (!LP.v_spot.empty())
    {
        xr_vector<light*>& Lvec = LP.v_spot;
        for (light* p_light : Lvec)
        {
            if (p_light->vis.visible)
            {
                LR.compute_xf_spot(p_light);
                Target->accum_spot(p_light);
            }
        }
        Lvec.clear();
    }
}
