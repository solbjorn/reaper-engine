#pragma once

// r3xx code-path (MRT)
#define r2_RT_MSAAdepth "$user$msaadepth" // MRT
#define r2_RT_P "$user$position" // MRT
#define r2_RT_albedo "$user$albedo" // MRT

// other
#define r2_RT_accum "$user$accum" // ---	16 bit fp or 16 bit fx
#define r2_RT_accum_temp "$user$accum_temp" // ---	16 bit fp - only for HW which doesn't feature fp16 blend

#define r2_T_envs0 "$user$env_s0" // ---
#define r2_T_envs1 "$user$env_s1" // ---

#define r2_T_sky0 "$user$sky0"
#define r2_T_sky1 "$user$sky1"

#define r2_RT_sunshafts0 "$user$sun_shafts0" // first rt
#define r2_RT_sunshafts1 "$user$sun_shafts1" // second rt

#define r2_RT_SunShaftsMask "$user$SunShaftsMask"
#define r2_RT_SunShaftsMaskSmoothed "$user$SunShaftsMaskSmoothed"
#define r2_RT_SunShaftsPass0 "$user$SunShaftsPass0"

#define r2_RT_generic0 "$user$generic0" // ---
#define r2_RT_generic0_r "$user$generic0_r" // ---
#define r2_RT_generic1 "$user$generic1" // ---
#define r2_RT_generic1_r "$user$generic1_r" // ---
#define r2_RT_generic2 "$user$generic2" // ---	//	Igor: for volumetric lights
#define r2_RT_generic "$user$generic" // ---

#define r2_RT_bloom1 "$user$bloom1" // ---
#define r2_RT_bloom2 "$user$bloom2" // ---

#define r2_RT_luminance_t64 "$user$lum_t64" // --- temp
#define r2_RT_luminance_t8 "$user$lum_t8" // --- temp

#define r2_RT_luminance_src "$user$tonemap_src" // --- prev-frame-result
#define r2_RT_luminance_cur "$user$tonemap" // --- result
#define r2_RT_luminance_pool "$user$luminance" // --- pool

#define r2_RT_smap_depth "$user$smap_depth" // ---directional

#define r2_jitter "$user$jitter_" // --- dither
#define r2_sunmask "sunmask"

#define r2_RT_secondVP "$user$viewport2" // --#SM+#-- +SecondVP+ Хранит картинку со второго вьюпорта

#define r2_RT_blur_h_2 "$user$blur_h_2"
#define r2_RT_blur_2 "$user$blur_2"
#define r2_RT_blur_2_zb "$user$blur_2_zb"

#define r2_RT_blur_h_4 "$user$blur_h_4"
#define r2_RT_blur_4 "$user$blur_4"
#define r2_RT_blur_4_zb "$user$blur_4_zb"

#define r2_RT_blur_h_8 "$user$blur_h_8"
#define r2_RT_blur_8 "$user$blur_8"
#define r2_RT_blur_8_zb "$user$blur_8_zb"

#define r2_RT_dof "$user$dof"

// SMAA
#define r2_RT_smaa_edgetex "$user$smaa_edgetex"
#define r2_RT_smaa_blendtex "$user$smaa_blendtex"

#define r2_RT_ssfx "$user$ssfx" // SSS Temp1
#define r2_RT_ssfx_temp "$user$ssfx_temp" // SSS Temp2
#define r2_RT_ssfx_temp2 "$user$ssfx_temp2" // SSS Temp3
#define r2_RT_ssfx_temp3 "$user$ssfx_temp3"
#define r2_RT_ssfx_accum "$user$ssfx_accum" // SSS Volumetric
#define r2_RT_ssfx_hud "$user$ssfx_hud" // HUD & Velocity Buffer

#define r2_RT_ssfx_ssr "$user$ssfx_ssr" // SSR Acc
#define r2_RT_ssfx_water "$user$ssfx_water" // Water Acc
#define r2_RT_ssfx_water_waves "$user$ssfx_water_waves"
#define r2_RT_ssfx_ao "$user$ssfx_ao" // AO Acc
#define r2_RT_ssfx_il "$user$ssfx_il" // IL Acc

