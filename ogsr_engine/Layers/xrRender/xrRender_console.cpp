#include "stdafx.h"

#include "xrRender_console.h"
#include "dxRenderDeviceRender.h"

u32 r2_SmapSize = 2048;
static constexpr xr_token SmapSizeToken[] = {{"1536x1536", 1536},
                                             {"2048x2048", 2048},
                                             {"2560x2560", 2560},
                                             {"3072x3072", 3072},
                                             {"4096x4096", 4096},
                                             {"6144x6144", 6144},
                                             {"8192x8192", 8192},
                                             //{ "16384x16384", 16384 },
                                             {nullptr, 0}};

u32 ps_r_sunshafts_mode = SS_COMBINED_MANOWAR;
static constexpr xr_token sunshafts_mode_token[] = {{"st_opt_off", SS_OFF},
                                                    {"volumetric", SS_VOLUMETRIC},
                                                    {"ss_ogse", SS_SS_OGSE},
                                                    {"ss_manowar", SS_SS_MANOWAR},
                                                    {"combined_ogse", SS_COMBINED_OGSE},
                                                    {"combined_manowar", SS_COMBINED_MANOWAR},
                                                    {nullptr, 0}};
// Sunshafts
u32 ps_r_sun_shafts = 3;
float ps_r_ss_sunshafts_length = 0.9f; // 1.0f;
float ps_r_ss_sunshafts_radius = 2.f; // 1.0f;
float ps_r_prop_ss_radius = 1.56f;
float ps_r_prop_ss_blend = 0.25f; // 0.066f;
float ps_r_prop_ss_sample_step_phase0 = 0.09f;
float ps_r_prop_ss_sample_step_phase1 = 0.07f;

u32 ps_Preset = 2;
static constexpr xr_token qpreset_token[] = {{"Minimum", 0}, {"Low", 1}, {"Default", 2}, {"High", 3}, {"Extreme", 4}, {nullptr, 0}};

u32 ps_r_ssao = 3;
static constexpr xr_token qssao_token[] = {{"st_opt_off", 0}, {"st_opt_low", 1}, {"st_opt_medium", 2}, {"st_opt_high", 3}, {nullptr, 0}};

u32 ps_smaa_quality = 3;
static constexpr xr_token smaa_quality_token[] = {{"st_opt_off", 0}, {"st_opt_low", 1}, {"st_opt_medium", 2}, {"st_opt_high", 3}, {"st_opt_ultra", 4}, {nullptr, 0}};

u32 ps_r_ao_mode = AO_MODE_SSDO;
static constexpr xr_token ao_mode_token[] = {{"st_ssdo", AO_MODE_SSDO}, {"st_gtao", AO_MODE_GTAO}, {nullptr, 0}};

u32 ps_r_sun_quality = 1; //	=	0;
static constexpr xr_token qsun_quality_token[] = {{"st_opt_low", 0}, {"st_opt_medium", 1}, {"st_opt_high", 2}, {"st_opt_ultra", 3}, {"st_opt_extreme", 4}, {nullptr, 0}};

u32 ps_r3_msaa = 0; //	=	0;
static constexpr xr_token qmsaa_token[] = {{"st_opt_off", 0}, {"2x", 1}, {"4x", 2}, {"8x", 3}, {nullptr, 0}};

// Common
extern int psSkeletonUpdate;
extern float r__dtex_range;

int ps_r__LightSleepFrames = 10;

float ps_r__Detail_l_ambient = 0.9f;
float ps_r__Detail_l_aniso = 0.25f;
float ps_r__Detail_rainbow_hemi = 0.75f;

float ps_r__Tree_w_rot = 10.0f;
float ps_r__Tree_w_speed = 1.00f;
float ps_r__Tree_w_amp = 0.005f;
Fvector ps_r__Tree_Wave = {.1f, .01f, .11f};
float ps_r__Tree_SBC = 1.5f; // scale bias correct

float ps_r__WallmarkTTL = 50.f;
float ps_r__WallmarkSHIFT = 0.0001f;
float ps_r__WallmarkSHIFT_V = 0.0001f;

float ps_r__GLOD_ssa_start = 256.f;
float ps_r__GLOD_ssa_end = 64.f;
float ps_r__LOD = 1.2f;
//. float		ps_r__LOD_Power				=  1.5f	;
float ps_r__ssaDISCARD = 3.5f; // RO
float ps_r__ssaDONTSORT = 32.f; // RO
float ps_r__ssaHZBvsTEX = 96.f; // RO

int ps_r__tf_Anisotropic = 16;
float ps_r__tf_Mipbias = -0.5f;

// R1
float ps_r1_ssaLOD_A = 64.f;
float ps_r1_ssaLOD_B = 48.f;
float ps_r1_lmodel_lerp = 0.1f;
float ps_r1_dlights_clip = 40.f;
float ps_r1_pps_u = 0.f;
float ps_r1_pps_v = 0.f;

// R1-specific
int ps_r1_GlowsPerFrame = 16; // r1-only
float ps_r1_fog_luminance = 1.1f; // r1-only
int ps_r1_SoftwareSkinning = 0; // r1-only

// R2
float ps_r2_ssaLOD_A = 64.f;
float ps_r2_ssaLOD_B = 48.f;

// R2-specific
Flags32 ps_r2_ls_flags = {R2FLAG_SUN | R2FLAG_MBLUR | R3FLAG_DYN_WET_SURF | R3FLAG_VOLUMETRIC_SMOKE | R2FLAG_DOF | R2FLAG_STEEP_PARALLAX | R2FLAG_SUN_FOCUS | R2FLAG_SUN_TSM |
                          R2FLAG_TONEMAP | R2FLAG_VOLUMETRIC_LIGHTS};

Flags32 ps_r2_ls_flags_ext = {R2FLAGEXT_ENABLE_TESSELLATION | R2FLAGEXT_FONT_SHADOWS};

BOOL ps_no_scale_on_fade = 0; // Alundaio
float ps_r2_df_parallax_h = 0.02f;
float ps_r2_df_parallax_range = 75.f;
float ps_r2_tonemap_middlegray = 1.f; // r2-only
float ps_r2_tonemap_adaptation = 1.f; // r2-only
float ps_r2_tonemap_low_lum = .4f; // r2-only
float ps_r2_tonemap_amount = 0.7f; // r2-only
float ps_r2_ls_bloom_kernel_g = 3.f; // r2-only
float ps_r2_ls_bloom_kernel_b = .7f; // r2-only
float ps_r2_ls_bloom_speed = 100.f; // r2-only
float ps_r2_ls_bloom_kernel_scale = .7f; // r2-only	// gauss
float ps_r2_ls_dsm_kernel = .7f; // r2-only
float ps_r2_ls_psm_kernel = .7f; // r2-only
float ps_r2_ls_ssm_kernel = .7f; // r2-only
float ps_r2_ls_bloom_threshold = 1.f; // r2-only
float ps_r2_mblur = .0f; // .5f
int ps_r2_GI_depth = 1; // 1..5
int ps_r2_GI_photons = 16; // 8..64
float ps_r2_GI_clip = EPS_L; // EPS
float ps_r2_GI_refl = .9f; // .9f
float ps_r2_ls_depth_scale = 1.00001f; // 1.00001f

