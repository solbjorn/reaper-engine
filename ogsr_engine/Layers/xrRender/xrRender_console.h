#ifndef xrRender_consoleH
#define xrRender_consoleH

#ifndef ECORE_API
#define ECORE_API
#endif

// Common
extern ECORE_API Fvector4 ps_ssfx_rain_drops_setup;
extern ECORE_API int ps_ssfx_terrain_grass_align;
extern ECORE_API int ps_ssfx_terrain_pom_refine;
extern ECORE_API int ps_ssfx_pom_refine;
extern ECORE_API int ps_ssfx_terrain_grass_align;
extern ECORE_API float ps_ssfx_terrain_grass_slope;
extern ECORE_API int ps_ssfx_bloom_use_presets;
extern ECORE_API Fvector4 ps_ssfx_bloom_2;
extern ECORE_API Fvector4 ps_ssfx_sss_quality;
extern ECORE_API Fvector4 ps_ssfx_sss;

extern ECORE_API int ps_ssfx_il_quality;
extern ECORE_API int ps_ssfx_ao_quality;
extern ECORE_API Fvector3 ps_ssfx_water_quality;
extern ECORE_API Fvector4 ps_ssfx_il;
extern ECORE_API Fvector4 ps_ssfx_ao;
extern ECORE_API Fvector4 ps_ssfx_water;

extern ECORE_API int ps_ssfx_ssr_quality;
extern ECORE_API Fvector4 ps_ssfx_ssr;
extern ECORE_API Fvector4 ps_ssfx_ssr_2;
extern ECORE_API Fvector4 ps_ssfx_terrain_quality;
extern ECORE_API Fvector3 ps_ssfx_shadows;
extern ECORE_API Fvector4 ps_ssfx_volumetric;

extern ECORE_API Fvector4 ps_ssfx_wind_grass;
extern ECORE_API Fvector4 ps_ssfx_wind_trees;

extern ECORE_API Fvector4 ps_ssfx_rain_1;
extern ECORE_API Fvector4 ps_ssfx_rain_2;
extern ECORE_API Fvector4 ps_ssfx_rain_3;
extern ECORE_API Fvector4 ps_ssfx_grass_shadows;
extern ECORE_API Fvector3 ps_ssfx_shadow_cascades;
extern ECORE_API Fvector4 ps_ssfx_grass_interactive;
extern ECORE_API Fvector4 ps_ssfx_int_grass_params_1;
extern ECORE_API Fvector4 ps_ssfx_int_grass_params_2;

extern ECORE_API u32 r2_SmapSize;

extern ECORE_API u32 ps_r_sun_shafts;
extern ECORE_API u32 ps_r_sunshafts_mode;
extern ECORE_API float ps_r_ss_sunshafts_length;
extern ECORE_API float ps_r_ss_sunshafts_radius;
extern ECORE_API float ps_r_prop_ss_radius;
extern ECORE_API float ps_r_prop_ss_blend;
extern ECORE_API float ps_r_prop_ss_sample_step_phase0;
extern ECORE_API float ps_r_prop_ss_sample_step_phase1;

extern ECORE_API u32 ps_r_ssao; //	=	0;

enum : u32
{
    AO_MODE_SSDO,
    AO_MODE_GTAO
};
extern u32 ps_r_ao_mode;

extern ECORE_API u32 ps_r_sun_quality; //	=	0;
extern ECORE_API u32 ps_smaa_quality;
extern ECORE_API u32 ps_r3_msaa; //	=	0;

extern ECORE_API int ps_r__LightSleepFrames;

extern ECORE_API float ps_r__Detail_l_ambient;
extern ECORE_API float ps_r__Detail_l_aniso;

extern ECORE_API float ps_r__Tree_w_rot;
extern ECORE_API float ps_r__Tree_w_speed;
extern ECORE_API float ps_r__Tree_w_amp;
extern ECORE_API float ps_r__Tree_SBC; // scale bias correct
extern ECORE_API Fvector ps_r__Tree_Wave;

extern ECORE_API float ps_r__WallmarkTTL;
extern ECORE_API float ps_r__WallmarkSHIFT;
extern ECORE_API float ps_r__WallmarkSHIFT_V;

extern ECORE_API float ps_r__GLOD_ssa_start;
extern ECORE_API float ps_r__GLOD_ssa_end;
extern ECORE_API float ps_r__LOD;
//.extern ECORE_API	float		ps_r__LOD_Power		;
extern ECORE_API float ps_r__ssaDISCARD;
extern ECORE_API float ps_r__ssaDONTSORT;
extern ECORE_API float ps_r__ssaHZBvsTEX;
extern ECORE_API int ps_r__tf_Anisotropic;
extern ECORE_API float ps_r__tf_Mipbias;

