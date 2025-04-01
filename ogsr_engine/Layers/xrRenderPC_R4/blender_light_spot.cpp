#include "stdafx.h"

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot() { description.CLS = 0; }
CBlender_accum_spot::~CBlender_accum_spot() {}

void CBlender_accum_spot::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    constexpr BOOL blend = true;
    constexpr D3DBLEND dest = blend ? D3DBLEND_ONE : D3DBLEND_ZERO;

    switch (C.iElement)
    {
    case SE_L_FILL: break;
    case SE_L_UNSHADOWED: // unshadowed
        C.r_Pass("accum_volume", "accum_spot_unshadowed_nomsaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");

        C.r_End();
        break;
    case SE_L_NORMAL: // normal
        C.r_Pass("accum_volume", "accum_spot_normal_nomsaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Texture("s_ssfx_sss", r2_RT_ssfx_sss_tmp);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_End();
        break;
    case SE_L_FULLSIZE: // normal-fullsize
        C.r_Pass("accum_volume", "accum_spot_fullsize_nomsaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Texture("s_ssfx_sss", r2_RT_ssfx_sss_tmp);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");
        C.r_End();
        break;
    }
}

CBlender_accum_spot_msaa::CBlender_accum_spot_msaa() { description.CLS = 0; }
CBlender_accum_spot_msaa::~CBlender_accum_spot_msaa() {}

void CBlender_accum_spot_msaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    constexpr BOOL blend = true;
    constexpr D3DBLEND dest = blend ? D3DBLEND_ONE : D3DBLEND_ZERO;

    if (Name)
        ::Render->m_MSAASample = atoi(Definition);
    else
        ::Render->m_MSAASample = -1;

    switch (C.iElement)
    {
    case SE_L_FILL: break;
    case SE_L_UNSHADOWED: // unshadowed
        C.r_Pass("accum_volume", "accum_spot_unshadowed_msaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");

        C.r_End();
        break;
    case SE_L_NORMAL: // normal
        C.r_Pass("accum_volume", "accum_spot_normal_msaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Texture("s_ssfx_sss", r2_RT_ssfx_sss_tmp);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");

        C.r_End();
        break;
    case SE_L_FULLSIZE: // normal-fullsize
        C.r_Pass("accum_volume", "accum_spot_fullsize_msaa", false, FALSE, FALSE, blend, D3DBLEND_ONE, dest);

        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);
        C.r_dx10Texture("s_accumulator", r2_RT_accum);

        C.r_dx10Texture("s_ssfx_sss", r2_RT_ssfx_sss_tmp);

        C.r_dx10Sampler("smp_nofilter");
        C.r_dx10Sampler("smp_material");
        C.r_dx10Sampler("smp_rtlinear");
        jitter(C);
        C.r_dx10Sampler("smp_smap");
        C.r_End();
        break;
    }
    ::Render->m_MSAASample = -1;
}

CBlender_accum_volumetric_msaa::CBlender_accum_volumetric_msaa() { description.CLS = 0; }
CBlender_accum_volumetric_msaa::~CBlender_accum_volumetric_msaa() {}

void CBlender_accum_volumetric_msaa::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    if (Name)
        ::Render->m_MSAASample = atoi(Definition);
    else
        ::Render->m_MSAASample = -1;

    switch (C.iElement)
    {
    case 0: // masking
        C.r_Pass("accum_volumetric", "accum_volumetric_msaa", false, FALSE, FALSE);

        C.r_dx10Texture("s_lmap", C.L_textures[0]);
        C.r_dx10Texture("s_smap", r2_RT_smap_depth);
        C.r_dx10Texture("s_noise", "fx\\fx_noise");

        C.r_dx10Sampler("smp_rtlinear");
        C.r_dx10Sampler("smp_smap");
        C.r_dx10Sampler("smp_linear");
        C.r_End();
        break;
    }
    ::Render->m_MSAASample = -1;
}