float ps_r2_ls_depth_bias = -0.001f;
float ps_r2_ls_squality = 1.0f; // Base shadow map quality. The higher the value the higher the base shadow resolution and resolution through distance.

float ps_r2_sun_tsm_projection = 0.3f; // 0.18f
float ps_r2_sun_tsm_bias = -0.01f; //
float ps_r2_sun_near = 20.f; // 12.0f

extern float OLES_SUN_LIMIT_27_01_07; //	actually sun_far

float ps_r2_sun_near_border = 0.75f; // 1.0f
float ps_r2_sun_depth_far_scale = 1.00000f; // 1.00001f
float ps_r2_sun_depth_far_bias = -0.00002f; // -0.0000f
float ps_r2_sun_depth_near_scale = 1.0000f; // 1.00001f
float ps_r2_sun_depth_near_bias = 0.00001f; // -0.00005f
float ps_r2_sun_lumscale = 1.0f; // 1.0f
float ps_r2_sun_lumscale_hemi = 1.0f; // 1.0f
float ps_r2_sun_lumscale_amb = 1.0f;
float ps_r2_gmaterial = 2.2f; //
float ps_r2_zfill = 0.25f; // .1f

float ps_r2_dhemi_sky_scale = 0.08f; // 1.5f
float ps_r2_dhemi_light_scale = 0.2f;
float ps_r2_dhemi_light_flow = 0.1f;
int ps_r2_dhemi_count = 5; // 5
int ps_r2_wait_sleep = 0;
int ps_lens_flare{};

float ps_r2_lt_smooth = 1.f; // 1.f
float ps_r2_slight_fade = 2.0f; // 1.f
///////lvutner
Fvector4 ps_r2_mask_control = {.0f, .0f, .0f, .0f}; // r2-only
Fvector ps_r2_drops_control = {.0f, 1.15f, .0f}; // r2-only

float ps_particle_update_coeff = 0.3f;

Fvector4 ps_r__color_gamma{0, 0, 0, 1};
Fvector4 ps_r__color_slope{0, 0, 0, 1};
Fvector4 ps_r__color_offset{0, 0, 0, 0};
Fvector4 ps_r__color_power{0, 0, 0, 1};
Fvector4 ps_r__color_saturation{0, 0, 0, 1};
Fvector4 ps_r__color_contrast{0, 0, 0, 0.7};
Fvector4 ps_r__color_red{1, 0, 0, 0};
Fvector4 ps_r__color_green{0, 1, 0, 0};
Fvector4 ps_r__color_blue{0, 0, 1, 0};

// Screen Space Shaders Stuff
int ps_ssfx_pom_refine = 0;
Fvector4 ps_ssfx_pom = {16, 12, 0.035f, 0.4f}; // Samples , Range, Height, AO

int ps_ssfx_terrain_grass_align = 0; // Grass align
float ps_ssfx_terrain_grass_slope = 1.0f; // Grass slope limit // Recommended 0.3f
Fvector4 ps_ssfx_terrain_pom = {12, 20, 0.04f, 1.0f}; // Samples, Range, Height, Water Limit
int ps_ssfx_terrain_pom_refine = 0;

int ps_ssfx_bloom_use_presets = 0;
Fvector4 ps_ssfx_bloom_1 = {3.5f, 3.0f, 0.0f, 0.6f}; // Threshold, Exposure, -, Sky
Fvector4 ps_ssfx_bloom_2 = {3.0f, 1.5f, 1.5f, 1.0f}; // Blur Radius, Vibrance, Lens, Dirt

Fvector4 ps_ssfx_sss_quality = {12.0f, 4.0f, 1.0f, 1.0f}; // Dir Samples, Omni Samples, Dir Enable, Omni Enable
Fvector4 ps_ssfx_sss = {1.0f, 1.0f, 1.0f, 0.0f}; // Dir Len, Omni Len, Grass shadows, -

float ps_ssfx_hud_hemi = 0.15f; // HUD Hemi Offset

int ps_ssfx_il_quality = 32; // IL Samples
Fvector4 ps_ssfx_il = {6.66f, 1.0f, 1.0f, 5.0f}; // Res, Int, Vibrance, Blur
Fvector4 ps_ssfx_il_setup1 = {150.0f, 1.0f, 0.5f, 0.0f}; // Distance, HUD, Flora, -

int ps_ssfx_ao_quality = 4; // AO Samples
Fvector4 ps_ssfx_ao = {1.0f, 5.0f, 1.0f, 2.5f}; // Res, AO int, Blur, Radius
Fvector4 ps_ssfx_ao_setup1 = {150.0, 1.0, 1.0, 0.0}; // Distance, HUD, Flora, Max OCC

Fvector4 ps_ssfx_water = {1.0f, 0.8f, 1.0f, 0.0f}; // Res, Blur, Blur Perlin, -
Fvector3 ps_ssfx_water_quality = {1.0, 2.0, 0.0}; // SSR Quality, Parallax Quality, -
Fvector4 ps_ssfx_water_setup1 = {0.6f, 3.0f, 0.3f, 0.05f}; // Distortion, Turbidity, Softborder, Parallax Height
Fvector4 ps_ssfx_water_setup2 = {0.8f, 6.0f, 0.3f, 0.5f}; // Reflection, Specular, Caustics, Ripples

int ps_ssfx_ssr_quality = 0; // Quality
Fvector4 ps_ssfx_ssr = {1.0f, 0.2f, 0.0f, 0.0f}; // Res, Blur, Temp, Noise
Fvector4 ps_ssfx_ssr_2 = {0.0f, 1.3f, 1.0f, 0.015f}; // Quality, Fade, Int, Wpn Int

Fvector4 ps_ssfx_terrain_quality = {8, 0, 0, 0};
Fvector4 ps_ssfx_terrain_offset = {0, 0, 0, 0};

Fvector3 ps_ssfx_shadows = {256, 1536, 0.0f}; // LOD min res, LOD max res, -
Fvector4 ps_ssfx_volumetric = {1.0f, 1.0f, 3.0f, 1.0f}; // Force Volumetric, Vol Intensity, Vol Quality, -

Fvector3 ps_ssfx_shadow_bias = {0.4f, 0.03f, 0.0f};

Fvector4 ps_ssfx_lut = {0.0f, 0.0f, 0.0f, 0.0f};