// R1
extern ECORE_API float ps_r1_ssaLOD_A;
extern ECORE_API float ps_r1_ssaLOD_B;
extern ECORE_API float ps_r1_lmodel_lerp;
extern ECORE_API float ps_r1_dlights_clip;
extern ECORE_API float ps_r1_pps_u;
extern ECORE_API float ps_r1_pps_v;

// R1-specific
extern ECORE_API int ps_r1_GlowsPerFrame; // r1-only

extern ECORE_API float ps_r1_fog_luminance; // 1.f r1-only
extern ECORE_API int ps_r1_SoftwareSkinning; // r1-only

// R2
extern ECORE_API float ps_r2_ssaLOD_A;
extern ECORE_API float ps_r2_ssaLOD_B;

// R2-specific
extern ECORE_API Flags32 ps_r2_ls_flags; // r2-only
extern ECORE_API Flags32 ps_r2_ls_flags_ext;
extern ECORE_API float ps_r2_df_parallax_h; // r2-only
extern ECORE_API float ps_r2_df_parallax_range; // r2-only
extern ECORE_API float ps_r2_gmaterial; // r2-only
extern ECORE_API float ps_r2_tonemap_middlegray; // r2-only
extern ECORE_API float ps_r2_tonemap_adaptation; // r2-only
extern ECORE_API float ps_r2_tonemap_low_lum; // r2-only
extern ECORE_API float ps_r2_tonemap_amount; // r2-only
extern ECORE_API float ps_r2_ls_bloom_kernel_scale; // r2-only	// gauss
extern ECORE_API float ps_r2_ls_bloom_kernel_g; // r2-only	// gauss
extern ECORE_API float ps_r2_ls_bloom_kernel_b; // r2-only	// bilinear
extern ECORE_API float ps_r2_ls_bloom_threshold; // r2-only
extern ECORE_API float ps_r2_ls_bloom_speed; // r2-only
extern ECORE_API float ps_r2_ls_dsm_kernel; // r2-only
extern ECORE_API float ps_r2_ls_psm_kernel; // r2-only
extern ECORE_API float ps_r2_ls_ssm_kernel; // r2-only
extern ECORE_API float ps_r2_mblur; // .5f
extern ECORE_API int ps_r2_GI_depth; // 1..5
extern ECORE_API int ps_r2_GI_photons; // 8..256
extern ECORE_API float ps_r2_GI_clip; // EPS
extern ECORE_API float ps_r2_GI_refl; // .9f
extern ECORE_API float ps_r2_ls_depth_scale; // 1.0f
extern ECORE_API float ps_r2_ls_depth_bias; // -0.0001f
extern ECORE_API float ps_r2_ls_squality; // 1.0f
extern ECORE_API float ps_r2_sun_near; // 10.0f
extern ECORE_API float ps_r2_sun_near_border; // 1.0f
extern ECORE_API float ps_r2_sun_tsm_projection; // 0.2f
extern ECORE_API float ps_r2_sun_tsm_bias; // 0.0001f
extern ECORE_API float ps_r2_sun_depth_far_scale; // 1.00001f
extern ECORE_API float ps_r2_sun_depth_far_bias; // -0.0001f
extern ECORE_API float ps_r2_sun_depth_near_scale; // 1.00001f
extern ECORE_API float ps_r2_sun_depth_near_bias; // -0.0001f
extern ECORE_API float ps_r2_sun_lumscale; // 0.5f
extern ECORE_API float ps_r2_sun_lumscale_hemi; // 1.0f
extern ECORE_API float ps_r2_sun_lumscale_amb; // 1.0f
extern ECORE_API float ps_r2_zfill; // .1f

extern ECORE_API float ps_r2_dhemi_sky_scale; // 1.5f
extern ECORE_API float ps_r2_dhemi_light_scale; // 1.f
extern ECORE_API float ps_r2_dhemi_light_flow; // .1f
extern ECORE_API int ps_r2_dhemi_count; // 5
extern ECORE_API float ps_r2_slight_fade; // 1.f
extern ECORE_API int ps_r2_wait_sleep;
////lvutner
extern ECORE_API Fvector4 ps_r2_mask_control; // r2-only
extern ECORE_API Fvector ps_r2_drops_control; // r2-only

