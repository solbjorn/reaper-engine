#include "stdafx.h"

// Screen Space Sun Shafts
void CRenderTarget::PhaseSSSS()
{
    // Constants
    float intensity = g_pGamePersistent->Environment().CurrentEnv->m_fSunShaftsIntensity *
        3.3f; // Домножаем, т.к. интенсивность плоских саншафтов примерно соответствует объемным как 1 к 0.3. А конфиги погоды рассчитаны на объемные лучи же.

    Fvector4 params{0.0f, 0.0f, 0.0f, 0.0f};
    string_unordered_map<const char*, Fvector4*> consts{{"ssss_params", &params}};

    u32 mode = ps_r_sunshafts_mode & SS_SS_MASK;
    if (mode == SS_SS_MANOWAR)
    {
        // Mask
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_SunShaftsMask, s_ssss_mrmnwar->E[0]);

        // Smoothed mask
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[1]);

        // Pass 0
        params.x = ps_r_prop_ss_sample_step_phase0;
        params.y = ps_r_prop_ss_radius;
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_SunShaftsPass0, s_ssss_mrmnwar->E[2], &consts);

        // Pass 1
        params.x = ps_r_prop_ss_sample_step_phase1;
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[3], &consts);

        // Combine
        params.x = intensity;
        params.y = ps_r_prop_ss_blend;

        ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

        RenderScreenQuad(Device.dwWidth, Device.dwHeight, dest_rt, s_ssss_mrmnwar->E[4], &consts);
        HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
    }
    else if (mode == SS_SS_OGSE)
    {
        // ***MASK GENERATION***
        // In this pass generates geometry mask
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_sunshafts_0, s_ssss_ogse->E[0]);

        // ***FIRST PASS***
        // First blurring pass
        params.x = intensity;
        params.y = ps_r_ss_sunshafts_length;
        params.z = 1.0f;
        params.w = ps_r_ss_sunshafts_radius;
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_sunshafts_1, s_ssss_ogse->E[1], &consts);

        //***SECOND PASS***
        // Second blurring pass
        params.z = 0.7f;
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_sunshafts_0, s_ssss_ogse->E[2], &consts);

        //***THIRD PASS***
        // Third blurring pass
        params.z = 0.3f;
        RenderScreenQuad(Device.dwWidth, Device.dwHeight, rt_sunshafts_1, s_ssss_ogse->E[3], &consts);

        //***BLEND PASS***
        // Combining sunshafts texture and image for further processing
        params.z = 0.0f;

        ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;

        RenderScreenQuad(Device.dwWidth, Device.dwHeight, dest_rt, s_ssss_ogse->E[4], &consts);
        HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
    }
}