Fvector4 ps_ssfx_wind_grass = {9.5f, 1.4f, 1.5f, 0.4f};
Fvector4 ps_ssfx_wind_trees = {11.0f, 0.15f, 0.5f, 0.15f};

Fvector4 ps_ssfx_florafixes_1 = {0.3f, 0.21f, 0.3f, 0.21f}; // Flora fixes 1
Fvector4 ps_ssfx_florafixes_2 = {2.0f, 1.0f, 0.0f, 0.0f}; // Flora fixes 2

Fvector4 ps_ssfx_wetsurfaces_1 = {1.0f, 1.0f, 1.0f, 1.0f}; // Wet surfaces 1
Fvector4 ps_ssfx_wetsurfaces_2 = {1.0f, 1.0f, 1.0f, 1.0f}; // Wet surfaces 2

int ps_ssfx_is_underground = 0;
int ps_ssfx_gloss_method = 0;
float ps_ssfx_gloss_factor = 0.5f;
Fvector3 ps_ssfx_gloss_minmax = {0.0f, 0.92f, 0.0f}; // Gloss

Fvector4 ps_ssfx_lightsetup_1 = {0.35f, 0.5f, 0.0f, 0.0f}; // Spec intensity

Fvector4 ps_ssfx_hud_drops_1 = {1.0f, 1.0f, 1.0f, 1.0f}; // Anim Speed, Int, Reflection, Refraction
Fvector4 ps_ssfx_hud_drops_2 = {1.5f, 0.85f, 0.0f, 2.0f}; // Density, Size, Extra Gloss, Gloss

Fvector4 ps_ssfx_blood_decals = {0.6f, 0.6f, 0.f, 0.f};
Fvector4 ps_ssfx_rain_1 = {2.0f, 0.1f, 0.6f, 2.f}; // Len, Width, Speed, Quality
Fvector4 ps_ssfx_rain_2 = {0.5f, 0.1f, 1.0f, 0.5f}; // Alpha, Brigthness, Refraction, Reflection
Fvector4 ps_ssfx_rain_3 = {0.5f, 1.0f, 0.0f, 0.0f}; // Alpha, Refraction ( Splashes )
Fvector4 ps_ssfx_rain_drops_setup = {2500, 15, 0.0f, 0.0f};

Fvector3 ps_ssfx_shadow_cascades = {20, 40, 160};
Fvector4 ps_ssfx_grass_shadows = {.0f, .35f, 30.0f, .0f};

Fvector4 ps_ssfx_grass_interactive = {.0f, .0f, 2000.0f, 1.0f};
Fvector4 ps_ssfx_int_grass_params_1 = {1.0f, 1.0f, 1.0f, 25.0f};
Fvector4 ps_ssfx_int_grass_params_2 = {1.0f, 5.0f, 1.0f, 1.0f};

Fvector4 ps_ssfx_wpn_dof_1 = {.0f, .0f, .0f, .0f};
float ps_ssfx_wpn_dof_2 = 1.0f;

float ps_r3_dyn_wet_surf_near = 10.f; // 10.0f
float ps_r3_dyn_wet_surf_far = 30.f; // 30.0f
int ps_r3_dyn_wet_surf_sm_res = 256; // 256
Flags32 psDeviceFlags2 = {0};

int ps_r__detail_radius = 49;
u32 dm_size = 24;
u32 dm_cache1_line = 12; // dm_size*2/dm_cache1_count
u32 dm_cache_line = 49; // dm_size+1+dm_size
u32 dm_cache_size = 2401; // dm_cache_line*dm_cache_line
float dm_fade = 47.5; // float(2*dm_size)-.5f;
u32 dm_current_size = 24;
u32 dm_current_cache1_line = 12; // dm_current_size*2/dm_cache1_count
u32 dm_current_cache_line = 49; // dm_current_size+1+dm_current_size
u32 dm_current_cache_size = 2401; // dm_current_cache_line*dm_current_cache_line
float dm_current_fade = 47.5; // float(2*dm_current_size)-.5f;
float ps_current_detail_density = 0.6;
float ps_current_detail_scale = 1.f;

//- Mad Max
float ps_r2_gloss_factor = 4.0f;
float ps_r2_gloss_min = 0.0f;
//- Mad Max

// textures
#ifdef DEBUG
int psTextureLOD = 0;
u32 psCurrentBPP = 32;
#endif

int opt_static = 2;
int opt_dynamic = 2;

#include "../../xr_3da/xr_ioconsole.h"
#include "../../xr_3da/xr_ioc_cmd.h"

#include "../xrRenderDX10/StateManager/dx10SamplerStateCache.h"

class CCC_r__color final : public CCC_Vector4
{
public:
    CCC_r__color(LPCSTR N, Fvector4* V, const Fvector4 _min, const Fvector4 _max) : CCC_Vector4(N, V, _min, _max) {}

    void Execute(LPCSTR args) override
    {
        float x, y, z, w;

        if (sscanf(args, "%g,%g,%g,%g", &x, &y, &z, &w) != 4 && sscanf(args, "(%g,%g,%g,%g)", &x, &y, &z, &w) != 4)
        {
            InvalidSyntax();
            return;
        }

        if (w < min.w || x + w < min.x || y + w < min.y || z + w < min.z || w > max.w || x + w > max.x || y + w > max.y || z + w > max.z)
        {
            InvalidSyntax();
            return;
        }

        value->set(x, y, z, w);
    }
};

class CCC_ssfx_cascades final : public CCC_Vector3
{
    void apply() { RImplementation.init_cacades(); }

public:
    CCC_ssfx_cascades(LPCSTR N, Fvector3* V, const Fvector3 _min, const Fvector3 _max) : CCC_Vector3(N, V, _min, _max) {}
    void Execute(LPCSTR args) override
    {
        CCC_Vector3::Execute(args);
        apply();
    }
    void Status(TStatus& S) override
    {
        CCC_Vector3::Status(S);
        apply();
    }
};

//-----------------------------------------------------------------------
class CCC_detail_radius : public CCC_Integer
{
public:
    void apply()
    {
        dm_current_size = iFloor((float)ps_r__detail_radius / 4) * 2;
        dm_current_cache1_line = dm_current_size * 2 / 4; // assuming cache1_count = 4
        dm_current_cache_line = dm_current_size + 1 + dm_current_size;
        dm_current_cache_size = dm_current_cache_line * dm_current_cache_line;
        dm_current_fade = float(2 * dm_current_size) - .5f;
    }
    CCC_detail_radius(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer(N, V, _min, _max) {};
    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S) { CCC_Integer::Status(S); }
};

class CCC_detail_reset : public CCC_Float
{
public:
    void apply()
    {
        if (RImplementation.Details)
            RImplementation.Details->need_init = true;
    }

