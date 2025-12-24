#include "stdafx.h"

#include "xr_task.h"

namespace
{
float g_fSCREEN;
}

void CRender::Calculate()
{
    XR_TRACY_ZONE_SCOPED();

    auto& dsgraph = get_imm_context();

    // Transfer to global space to avoid deep pointer access
    float fov_factor = _sqr(90.f / Device.fFOV);
    g_fSCREEN = float(Target->get_width(dsgraph.cmd_list) * Target->get_height(dsgraph.cmd_list)) * fov_factor * (EPS_S + ps_r__LOD);
    r_ssaDISCARD = _sqr(ps_r__ssaDISCARD) / g_fSCREEN;
    r_ssaLOD_A = _sqr(ps_r2_ssaLOD_A / 3) / g_fSCREEN;
    r_ssaLOD_B = _sqr(ps_r2_ssaLOD_B / 3) / g_fSCREEN;
    r_ssaGLOD_start = _sqr(ps_r__GLOD_ssa_start / 3) / g_fSCREEN;
    r_ssaGLOD_end = _sqr(ps_r__GLOD_ssa_end / 3) / g_fSCREEN;
    r_dtex_range = ps_r2_df_parallax_range * g_fSCREEN / (1024.f * 768.f);

    if (m_bFirstFrameAfterReset)
        return;

    // Detect camera-sector
    if (!Device.vCameraDirectionSaved.similar(Device.vCameraPosition, EPS_L))
    {
        XR_TRACY_ZONE_SCOPED();

        const auto sector_id = dsgraph.detect_sector(Device.vCameraPosition);
        if (sector_id != INVALID_SECTOR_ID)
        {
            if (sector_id != last_sector_id)
                g_pGamePersistent->OnSectorChanged(sector_id);

            last_sector_id = sector_id;
        }
    }

    //
    Lights.Update();

    main_tg->run([this] {
        XR_TRACY_ZONE_SCOPED();

        auto& dsgraph = get_imm_context();
        VERIFY(dsgraph.mapDistort.empty() && dsgraph.mapHUDDistort.empty());

        // Check if we touch some light even trough portal
        static xr_vector<ISpatial*> spatial_lights;
        g_SpatialSpace->q_sphere(spatial_lights, 0, STYPE_LIGHTSOURCE, Device.vCameraPosition, EPS_L);

        for (auto spatial : spatial_lights)
        {
            const auto& entity_pos = spatial->spatial_sector_point();

            spatial->spatial_updatesector(dsgraph.detect_sector(entity_pos));
            if (spatial->spatial.sector_id == INVALID_SECTOR_ID)
                continue; // disassociated from S/P structure

            VERIFY(spatial->spatial.type & STYPE_LIGHTSOURCE);
            // lightsource
            light* L = (light*)spatial->dcast_Light();
            VERIFY(L);
            Lights.add_light(L);
        }

        Device.Statistic->RenderCALC.Begin();
        dsgraph.r_pmask(true, true, true);
        dsgraph.build_subspace(last_sector_id, ViewBase);
        Device.Statistic->RenderCALC.End();
    });

    rain_run();
    sun_run();
}

void CRender::main_sync() { main_tg->wait(); }