#define r2_RT_ssfx_sss "$user$ssfx_sss" // SSS Acc
#define r2_RT_ssfx_sss_ext "$user$ssfx_sss_ext" // SSS Acc
#define r2_RT_ssfx_sss_ext2 "$user$ssfx_sss_ext2" // SSS Acc
#define r2_RT_ssfx_sss_tmp "$user$ssfx_sss_tmp" // SSS Acc
#define r2_RT_ssfx_bloom1 "$user$ssfx_bloom1" // Bloom
#define r2_RT_ssfx_bloom_emissive "$user$ssfx_bloom_emissive" // Bloom
#define r2_RT_ssfx_bloom_lens "$user$ssfx_bloom_lens" // Bloom
#define r2_RT_ssfx_rain "$user$ssfx_rain" // Rain refraction buffer
#define r2_RT_ssfx_volumetric "$user$ssfx_volumetric" // Volumetric
#define r2_RT_ssfx_volumetric_tmp "$user$ssfx_volumetric_tmp" // Volumetric

#define r2_RT_ssfx_bloom_tmp2 "$user$ssfx_bloom_tmp2" // Bloom
#define r2_RT_ssfx_bloom_tmp4 "$user$ssfx_bloom_tmp4" // Bloom
#define r2_RT_ssfx_bloom_tmp8 "$user$ssfx_bloom_tmp8" // Bloom
#define r2_RT_ssfx_bloom_tmp16 "$user$ssfx_bloom_tmp16" // Bloom
#define r2_RT_ssfx_bloom_tmp32 "$user$ssfx_bloom_tmp32" // Bloom
#define r2_RT_ssfx_bloom_tmp64 "$user$ssfx_bloom_tmp64" // Bloom

#define r2_RT_ssfx_bloom_tmp32_2 "$user$ssfx_bloom_tmp32_2" // Bloom
#define r2_RT_ssfx_bloom_tmp16_2 "$user$ssfx_bloom_tmp16_2" // Bloom
#define r2_RT_ssfx_bloom_tmp8_2 "$user$ssfx_bloom_tmp8_2" // Bloom
#define r2_RT_ssfx_bloom_tmp4_2 "$user$ssfx_bloom_tmp4_2" // Bloom

#define r2_RT_ssfx_prevPos "$user$ssfx_prev_p" // Prev Position

#define JITTER(a) r2_jitter #a

constexpr float SMAP_near_plane = .1f;

constexpr u32 SMAP_adapt_min = 768; // 32	;
constexpr u32 SMAP_adapt_optimal = 768;
constexpr u32 SMAP_adapt_max = 1536;

constexpr u32 TEX_jitter = 64;
constexpr u32 TEX_jitter_count = 2; // Simp: реально в шейдерах используется только jitter0 и jitter1. Не понятно зачем вообще столько текстур шума, одной было бы достаточно.

constexpr u32 BLOOM_size_X = 256;
constexpr u32 BLOOM_size_Y = 256;
constexpr u32 LUMINANCE_size = 16;

// deffer
#define SE_R2_NORMAL_HQ 0 // high quality/detail
#define SE_R2_NORMAL_LQ 1 // low quality
#define SE_R2_SHADOW 2 // shadow generation

// spot
#define SE_L_FILL 0
#define SE_L_UNSHADOWED 1
#define SE_L_NORMAL 2 // typical, scaled
#define SE_L_FULLSIZE 3 // full texture coverage

// mask
#define SE_MASK_SPOT 0
#define SE_MASK_POINT 1
#define SE_MASK_DIRECT 2
#define SE_MASK_ACCUM_VOL 3
#define SE_MASK_ACCUM_2D 4
#define SE_MASK_ALBEDO 5

// sun
#define SE_SUN_NEAR 0
#define SE_SUN_MIDDLE 1
#define SE_SUN_FAR 2
#define SE_SUN_LUMINANCE 3
//	For rain R3 rendering
#define SE_SUN_RAIN_SMAP 5

extern float ps_r2_gloss_factor;
extern float ps_r2_gloss_min;

extern int ps_ssfx_gloss_method;
extern float ps_ssfx_gloss_factor;
extern Fvector3 ps_ssfx_gloss_minmax;

inline float u_diffuse2s(const float x, const float y, const float z)
{
    if (ps_ssfx_gloss_method == 0)
    {
        const float v = (x + y + z) / 3.f;
        return ps_r2_gloss_min + ps_r2_gloss_factor * ((v < 1) ? powf(v, 2.f / 3.f) : v);
    }
    else
    {
        // Remove sun from the equation and clamp value.
        return ps_ssfx_gloss_minmax.x + clampr(ps_ssfx_gloss_minmax.y - ps_ssfx_gloss_minmax.x, 0.0f, 1.0f) * ps_ssfx_gloss_factor;
    }
}

inline float u_diffuse2s(const Fvector3& c) { return u_diffuse2s(c.x, c.y, c.z); }