    CCC_detail_reset(LPCSTR N, float* V, float _min = 0, float _max = 1) : CCC_Float(N, V, _min, _max) {}
    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S) { CCC_Float::Status(S); }
};

class CCC_tf_Aniso : public CCC_Integer
{
public:
    void apply()
    {
        if (0 == HW.pDevice)
            return;
        int val = *value;
        clamp(val, 1, 16);
        SSManager.SetMaxAnisotropy(val);
    }
    CCC_tf_Aniso(LPCSTR N, int* v) : CCC_Integer(N, v, 1, 16) {};
    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Integer::Status(S);
        apply();
    }
};
class CCC_tf_MipBias : public CCC_Float
{
public:
    void apply()
    {
        if (0 == HW.pDevice)
            return;

        //	TODO: DX10: Implement mip bias control
        // VERIFY(!"apply not implmemented.");
        SSManager.SetMipLODBias(*value);
    }

    CCC_tf_MipBias(LPCSTR N, float* v) : CCC_Float(N, v, -3.f, +3.f) {};
    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Float::Status(S);
        apply();
    }
};

#ifdef DEBUG
class CCC_R2GM : public CCC_Float
{
public:
    CCC_R2GM(LPCSTR N, float* v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
    virtual void Execute(LPCSTR args)
    {
        if (0 == xr_strcmp(args, "on"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, TRUE);
        }
        else if (0 == xr_strcmp(args, "off"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, FALSE);
        }
        else
        {
            CCC_Float::Execute(args);
            if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))
            {
                static LPCSTR name[4] = {"oren", "blin", "phong", "metal"};
                float mid = *value;
                int m0 = iFloor(mid) % 4;
                int m1 = (m0 + 1) % 4;
                float frc = mid - float(iFloor(mid));
                Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
            }
        }
    }
};
#endif

class CCC_Screenshot : public IConsole_Command
{
public:
    CCC_Screenshot(LPCSTR N) : IConsole_Command(N) {};
    virtual void Execute(LPCSTR args)
    {
        string_path name;
        name[0] = 0;
        sscanf(args, "%s", name);
        LPCSTR image = xr_strlen(name) ? name : 0;
        ::Render->Screenshot(IRender_interface::SM_NORMAL, image);
    }
};

class CCC_RestoreQuadIBData : public IConsole_Command
{
public:
    CCC_RestoreQuadIBData(LPCSTR N) : IConsole_Command(N) {};
    virtual void Execute(LPCSTR args) { RCache.RestoreQuadIBData(); }
};

class CCC_ModelPoolStat : public IConsole_Command
{
public:
    CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { RImplementation.Models->dump(); }
};

//-----------------------------------------------------------------------
class CCC_Preset : public CCC_Token
{
public:
    CCC_Preset(LPCSTR N, u32* V, const xr_token* T) : CCC_Token(N, V, T) {};

    virtual void Execute(LPCSTR args)
    {
        CCC_Token::Execute(args);
        string_path _cfg;
        string_path cmd;

        switch (*value)
        {
        case 0: xr_strcpy(_cfg, "rspec_minimum.ltx"); break;
        case 1: xr_strcpy(_cfg, "rspec_low.ltx"); break;
        case 2: xr_strcpy(_cfg, "rspec_default.ltx"); break;
        case 3: xr_strcpy(_cfg, "rspec_high.ltx"); break;
        case 4: xr_strcpy(_cfg, "rspec_extreme.ltx"); break;
        }
        FS.update_path(_cfg, "$game_config$", _cfg);
        strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
        Console->Execute(cmd);
    }
};

class CCC_VideoMemoryStats : public IConsole_Command
{
protected:
public:
    CCC_VideoMemoryStats(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };

    virtual void Execute(LPCSTR args)
    {
        Msg("memory usage  mb \t \t video    \t managed      \t system");

        float vb_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_DEFAULT] / 1024 / 1024;
        float vb_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_MANAGED] / 1024 / 1024;
        float vb_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("vertex buffer\t \t %f \t %f \t %f ", vb_video, vb_managed, vb_system);

        float ib_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_DEFAULT] / 1024 / 1024;
        float ib_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_MANAGED] / 1024 / 1024;
        float ib_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("index buffer\t \t %f \t %f \t %f ", ib_video, ib_managed, ib_system);

        float rt_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_DEFAULT] / 1024 / 1024;
        float rt_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_MANAGED] / 1024 / 1024;
        float rt_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("rtarget\t \t %f \t %f \t %f ", rt_video, rt_managed, rt_system);

        Msg("total\t \t %f \t %f \t %f \n", vb_video + ib_video + rt_video, vb_managed + ib_managed + rt_managed, vb_system + ib_system + rt_system);

        u32 m_base = 0;
        u32 c_base = 0;
        u32 m_lmaps = 0;
        u32 c_lmaps = 0;

        dxRenderDeviceRender::Instance().ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
        Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

        HW.DumpVideoMemoryUsage();
    }
};

class CCC_DumpResources : public IConsole_Command
{
public:
    CCC_DumpResources(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        RImplementation.Models->dump();
        dxRenderDeviceRender::Instance().Resources->Dump(false);
    }
};

//	Allow real-time fog config reload
#ifdef DEBUG

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"

