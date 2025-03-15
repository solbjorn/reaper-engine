#include "stdafx.h"

bool check_grass_shadow(light* L, CFrustum VB)
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

void CRender::render_lights(light_Package& LP)
{
    //////////////////////////////////////////////////////////////////////////
    // Refactor order based on ability to pack shadow-maps
    // 1. calculate area + sort in descending order
    // const	u16		smap_unassigned		= u16(-1);
    {
        xr_vector<light*>& source = LP.v_shadowed;
        for (u32 it = 0; it < source.size(); it++)
        {
            light* L = source[it];
            if (!L->vis.visible)
            {
                source.erase(source.begin() + it);
                it--;
            }
            else
            {
                LR.compute_xf_spot(L);
            }
        }
    }

    // 2. refactor - infact we could go from the backside and sort in ascending order
    {
        xr_vector<light*>& source = LP.v_shadowed;
        xr_vector<light*> refactored;
        refactored.reserve(source.size());
        u32 total = source.size();

        for (u16 smap_ID = 0; refactored.size() != total; smap_ID++)
        {
            LP_smap_pool.initialize(o.smapsize);
            std::ranges::sort(source, [](const light* l1, const light* l2) {
                const u32 a0 = l1->X.S.size;
                const u32 a1 = l2->X.S.size;
                return a0 > a1; // reverse -> descending
            });
            for (u32 test = 0; test < source.size(); test++)
            {
                light* L = source[test];
                SMAP_Rect R;
                if (LP_smap_pool.push(R, L->X.S.size))
                {
                    // OK
                    L->X.S.posX = R.min.x;
                    L->X.S.posY = R.min.y;
                    L->vis.smap_ID = smap_ID;
                    refactored.push_back(L);
                    source.erase(source.begin() + test);
                    test--;
                }
            }
        }

        // save (lights are popped from back)
        std::reverse(refactored.begin(), refactored.end());
        LP.v_shadowed = refactored;
    }

    PIX_EVENT(SHADOWED_LIGHTS);

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
    while (!LP.v_shadowed.empty())
    {
        // if (has_spot_shadowed)
        xr_vector<light*> L_spot_s;
        stats.s_used++;

        // generate spot shadowmap
        Target->phase_smap_spot_clear();

        auto& source = LP.v_shadowed;
        const u16 sid = source.back()->vis.smap_ID;

        while (!source.empty())
        {
            light* L = source.back();

            if (L->vis.smap_ID != sid)
                break;

            source.pop_back();

            if (L->spatial.sector_id == INVALID_SECTOR_ID) [[unlikely]]
            {
                Msg("!![%s] Invalid L->spatial.sector_id in Light [%p]", __FUNCTION__, L);
                continue;
            }

            Lights_LastFrame.push_back(L);

            // render
            PIX_EVENT(SHADOWED_LIGHTS_RENDER_SUBSPACE);
            L->svis.begin();
            dsgraph.r_pmask(true, false);
            CFrustum temp;
            temp.CreateFromMatrix(L->X.S.combine, FRUSTUM_P_ALL & ~FRUSTUM_P_NEAR);
            dsgraph.build_subspace(L->spatial.sector_id, temp, L->X.S.combine, L->position, true);

            bool bNormal = dsgraph.mapNormalPasses[0][0].size() || dsgraph.mapMatrixPasses[0][0].size();
            VERIFY(!(dsgraph.mapNormalPasses[1][0].size() || dsgraph.mapMatrixPasses[1][0].size() || dsgraph.mapSorted.size()));

            if (bNormal)
            {
                stats.s_merged++;
                L_spot_s.push_back(L);
                Target->phase_smap_spot(L);
                RCache.set_xform_world(Fidentity);
                RCache.set_xform_view(L->X.S.view);
                RCache.set_xform_project(L->X.S.project);
                dsgraph.render_graph(0);
                if (Details)
                {
                    if (check_grass_shadow(L, ViewBase))
                    {
                        Details->fade_distance = -1; // Use light position to calc "fade"
                        Details->light_position.set(L->position);
                        Details->Render(true);
                    }
                }
            }
            else
            {
                stats.s_finalclip++;
            }
            L->svis.end();
            dsgraph.r_pmask(true, false);
        }

        PIX_EVENT(UNSHADOWED_LIGHTS);

        //		switch-to-accumulator
        Target->phase_accumulator();

        PIX_EVENT(POINT_LIGHTS);

        //		if (has_point_unshadowed)	-> 	accum point unshadowed
        if (!LP.v_point.empty())
        {
            light* L = LP.v_point.back();
            LP.v_point.pop_back();
            if (L->vis.visible)
                Target->accum_point(L);
        }

        PIX_EVENT(SPOT_LIGHTS);

        //		if (has_spot_unshadowed)	-> 	accum spot unshadowed
        if (!LP.v_spot.empty())
        {
            light* L = LP.v_spot.back();
            LP.v_spot.pop_back();
            if (L->vis.visible)
            {
                LR.compute_xf_spot(L);
                Target->accum_spot(L);
            }
        }

        PIX_EVENT(SPOT_LIGHTS_ACCUM_VOLUMETRIC);

        //		if (was_spot_shadowed)		->	accum spot shadowed
        if (!L_spot_s.empty())
        {
            PIX_EVENT(ACCUM_SPOT);
            for (u32 it = 0; it < L_spot_s.size(); it++)
                Target->accum_spot(L_spot_s[it]);

            PIX_EVENT(ACCUM_VOLUMETRIC);
            if (ps_r2_ls_flags.is(R2FLAG_VOLUMETRIC_LIGHTS))
            {
                // Current Resolution
                float w = float(Device.dwWidth);
                float h = float(Device.dwHeight);

                // Adjust resolution
                if (o.ssfx_volumetric)
                    Target->set_viewport_size(HW.pContext, w / 8, h / 8);

                for (u32 it = 0; it < L_spot_s.size(); it++)
                {
                    Target->accum_volumetric(L_spot_s[it]);
                }

                // Restore resolution
                if (o.ssfx_volumetric)
                    Target->set_viewport_size(HW.pContext, w, h);
            }

            L_spot_s.clear();
        }
    }

    PIX_EVENT(POINT_LIGHTS_ACCUM);
    // Point lighting (unshadowed, if left)
    if (!LP.v_point.empty())
    {
        xr_vector<light*>& Lvec = LP.v_point;
        for (u32 pid = 0; pid < Lvec.size(); pid++)
        {
            if (Lvec[pid]->vis.visible)
                Target->accum_point(Lvec[pid]);
        }
        Lvec.clear();
    }

    PIX_EVENT(SPOT_LIGHTS_ACCUM);
    // Spot lighting (unshadowed, if left)
    if (!LP.v_spot.empty())
    {
        xr_vector<light*>& Lvec = LP.v_spot;
        for (u32 pid = 0; pid < Lvec.size(); pid++)
        {
            if (Lvec[pid]->vis.visible)
            {
                LR.compute_xf_spot(Lvec[pid]);
                Target->accum_spot(Lvec[pid]);
            }
        }
        Lvec.clear();
    }
}