extern ECORE_API Fvector4 ps_r__color_gamma;
extern ECORE_API Fvector4 ps_r__color_slope;
extern ECORE_API Fvector4 ps_r__color_offset;
extern ECORE_API Fvector4 ps_r__color_power;
extern ECORE_API Fvector4 ps_r__color_saturation;
extern ECORE_API Fvector4 ps_r__color_contrast;
extern ECORE_API Fvector4 ps_r__color_red;
extern ECORE_API Fvector4 ps_r__color_green;
extern ECORE_API Fvector4 ps_r__color_blue;

extern ECORE_API float ps_r3_dyn_wet_surf_near; // 10.0f
extern ECORE_API float ps_r3_dyn_wet_surf_far; // 30.0f
extern ECORE_API int ps_r3_dyn_wet_surf_sm_res; // 256

// textures
#ifdef DEBUG
extern ECORE_API int psTextureLOD;
extern ECORE_API u32 psCurrentBPP;
#else
constexpr int psTextureLOD = 0;
constexpr u32 psCurrentBPP = 32;
#endif

extern ECORE_API int opt_static;
extern ECORE_API int opt_dynamic;

enum
{
    R2FLAG_SUN = (1 << 0),
    R2FLAG_SUN_FOCUS = (1 << 1),
    R2FLAG_SUN_TSM = (1 << 2),
    R2FLAG_SUN_DETAILS = (1 << 3),
    R2FLAG_TONEMAP = (1 << 4),
    R2FLAG_EXP_MT_DETAILS = 1 << 5,
#ifdef DEBUG
    R2FLAG_GI = (1 << 6),
#endif
/* 1 << 7, */
#ifdef DEBUG
    R2FLAG_GLOBALMATERIAL = (1 << 8),
#endif
    R2FLAG_ZFILL = (1 << 9),
    /* 1 << 10, */
    /* 1 << 11, */

    /* 1 << 12, */
    /* 1 << 13, */
    R2FLAG_EXP_DONT_TEST_SHADOWED = (1 << 14),

    /* 1 << 15, */
    /* 1 << 16, */

    R2FLAG_EXP_MT_CALC = (1 << 17),
    R2FLAG_EXP_MT_SUN = 1 << 18,
    R2FLAG_EXP_MT_RAIN = 1 << 19,

    R2FLAG_VOLUMETRIC_LIGHTS = (1 << 20),
    R2FLAG_STEEP_PARALLAX = (1 << 21),
    R2FLAG_DOF = (1 << 22),

    R2FLAG_MBLUR = 1 << 23,
    RFLAG_MT_PARTICLES = 1 << 24,

    R3FLAG_DYN_WET_SURF = (1 << 25),
    R3FLAG_VOLUMETRIC_SMOKE = (1 << 26),

    /* 1 << 27, */
    /* 1 << 28, */
    R2FLAG_TERRAIN_PREPASS = 1 << 29,
    /* 1 << 30, */
};

enum
{
    /* 1 << 0, */
    /* 1 << 1, */
    /* 1 << 2, */
    /* 1 << 3, */
    /* 1 << 4, */
    R2FLAGEXT_ENABLE_TESSELLATION = (1 << 5),
    R2FLAGEXT_WIREFRAME = (1 << 6),
    R_FLAGEXT_HOM_DEPTH_DRAW = (1 << 7),
    R2FLAGEXT_SUN_ZCULLING = (1 << 8),
    /* 1 << 9, */
    /* 1 << 10, */
    R2FLAGEXT_DISABLE_HOM = 1 << 11,
    /* 1 << 12, */
    /* 1 << 13, */
    R2FLAGEXT_ACTOR_SHADOW = 1 << 14,
    /* 1 << 15, */
    /* 1 << 16, */
    /* 1 << 17, */
    /* 1 << 18, */
    R2FLAGEXT_MT_TEXLOAD = 1 << 19,
    R2FLAGEXT_FONT_SHADOWS = 1 << 20,
};

// Sunshafts types
enum
{
    SS_OFF = 0,

    SS_SS_OGSE = 1,
    SS_SS_MANOWAR = 2,
    SS_SS_MASK = 3,

    SS_VOLUMETRIC = 4,

    SS_COMBINED_OGSE = SS_VOLUMETRIC | SS_SS_OGSE,
    SS_COMBINED_MANOWAR = SS_VOLUMETRIC | SS_SS_MANOWAR,
};

// Rezy: cleanup flags
extern Flags32 psDeviceFlags2;

extern void xrRender_initconsole();

#ifndef XRRENDER_STATIC
extern BOOL xrRender_test_hw();
#endif

#endif