class CCC_Fog_Reload : public IConsole_Command
{
public:
    CCC_Fog_Reload(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { FluidManager.UpdateProfiles(); }
};
#endif //	DEBUG

class CCC_PART_Export : public IConsole_Command
{
public:
    CCC_PART_Export(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        if (g_pGameLevel)
        {
            Log("Error: Unload level first!");
            return;
        }

        Msg("Exporting particles...");
        RImplementation.PSLibrary.Reload();
        RImplementation.PSLibrary.Save2(0 == xr_strcmp(args, "1"));
        Msg("Exporting particles Done!");
    }
};

class CCC_PART_Import : public IConsole_Command
{
public:
    CCC_PART_Import(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        if (g_pGameLevel)
        {
            Log("Error: Unload level first!");
            return;
        }

        Msg("Importing particles...");
        RImplementation.PSLibrary.OnDestroy();
        RImplementation.PSLibrary.Load2();
        RImplementation.PSLibrary.ExportAllAsNew();
        RImplementation.PSLibrary.OnCreate();
        Msg("Importing particles Done!");
    }
};

void xrRender_initconsole()
{
    if (!FS.path_exist("$game_weathers$"))
        ps_r2_gloss_factor = 1.0f;

    CMD3(CCC_Preset, "_preset", &ps_Preset, qpreset_token);

    CMD4(CCC_Integer, "rs_skeleton_update", &psSkeletonUpdate, 2, 128);
#ifdef DEBUG
    CMD1(CCC_DumpResources, "dump_resources");
#endif //	 DEBUG

    CMD4(CCC_Float, "r__dtex_range", &r__dtex_range, 5, 175);

    // Common
    CMD1(CCC_Screenshot, "screenshot");

    //	Igor: just to test bug with rain/particles corruption
    CMD1(CCC_RestoreQuadIBData, "r_restore_quad_ib_data");

#ifdef DEBUG
    CMD1(CCC_BuildSSA, "build_ssa");

    CMD4(CCC_Integer, "r__lsleep_frames", &ps_r__LightSleepFrames, 4, 30);
    CMD4(CCC_Float, "r__ssa_glod_start", &ps_r__GLOD_ssa_start, 128, 512);
    CMD4(CCC_Float, "r__ssa_glod_end", &ps_r__GLOD_ssa_end, 16, 96);
    CMD4(CCC_Float, "r__wallmark_shift_pp", &ps_r__WallmarkSHIFT, 0.0f, 1.f);
    CMD4(CCC_Float, "r__wallmark_shift_v", &ps_r__WallmarkSHIFT_V, 0.0f, 1.f);
    CMD1(CCC_ModelPoolStat, "stat_models");
    CMD3(CCC_Token, "vid_bpp", &psCurrentBPP, vid_bpp_token);
#endif // DEBUG
    CMD4(CCC_Float, "r__wallmark_ttl", &ps_r__WallmarkTTL, 1.0f, 10.f * 60.f);

    CMD4(CCC_Float, "r__geometry_lod", &ps_r__LOD, 1.f, 3.f); // AVO: extended from 1.2f to 3.f
    //.	CMD4(CCC_Float,		"r__geometry_lod_pow",	&ps_r__LOD_Power,			0,		2		);

    CMD4(CCC_detail_reset, "r__detail_density", &ps_current_detail_density, 0.04f, 1.f);
    CMD4(CCC_detail_reset, "r__detail_scale", &ps_current_detail_scale, 0.2f, 3.0f);

#ifdef DEBUG
    CMD4(CCC_Float, "r__detail_l_ambient", &ps_r__Detail_l_ambient, .5f, .95f);
    CMD4(CCC_Float, "r__detail_l_aniso", &ps_r__Detail_l_aniso, .1f, .5f);

    CMD4(CCC_Float, "r__d_tree_w_amp", &ps_r__Tree_w_amp, .001f, 1.f);
    CMD4(CCC_Float, "r__d_tree_w_rot", &ps_r__Tree_w_rot, .01f, 100.f);
    CMD4(CCC_Float, "r__d_tree_w_speed", &ps_r__Tree_w_speed, 1.0f, 10.f);

    tw_min.set(EPS, EPS, EPS);
    tw_max.set(2, 2, 2);
    CMD4(CCC_Vector3, "r__d_tree_wave", &ps_r__Tree_Wave, tw_min, tw_max);
#endif // DEBUG

    CMD2(CCC_tf_Aniso, "r__tf_aniso", &ps_r__tf_Anisotropic); //	{1..16}
    CMD2(CCC_tf_MipBias, "r__tf_mipbias", &ps_r__tf_Mipbias); // {-3 +3}

    CMD3(CCC_Mask, "r__actor_shadow", &ps_r2_ls_flags_ext, R2FLAGEXT_ACTOR_SHADOW);

    CMD4(CCC_Float, "r1_pps_u", &ps_r1_pps_u, -1.f, +1.f);
    CMD4(CCC_Float, "r1_pps_v", &ps_r1_pps_v, -1.f, +1.f);

    // R2
    CMD4(CCC_Float, "r2_ssa_lod_a", &ps_r2_ssaLOD_A, 16, 96);
    CMD4(CCC_Float, "r2_ssa_lod_b", &ps_r2_ssaLOD_B, 32, 64);

    // R2-specific
#ifdef DEBUG
    CMD2(CCC_R2GM, "r2em", &ps_r2_gmaterial);
#endif

    CMD3(CCC_Mask, "r2_tonemap", &ps_r2_ls_flags, R2FLAG_TONEMAP);
    CMD4(CCC_Float, "r2_tonemap_middlegray", &ps_r2_tonemap_middlegray, 0.0f, 2.0f);
    CMD4(CCC_Float, "r2_tonemap_adaptation", &ps_r2_tonemap_adaptation, 0.01f, 10.0f);
    CMD4(CCC_Float, "r2_tonemap_lowlum", &ps_r2_tonemap_low_lum, 0.0001f, 1.0f);
    CMD4(CCC_Float, "r2_tonemap_amount", &ps_r2_tonemap_amount, 0.0000f, 1.0f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_scale", &ps_r2_ls_bloom_kernel_scale, 0.05f, 2.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_g", &ps_r2_ls_bloom_kernel_g, 1.f, 7.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_b", &ps_r2_ls_bloom_kernel_b, 0.01f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_threshold", &ps_r2_ls_bloom_threshold, 0.f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_speed", &ps_r2_ls_bloom_speed, 0.f, 100.f);
    CMD4(CCC_Float, "r2_ls_dsm_kernel", &ps_r2_ls_dsm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_psm_kernel", &ps_r2_ls_psm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_ssm_kernel", &ps_r2_ls_ssm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_squality", &ps_r2_ls_squality, .5f, 3.f);

    CMD3(CCC_Mask, "r2_zfill", &ps_r2_ls_flags, R2FLAG_ZFILL);
    CMD4(CCC_Float, "r2_zfill_depth", &ps_r2_zfill, .001f, .5f);

    //- Mad Max
    CMD4(CCC_Float, "r2_gloss_factor", &ps_r2_gloss_factor, .001f, 10.f);
    CMD4(CCC_Float, "r2_gloss_min", &ps_r2_gloss_min, .001f, 1.0f);
    //- Mad Max

    CMD3(CCC_Mask, "r2_disable_hom", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_HOM);

    CMD3(CCC_Mask, "r_font_shadows", &ps_r2_ls_flags_ext, R2FLAGEXT_FONT_SHADOWS);

    CMD3(CCC_Mask, "r2_sun", &ps_r2_ls_flags, R2FLAG_SUN);
    CMD3(CCC_Mask, "r2_sun_details", &ps_r2_ls_flags, R2FLAG_SUN_DETAILS);
    CMD3(CCC_Mask, "r2_sun_focus", &ps_r2_ls_flags, R2FLAG_SUN_FOCUS);
    CMD3(CCC_Mask, "r2_exp_donttest_shad", &ps_r2_ls_flags, R2FLAG_EXP_DONT_TEST_SHADOWED);

    CMD3(CCC_Mask, "r2_sun_tsm", &ps_r2_ls_flags, R2FLAG_SUN_TSM);
    CMD4(CCC_Float, "r2_sun_tsm_proj", &ps_r2_sun_tsm_projection, .001f, 0.8f);
    CMD4(CCC_Float, "r2_sun_tsm_bias", &ps_r2_sun_tsm_bias, -0.5, +0.5);

    CMD3(CCC_Token, "r__smap_size", &r2_SmapSize, SmapSizeToken);
    CMD4(CCC_Float, "r2_sun_near", &ps_r2_sun_near, 1.f, 150.f);

    CMD4(CCC_Float, "r2_sun_far", &OLES_SUN_LIMIT_27_01_07, 51.f, 180.f);

    CMD4(CCC_Float, "r2_sun_near_border", &ps_r2_sun_near_border, .5f, 3.0f);
    CMD4(CCC_Float, "r2_sun_depth_far_scale", &ps_r2_sun_depth_far_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_sun_depth_far_bias", &ps_r2_sun_depth_far_bias, -0.5, +0.5);
    CMD4(CCC_Float, "r2_sun_depth_near_scale", &ps_r2_sun_depth_near_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_sun_depth_near_bias", &ps_r2_sun_depth_near_bias, -0.5, +0.5);
    CMD4(CCC_Float, "r2_sun_lumscale", &ps_r2_sun_lumscale, -1.0, +3.0);
    CMD4(CCC_Float, "r2_sun_lumscale_hemi", &ps_r2_sun_lumscale_hemi, 0.0, +3.0);
    CMD4(CCC_Float, "r2_sun_lumscale_amb", &ps_r2_sun_lumscale_amb, 0.0, +3.0);

    CMD4(CCC_Float, "r2_mblur", &ps_r2_mblur, 0.0f, 1.0f);

    ///////lvutner
    CMD4(CCC_Vector4, "r2_mask_control", &ps_r2_mask_control, Fvector4().set(0, 0, 0, 0), Fvector4().set(10, 3, 1, 1));

    Fvector3 tw_min{0, 0, 0};
    Fvector3 tw_max{1, 2, 1};
    CMD4(CCC_Vector3, "r2_drops_control", &ps_r2_drops_control, tw_min, tw_max);

    Fvector4 tw2_min = {0.5f, 0.5f, 0.5f, -2.f};
    Fvector4 tw2_max = {2.f, 2.f, 2.f, 2.f};

    CMD4(CCC_r__color, "r__color_gamma", &ps_r__color_gamma, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_slope", &ps_r__color_slope, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_saturation", &ps_r__color_saturation, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_contrast", &ps_r__color_contrast, tw2_min, tw2_max);

    tw2_min.set(0, 0, 0, -2.f);

    CMD4(CCC_r__color, "r__color_offset", &ps_r__color_offset, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_power", &ps_r__color_power, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_red", &ps_r__color_red, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_green", &ps_r__color_green, tw2_min, tw2_max);
    CMD4(CCC_r__color, "r__color_blue", &ps_r__color_blue, tw2_min, tw2_max);

    CMD3(CCC_Mask, "r2_terrain_z_prepass", &ps_r2_ls_flags, R2FLAG_TERRAIN_PREPASS); // Terrain Z Prepass @Zagolski

    //////////other
    CMD3(CCC_Mask, "r2_mblur_enabled", &ps_r2_ls_flags, R2FLAG_MBLUR);
    CMD3(CCC_Token, "r2_smaa", &ps_smaa_quality, smaa_quality_token);

#ifdef DEBUG
    CMD3(CCC_Mask, "r2_gi", &ps_r2_ls_flags, R2FLAG_GI);
    CMD4(CCC_Float, "r2_gi_clip", &ps_r2_GI_clip, EPS, 0.1f);
    CMD4(CCC_Integer, "r2_gi_depth", &ps_r2_GI_depth, 1, 5);
    CMD4(CCC_Integer, "r2_gi_photons", &ps_r2_GI_photons, 8, 256);
    CMD4(CCC_Float, "r2_gi_refl", &ps_r2_GI_refl, EPS_L, 0.99f);
#endif

    // Shader param stuff
    tw2_max.set(100.f, 100.f, 100.f, 100.f);

    // Screen Space Shaders
    CMD4(CCC_Integer, "ssfx_pom_refine", &ps_ssfx_pom_refine, 0, 1);
    CMD4(CCC_Vector4, "ssfx_pom", &ps_ssfx_pom, Fvector4().set(0, 0, 0, 0), Fvector4().set(36, 60, 1, 1));

    CMD4(CCC_Integer, "ssfx_terrain_pom_refine", &ps_ssfx_terrain_pom_refine, 0, 1);
    CMD4(CCC_Integer, "ssfx_terrain_grass_align", &ps_ssfx_terrain_grass_align, 0, 1);
    CMD4(CCC_Float, "ssfx_terrain_grass_slope", &ps_ssfx_terrain_grass_slope, 0, 1);
    CMD4(CCC_Vector4, "ssfx_terrain_pom", &ps_ssfx_terrain_pom, Fvector4().set(0, 0, 0, 0), Fvector4().set(36, 60, 1, 2));

    CMD4(CCC_Integer, "ssfx_bloom_use_presets", &ps_ssfx_bloom_use_presets, 0, 1);
    CMD4(CCC_Vector4, "ssfx_bloom_1", &ps_ssfx_bloom_1, Fvector4().set(1, 1, 0, 0), Fvector4().set(10, 100, 100, 10));
    CMD4(CCC_Vector4, "ssfx_bloom_2", &ps_ssfx_bloom_2, Fvector4().set(1, 0, 0, 0), Fvector4().set(5, 10, 10, 10));
    CMD4(CCC_Vector4, "ssfx_sss_quality", &ps_ssfx_sss_quality, Fvector4().set(1, 1, 0, 0), Fvector4().set(24, 12, 1, 1));
    CMD4(CCC_Vector4, "ssfx_sss", &ps_ssfx_sss, Fvector4().set(0, 0, 0, 0), Fvector4().set(3, 3, 1, 1));

    CMD4(CCC_Float, "ssfx_hud_hemi", &ps_ssfx_hud_hemi, 0.0f, 1.0f);

    CMD4(CCC_Integer, "ssfx_il_quality", &ps_ssfx_il_quality, 16, 64);
    CMD4(CCC_Vector4, "ssfx_il", &ps_ssfx_il, Fvector4().set(0, 0, 0, 0), Fvector4().set(8, 10, 3, 6));
    CMD4(CCC_Vector4, "ssfx_il_setup1", &ps_ssfx_il_setup1, Fvector4().set(0, 0, 0, 0), Fvector4().set(300, 1, 1, 1));

    CMD4(CCC_Integer, "ssfx_ao_quality", &ps_ssfx_ao_quality, 2, 8);
    CMD4(CCC_Vector4, "ssfx_ao", &ps_ssfx_ao, Fvector4().set(0, 0, 0, 0), Fvector4().set(8, 10, 1, 10));
    CMD4(CCC_Vector4, "ssfx_ao_setup1", &ps_ssfx_ao_setup1, Fvector4().set(0, 0, 0, 0), Fvector4().set(300, 1, 1, 1));

    CMD4(CCC_Vector4, "ssfx_water", &ps_ssfx_water, Fvector4().set(1, 0, 0, 0), Fvector4().set(8, 1, 1, 0));
    CMD4(CCC_Vector3, "ssfx_water_quality", &ps_ssfx_water_quality, Fvector3().set(0, 0, 0), Fvector3().set(4, 3, 0));
    CMD4(CCC_Vector4, "ssfx_water_setup1", &ps_ssfx_water_setup1, Fvector4().set(0, 0, 0, 0), Fvector4().set(2, 10, 1, 0.1));
    CMD4(CCC_Vector4, "ssfx_water_setup2", &ps_ssfx_water_setup2, Fvector4().set(0, 0, 0, 0), Fvector4().set(1, 10, 1, 1));

    CMD4(CCC_Integer, "ssfx_ssr_quality", &ps_ssfx_ssr_quality, 0, 5);
    CMD4(CCC_Vector4, "ssfx_ssr", &ps_ssfx_ssr, Fvector4().set(1, 0, 0, 0), Fvector4().set(2, 1, 1, 1));
    CMD4(CCC_Vector4, "ssfx_ssr_2", &ps_ssfx_ssr_2, Fvector4().set(0, 0, 0, 0), Fvector4().set(2, 2, 2, 2));

    CMD4(CCC_Vector4, "ssfx_terrain_quality", &ps_ssfx_terrain_quality, Fvector4().set(0, 0, 0, 0), Fvector4().set(40, 0, 0, 0));
    CMD4(CCC_Vector4, "ssfx_terrain_offset", &ps_ssfx_terrain_offset, Fvector4().set(-1, -1, -1, -1), Fvector4().set(1, 1, 1, 1));

    CMD4(CCC_Vector3, "ssfx_shadows", &ps_ssfx_shadows, Fvector3().set(128, 1536, 0), Fvector3().set(1536, 4096, 0));
    CMD4(CCC_Vector4, "ssfx_volumetric", &ps_ssfx_volumetric, Fvector4().set(0, 0, 1.0, 1.0), Fvector4().set(1.0, 5.0, 5.0, 16.0));

    CMD4(CCC_Vector3, "ssfx_shadow_bias", &ps_ssfx_shadow_bias, Fvector3().set(0, 0, 0), Fvector3().set(1.0, 1.0, 1.0));

    CMD4(CCC_Vector4, "ssfx_lut", &ps_ssfx_lut, Fvector4().set(0.0, 0.0, 0.0, 0.0), tw2_max);

    CMD4(CCC_Vector4, "ssfx_wind_grass", &ps_ssfx_wind_grass, Fvector4().set(0.0, 0.0, 0.0, 0.0), Fvector4().set(20.0, 5.0, 5.0, 5.0));
    CMD4(CCC_Vector4, "ssfx_wind_trees", &ps_ssfx_wind_trees, Fvector4().set(0.0, 0.0, 0.0, 0.0), Fvector4().set(20.0, 5.0, 5.0, 1.0));

    CMD4(CCC_Vector4, "ssfx_florafixes_1", &ps_ssfx_florafixes_1, Fvector4().set(0.0, 0.0, 0.0, 0.0), Fvector4().set(1.0, 1.0, 1.0, 1.0));
    CMD4(CCC_Vector4, "ssfx_florafixes_2", &ps_ssfx_florafixes_2, Fvector4().set(0.0, 0.0, 0.0, 0.0), Fvector4().set(10.0, 1.0, 1.0, 1.0));

    CMD4(CCC_Vector4, "ssfx_wetsurfaces_1", &ps_ssfx_wetsurfaces_1, Fvector4().set(0.01, 0.01, 0.01, 0.01), Fvector4().set(2.0, 2.0, 2.0, 2.0));
    CMD4(CCC_Vector4, "ssfx_wetsurfaces_2", &ps_ssfx_wetsurfaces_2, Fvector4().set(0.01, 0.01, 0.01, 0.01), Fvector4().set(2.0, 2.0, 2.0, 2.0));

    CMD4(CCC_Integer, "ssfx_is_underground", &ps_ssfx_is_underground, 0, 1);
    CMD4(CCC_Integer, "ssfx_gloss_method", &ps_ssfx_gloss_method, 0, 1);
    CMD4(CCC_Vector3, "ssfx_gloss_minmax", &ps_ssfx_gloss_minmax, Fvector3().set(0, 0, 0), Fvector3().set(1.0, 1.0, 1.0));
    CMD4(CCC_Float, "ssfx_gloss_factor", &ps_ssfx_gloss_factor, 0.0f, 1.0f);

    CMD4(CCC_Vector4, "ssfx_lightsetup_1", &ps_ssfx_lightsetup_1, Fvector4().set(0, 0, 0, 0), Fvector4().set(1.0, 1.0, 1.0, 1.0));

    CMD4(CCC_Vector4, "ssfx_hud_drops_1", &ps_ssfx_hud_drops_1, Fvector4().set(0, 0, 0, 0), Fvector4().set(100000, 100, 100, 100));
    CMD4(CCC_Vector4, "ssfx_hud_drops_2", &ps_ssfx_hud_drops_2, Fvector4().set(0, 0, 0, 0), tw2_max);

    CMD4(CCC_Vector4, "ssfx_blood_decals", &ps_ssfx_blood_decals, Fvector4().set(0, 0, 0, 0), Fvector4().set(5, 5, 0, 0));

    CMD4(CCC_Vector4, "ssfx_rain_drops_setup", &ps_ssfx_rain_drops_setup, Fvector4().set(1000, 10, 0, 0), Fvector4().set(5000, 30, 0, 0));
    CMD4(CCC_Vector4, "ssfx_rain_1", &ps_ssfx_rain_1, Fvector4().set(0, 0, 0, 0), Fvector4().set(10, 5, 5, 2));
    CMD4(CCC_Vector4, "ssfx_rain_2", &ps_ssfx_rain_2, Fvector4().set(0, 0, 0, 0), Fvector4().set(1, 10, 10, 10));
    CMD4(CCC_Vector4, "ssfx_rain_3", &ps_ssfx_rain_3, Fvector4().set(0, 0, 0, 0), Fvector4().set(1, 10, 10, 10));

    CMD4(CCC_Vector4, "ssfx_grass_shadows", &ps_ssfx_grass_shadows, Fvector4().set(0, 0, 0, 0), Fvector4().set(3, 1, 100, 100));
    CMD4(CCC_ssfx_cascades, "ssfx_shadow_cascades", &ps_ssfx_shadow_cascades, Fvector3().set(1.0f, 1.0f, 1.0f), Fvector3().set(300, 300, 300));

    CMD4(CCC_Vector4, "ssfx_grass_interactive", &ps_ssfx_grass_interactive, Fvector4().set(0, 0, 0, 0), Fvector4().set(1, 15, 5000, 1));
    CMD4(CCC_Vector4, "ssfx_int_grass_params_1", &ps_ssfx_int_grass_params_1, Fvector4().set(0, 0, 0, 0), Fvector4().set(5, 5, 5, 60));
    CMD4(CCC_Vector4, "ssfx_int_grass_params_2", &ps_ssfx_int_grass_params_2, Fvector4().set(0, 0, 0, 0), Fvector4().set(5, 20, 1, 5));

    CMD4(CCC_Vector4, "ssfx_wpn_dof_1", &ps_ssfx_wpn_dof_1, tw2_min, tw2_max);
    CMD4(CCC_Float, "ssfx_wpn_dof_2", &ps_ssfx_wpn_dof_2, 0, 1);

    // Geometry optimization
    CMD4(CCC_Integer, "r__optimize_static_geom", &opt_static, 0, 4);
    CMD4(CCC_Integer, "r__optimize_dynamic_geom", &opt_dynamic, 0, 4);
    psDeviceFlags2.set(rsOptShadowGeom, TRUE);
    CMD3(CCC_Mask, "r__optimize_shadow_geom", &psDeviceFlags2, rsOptShadowGeom);

    CMD4(CCC_Integer, "r2_wait_sleep", &ps_r2_wait_sleep, 0, 1);

#ifndef MASTER_GOLD
    CMD4(CCC_Integer, "r2_dhemi_count", &ps_r2_dhemi_count, 4, 25);
    CMD4(CCC_Float, "r2_dhemi_sky_scale", &ps_r2_dhemi_sky_scale, 0.0f, 100.f);
    CMD4(CCC_Float, "r2_dhemi_light_scale", &ps_r2_dhemi_light_scale, 0, 100.f);
    CMD4(CCC_Float, "r2_dhemi_light_flow", &ps_r2_dhemi_light_flow, 0, 1.f);
    CMD4(CCC_Float, "r2_dhemi_smooth", &ps_r2_lt_smooth, 0.f, 10.f);
    CMD3(CCC_Mask, "rs_hom_depth_draw", &ps_r2_ls_flags_ext, R_FLAGEXT_HOM_DEPTH_DRAW);
#endif // DEBUG

    CMD3(CCC_Mask, "r2_shadow_cascede_zcul", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_ZCULLING);

    CMD4(CCC_Float, "r2_ls_depth_scale", &ps_r2_ls_depth_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_ls_depth_bias", &ps_r2_ls_depth_bias, -0.5, +0.5);

    CMD4(CCC_Float, "r2_parallax_h", &ps_r2_df_parallax_h, .0f, .5f);
    CMD4(CCC_Float, "r2_slight_fade", &ps_r2_slight_fade, .2f, 2.f);

    CMD3(CCC_Mask, "r2_dof_enable", &ps_r2_ls_flags, R2FLAG_DOF);
    CMD3(CCC_Mask, "r2_volumetric_lights", &ps_r2_ls_flags, R2FLAG_VOLUMETRIC_LIGHTS);

    // Sunshafts
    CMD3(CCC_Token, "r_sunshafts_mode", &ps_r_sunshafts_mode, sunshafts_mode_token);
    CMD4(CCC_Float, "r_ss_sunshafts_length", &ps_r_ss_sunshafts_length, 0.2f, 1.5f);
    CMD4(CCC_Float, "r_ss_sunshafts_radius", &ps_r_ss_sunshafts_radius, 0.5f, 2.f);

    CMD4(CCC_Float, "r_SunShafts_Radius", &ps_r_prop_ss_radius, 0.5f, 2.0f);
    CMD4(CCC_Float, "r_SunShafts_Blend", &ps_r_prop_ss_blend, 0.01f, 1.0f);

    CMD3(CCC_Token, "r_ao_mode", &ps_r_ao_mode, ao_mode_token);
    CMD3(CCC_Token, "r2_ssao", &ps_r_ssao, qssao_token);

    CMD3(CCC_Mask, "r4_enable_tessellation", &ps_r2_ls_flags_ext, R2FLAGEXT_ENABLE_TESSELLATION); // Need restart
    CMD3(CCC_Mask, "r4_wireframe", &ps_r2_ls_flags_ext, R2FLAGEXT_WIREFRAME); // Need restart
    CMD3(CCC_Mask, "r2_steep_parallax", &ps_r2_ls_flags, R2FLAG_STEEP_PARALLAX);

    CMD3(CCC_Token, "r2_sun_quality", &ps_r_sun_quality, qsun_quality_token);

    CMD3(CCC_Token, "r3_msaa", &ps_r3_msaa, qmsaa_token);

    CMD4(CCC_detail_radius, "r__detail_radius", &ps_r__detail_radius, 49, 300);
    CMD4(CCC_Integer, "r__no_scale_on_fade", &ps_no_scale_on_fade, 0, 1); // Alundaio

    //	Allow real-time fog config reload
#ifdef DEBUG
    CMD1(CCC_Fog_Reload, "r3_fog_reload");
#endif //	DEBUG

    CMD3(CCC_Mask, "r3_dynamic_wet_surfaces", &ps_r2_ls_flags, R3FLAG_DYN_WET_SURF);
    CMD4(CCC_Float, "r3_dynamic_wet_surfaces_near", &ps_r3_dyn_wet_surf_near, 5, 70);
    CMD4(CCC_Float, "r3_dynamic_wet_surfaces_far", &ps_r3_dyn_wet_surf_far, 30, 300);
    CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_sm_res", &ps_r3_dyn_wet_surf_sm_res, 64, 2048);

    CMD3(CCC_Mask, "r3_volumetric_smoke", &ps_r2_ls_flags, R3FLAG_VOLUMETRIC_SMOKE);
    CMD1(CCC_VideoMemoryStats, "video_memory_stats");

    CMD1(CCC_PART_Export, "particles_export");
    CMD1(CCC_PART_Import, "particles_import");

    extern BOOL bShadersXrExport;
    CMD4(CCC_Integer, "shaders_xr_export", &bShadersXrExport, FALSE, TRUE);

    extern BOOL bSenvironmentXrExport;
    CMD4(CCC_Integer, "senvironment_xr_export", &bSenvironmentXrExport, FALSE, TRUE);

    CMD4(CCC_Integer, "r_lens_flare", &ps_lens_flare, FALSE, TRUE);

    CMD4(CCC_Float, "particle_update_mod", &ps_particle_update_coeff, 0.04f, 10.f);
}
