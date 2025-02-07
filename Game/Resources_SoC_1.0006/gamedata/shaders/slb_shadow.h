#ifndef SLB_SHADOW_H
#define SLB_SHADOW_H

#include "slb_common.h"
#include "slb_shadow_settings.h"

#ifdef SLB_GLSL /// As always dummy
float SLB_Shadow_Load(float2 uv) { return texelFetch(s_smap, int2(frac(uv) * float(SMAP_size)), 0).x; }

float SLB_Shadow_LoadOffset(float2 uv, int2 offset) { return texelFetch(s_smap, int2(frac(uv) * float(SMAP_size)) + offset, 0).x; }

float SLB_Shadow_SampleCmp(float2 uv, float z) { return textureLod(s_smap, uv, 0.0).x; }

float4 SLB_Shadow_Gather(float2 uv) { return textureGather(s_smap, uv); }

float4 SLB_Shadow_GatherOffset(float2 uv, int2 offset) { return textureGatherOffset(s_smap, uv, offset); }

float4 SLB_Shadow_GatherCmp(float2 uv, float z) { return textureGather(s_smap, uv); }
#else
float SLB_Shadow_Load(float2 uv) { return s_smap.Load(int3(uv * float(SMAP_size) - 0.25, 0)); }

float SLB_Shadow_LoadOffset(float2 uv, int2 offset) { return s_smap.Load(int3(uv * float(SMAP_size) - 0.25, 0), offset); }

float SLB_Shadow_SampleCmp(float2 uv, float z) { return s_smap.SampleCmpLevelZero(smp_smap, uv, z).x; }

float4 SLB_Shadow_Gather(float2 uv) { return s_smap.Gather(smp_nofilter, uv); }

float4 SLB_Shadow_GatherOffset(float2 uv, int2 offset) { return s_smap.Gather(smp_nofilter, uv, offset); }

float4 SLB_Shadow_GatherCmp(float2 uv, float z) { return s_smap.GatherCmp(smp_smap, uv, z); }
#endif

float SLB_Shadow_Linear(float4 tc) { return SLB_Shadow_SampleCmp(tc.xy, tc.z); }

/// Taken from https://www.shadertoy.com/view/XsfGDn
float SLB_Shadow_Nice(float4 tc)
{
    float2 uv = tc.xy;
    float2 textureResolution = float2(float(SMAP_size), float(SMAP_size));

    uv = uv * textureResolution + 0.5;

    float2 iuv = floor(uv);
    float2 fuv = frac(uv);

    uv = iuv + fuv * fuv * (3.0 - 2.0 * fuv);
    uv = (uv - 0.5) / textureResolution;

    return SLB_Shadow_SampleCmp(uv, tc.z);
}

/// Taken from http://www.java-gaming.org/index.php?topic=35123.0
float SLB_Shadow_Bicubic(float4 tc)
{
    float2 texCoords = tc.xy;

    float2 texSize = float2(float(SMAP_size), float(SMAP_size));
    float2 invTexSize = 1.0 / texSize;

    texCoords = texCoords * texSize - 0.5;

    float2 fxy = frac(texCoords);
    texCoords -= fxy;

    float4 xcubic = SLB_Cubic_For_Bicubic(fxy.x);
    float4 ycubic = SLB_Cubic_For_Bicubic(fxy.y);

    float4 c = texCoords.xxyy + float2(-0.5, +1.5).xyxy;

    float4 s = float4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    float4 offset = c + float4(xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    float sample0 = SLB_Shadow_SampleCmp(offset.xz, tc.z);
    float sample1 = SLB_Shadow_SampleCmp(offset.yz, tc.z);
    float sample2 = SLB_Shadow_SampleCmp(offset.xw, tc.z);
    float sample3 = SLB_Shadow_SampleCmp(offset.yw, tc.z);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return lerp(lerp(sample3, sample2, sx), lerp(sample1, sample0, sx), sy);
}

#include "slb_shadow_optimized_pcf.h"

float SLB_Shadow_CHS(float4 tc, float3 noise)
{
    float light_width = slb_shadow_settings_pcss_light_width;

    float average_blocker_depth = 0.0;

    {
        float average_blocker_div = 1e-6;

        float blocker_count = 0.0;
        int blocker_count_max = 0;

        for (int i = -3; i <= 3; i += 2)
        {
            for (int j = -3; j <= 3; j += 2)
            {
                float4 gather = SLB_Shadow_GatherOffset(tc.xy, int2(i, j));
                float4 weight = max(float4(0.0, 0.0, 0.0, 0.0), tc.z - gather);
                weight /= weight + 0.025;
#ifdef SLB_GLSL
                weight = 1.0 - gather;
#endif
                average_blocker_depth += dot(gather, weight);
                average_blocker_div += dot(weight, float4(1.0, 1.0, 1.0, 1.0));
                blocker_count += float(weight.x > 1e-6) + float(weight.y > 1e-6) + float(weight.z > 1e-6) + float(weight.w > 1e-6);
                blocker_count_max += 4;
            }
        }

        SLB_BRANCH
        if (blocker_count < 0.5 || blocker_count > float(blocker_count_max) - 0.5)
        {
            return 1.0 - blocker_count / float(blocker_count_max);
        }

        average_blocker_depth /= average_blocker_div;
    }

    float penumbra_size = light_width * (tc.z - average_blocker_depth) / average_blocker_depth;

    float lightness = SLB_Shadow_OptimizedPCF(tc);

    float scurve = clamp(3.0 / float(SMAP_size) / penumbra_size, 1.0, 100.0);
    lightness = saturate((lightness * scurve + 0.5) - 0.5 * scurve);

    return lightness;
}

float SLB_Shadow_VogelFilter(float4 tc, float3 noise, float filter_size, float filter_size_real, const int filter_samples, bool linear_sample, float radial_bias)
{
    const float compare_window = 0.0001;

    float lightness = 0.0;
    float div = 1e-6; /// Migitate division by zero
    float2x2 rot_mat = SLB_Rotate2Matrix(noise.x * SLB_TAU);

    if (linear_sample)
    {
        SLB_UNROLL(filter_samples)
        for (int i = 0; i < filter_samples; i++)
        {
            float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
            float offset_length = length(offset);
            offset = mul(offset, rot_mat);
            offset *= filter_size;

            float biased_z = tc.z - radial_bias * offset_length * filter_size_real;

            float2 uv = tc.xy + offset;
            float notblocker;

            {
                float4 depths = SLB_Shadow_Gather(uv);
                float4 notblockers = saturate((depths - biased_z + compare_window) / compare_window);
#ifdef SLB_GLSL
                notblockers = depths;
#endif

                float s1 = notblockers.x;
                float s2 = notblockers.y;
                float s3 = notblockers.z;
                float s4 = notblockers.w;

                float2 fract = frac(uv * float(SMAP_size) + 0.5);
                notblocker = lerp(lerp(s4, s3, fract.x), lerp(s1, s2, fract.x), fract.y);

                lightness += notblocker;
                div += 1.0;
            }
        }
    }
    else
    {
        SLB_UNROLL(filter_samples)
        for (int i = 0; i < filter_samples; i++)
        {
            float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
            float offset_length = length(offset);
            offset = mul(offset, rot_mat);
            offset *= filter_size;

            float biased_z = tc.z - radial_bias * offset_length * filter_size_real;

            float2 uv = tc.xy + offset;

            float depth = SLB_Shadow_Load(uv);
            float notblocker = saturate((depth - biased_z) / compare_window);
            float weight = float(depth > 1e-6);

#ifdef SLB_GLSL
            notblocker = depth;
            weight = 1.0;
#endif

            lightness += notblocker * weight;
            div += weight;
        }
    }

    lightness /= div;
    return lightness;
}

float SLB_Shadow_PCF(float4 tc, float3 noise)
{
    SLB_STATIC_CONST int filter_samples = slb_shadow_settings_pcss_filter_samples;
    float pcss_size_min = slb_shadow_settings_pcss_size_min / float(SMAP_size);
    float filter_size = pcss_size_min;

#if defined(SLB_SHADOW_SPOT)
    SLB_STATIC_CONST float radial_bias = 2.0;
#elif defined(SLB_SHADOW_OMNI)
    SLB_STATIC_CONST float radial_bias = 1.0;
#else
    SLB_STATIC_CONST float radial_bias = 0.3;
#endif

    float lightness = SLB_Shadow_VogelFilter(tc, noise, filter_size, filter_size / 2.0, filter_samples, true, radial_bias);

    lightness = SLB_QuarticRational1D(lightness);

    return lightness;
}

float SLB_Shadow_VogelCHS(float4 tc, float3 noise)
{
    SLB_STATIC_CONST int filter_samples = slb_shadow_settings_pcss_filter_samples;
    SLB_STATIC_CONST float light_width = slb_shadow_settings_pcss_light_width;
    float pcss_size_min = slb_shadow_settings_pcss_size_min / float(SMAP_size);
    float filter_size = pcss_size_min;

    SLB_STATIC_CONST int biases = 4;
    const float compare_window = 0.0001;

    const float filter_bias[biases] = {0.0, 0.1, 0.2, 0.3};

    float average_blocker_depth = 0.0;
    float average_blocker_div = 1e-6; /// Migitate division by zero

    float lightnesses[biases] = {0.0, 0.0, 0.0, 0.0};
    float lightnesses_div[biases] = {1e-6, 1e-6, 1e-6, 1e-6}; /// Migitate division by zero

    float2x2 rot_mat = SLB_Rotate2Matrix(noise.x * SLB_TAU);

    SLB_UNROLL(filter_samples)
    for (int i = 0; i < filter_samples; i++)
    {
        float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
        float offset_length = length(offset);
        offset = mul(offset, rot_mat);
        offset *= filter_size;

        float2 uv = tc.xy + offset;
        float4 depths = SLB_Shadow_Gather(uv);

        SLB_UNROLL(biases)
        for (int j = 0; j < biases; j++)
        {
            float biased_z = tc.z - filter_bias[j] * offset_length * filter_size;
            {
                float4 notblockers = saturate((depths - biased_z + compare_window) / compare_window);
#ifdef SLB_GLSL
                notblockers = depths;
#endif

                float s1 = notblockers.x;
                float s2 = notblockers.y;
                float s3 = notblockers.z;
                float s4 = notblockers.w;

                float2 fract = frac(uv * float(SMAP_size) + 0.5);
                float notblocker = lerp(lerp(s4, s3, fract.x), lerp(s1, s2, fract.x), fract.y);

                lightnesses[j] += notblocker;
                lightnesses_div[j] += 1.0;
            }
            {
                float4 blockers = float4(float(biased_z - 0.0001 * float(j - 1) > depths.x), float(biased_z - 0.0001 * float(j - 1) > depths.y),
                                         float(biased_z - 0.0001 * float(j - 1) > depths.z), float(biased_z - 0.0001 * float(j - 1) > depths.w));

                average_blocker_depth += dot(depths, blockers);
                average_blocker_div += dot(float4(1.0, 1.0, 1.0, 1.0), blockers);
            }
        }
    }

    SLB_UNROLL(biases)
    for (int i = 0; i < biases; i++)
    {
        lightnesses[i] /= lightnesses_div[i];
    }

    average_blocker_depth /= average_blocker_div;

    float penumbra_size = light_width * (tc.z - average_blocker_depth) / average_blocker_depth;

    /// Choose bias
    float lightness = lightnesses[0];
    SLB_UNROLL(biases - 1)
    for (int i = 1; i < biases; i++)
    {
        if (penumbra_size > float(i) / float(biases - 1) * filter_size)
        {
            lightness = lightnesses[i];
        }
    }

    float scurve = clamp(filter_size / penumbra_size, 1.0, 2.0 * float(filter_samples) / (filter_size * float(SMAP_size)));
    lightness = saturate((lightness * scurve + 0.5) - 0.5 * scurve);

    return lightness;
}

float SLB_Shadow_PCSS(float4 tc, float3 noise)
{
    /// Settings
    SLB_STATIC_CONST int filter_samples = slb_shadow_settings_pcss_filter_samples;
    float pcss_size = slb_shadow_settings_pcss_size / float(2048);
    float pcss_size_min = slb_shadow_settings_pcss_size_min / float(SMAP_size);
    float light_width = slb_shadow_settings_pcss_light_width;

    float filter_size_real;
    float filter_size;

#if defined(SLB_SHADOW_SPOT)
    SLB_STATIC_CONST float radial_bias = 2.0;
#elif defined(SLB_SHADOW_OMNI)
    SLB_STATIC_CONST float radial_bias = 1.0;
#else
    SLB_STATIC_CONST float radial_bias = 0.3;
#endif

    {
        /// Blocker search
        float blocker_size = pcss_size;

        float average_blocker_depth = tc.z * 1e-6;
        float average_blocker_div = 1e-6; /// Migitate division by zero

        /// depth for maximum filter size
        float depth_min = tc.z / (1 + light_width / blocker_size);

        float2x2 rot_mat = SLB_Rotate2Matrix(noise.x * SLB_TAU);

        SLB_UNROLL(filter_samples)
        for (int i = 0; i < filter_samples; i++)
        {
            float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
            {
                float x = length(offset);
                offset *= (0.3 * (1 - x * x * x) + x * x);
            }
            float offset_length = length(offset);
            offset = mul(offset, rot_mat);
            offset *= blocker_size;

            float depth = SLB_Shadow_Load(tc.xy + offset);
            float biased_z = tc.z - radial_bias * offset_length * blocker_size;
            float blocker = float(biased_z > depth && depth > 1e-6);

            /// limit minimal depth value for hopefully better average
            depth = max(depth_min, depth);

            blocker *= 1.0 / (offset_length + 0.01);

            average_blocker_depth += depth * blocker;
            average_blocker_div += blocker;
        }

        average_blocker_depth /= average_blocker_div;

        /// Calculate penumbra size
        filter_size = light_width * (tc.z - average_blocker_depth) / average_blocker_depth;

#if defined(SLB_SHADOW_SPOT) || defined(SLB_SHADOW_OMNI)
        filter_size *= 2 - average_blocker_depth;
#endif

#ifdef SLB_GLSL
        filter_size += 1;
#endif

        filter_size = min(pcss_size, filter_size);

#ifdef SLB_GLSL
        // filter_size *= frac(timers.y/10.0);
        // filter_size *= cos(timers.y/5.0)*0.5 + 0.5;
        // filter_size *= abs(frac(timers.y/2.0/8.0)*2.0 - 1.0)*1.0 + 0.0;
        // filter_size *= noise.z;
        // filter_size *= 0.0;
        // if(sin(tc.x*100.0) > 0.5) {
        //     filter_size = 2.9/float(SMAP_size);
        // } else {
        //     filter_size = 3.000/float(SMAP_size);
        // }
#endif

        filter_size_real = max(pcss_size_min / 1000.0, filter_size);
        filter_size = max(pcss_size_min, filter_size);
    }

    float lightness;
    float contrast_max = 2.0 * sqrt(float(filter_samples)) / (pcss_size_min * pcss_size_min * float(SMAP_size) * float(SMAP_size));

    SLB_BRANCH
    if (slb_shadow_pcss_branch && filter_size_real < pcss_size_min)
    {
        contrast_max *= 4.0;
        lightness = SLB_Shadow_VogelFilter(tc, noise, filter_size, filter_size_real, filter_samples, true, radial_bias);
    }
    else
    {
        lightness = SLB_Shadow_VogelFilter(tc, noise, filter_size, filter_size_real, filter_samples, false, radial_bias);
    }

    float contrast = clamp(filter_size / filter_size_real, 1.0, contrast_max);
    lightness = saturate(lightness * contrast - 0.5 * contrast + 0.5);

    return lightness;
}

float SLB_Shadow_PCSSv2(float4 tc, float3 noise)
{
    /// Settings
    SLB_STATIC_CONST int filter_samples = slb_shadow_settings_pcss_filter_samples;
    float pcss_size = slb_shadow_settings_pcss_size / float(2048);
    float pcss_size_min = slb_shadow_settings_pcss_size_min / float(SMAP_size);
    float light_width = slb_shadow_settings_pcss_light_width;

#if defined(SLB_SHADOW_SPOT)
    SLB_STATIC_CONST float radial_bias = 2.0;
#elif defined(SLB_SHADOW_OMNI)
    SLB_STATIC_CONST float radial_bias = 1.0;
#else
    SLB_STATIC_CONST float radial_bias = 0.3;
#endif

    float blocker_size;
    {
        float min_depth = 1.0;
        float2x2 rot_mat = SLB_Rotate2Matrix(noise.x * SLB_TAU);
        SLB_UNROLL(filter_samples)
        for (int i = 0; i < filter_samples; i++)
        {
            float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
            {
                float x = length(offset);
                offset *= (0.3 * (1 - x * x * x) + x * x);
            }
            offset = mul(offset, rot_mat);
            offset *= pcss_size;

            float depth = SLB_Shadow_Load(tc.xy + offset);
            depth = (depth > 1e-6) ? depth : 1.0;
            min_depth = min(depth, min_depth);
        }

        blocker_size = light_width * (tc.z - min_depth) / min_depth;
        blocker_size = clamp(blocker_size, pcss_size_min, pcss_size);
    }

    float filter_size_real;
    float filter_size;
    {
        /// Blocker search
        float average_blocker_depth = tc.z * 1e-6;
        float average_blocker_div = 1e-6; /// Migitate division by zero

        /// depth for maximum filter size
        float depth_min = tc.z / (1 + light_width / blocker_size);

        float2x2 rot_mat = SLB_Rotate2Matrix(noise.x * SLB_TAU);

        SLB_UNROLL(filter_samples)
        for (int i = 0; i < filter_samples; i++)
        {
            float2 offset = SLB_VogelDisk_Sample(i, filter_samples, noise.y);
            float offset_length = length(offset);
            offset = mul(offset, rot_mat);
            offset *= blocker_size;

            float depth = SLB_Shadow_Load(tc.xy + offset);
            float biased_z = tc.z - radial_bias * offset_length * blocker_size;
            float blocker = float(biased_z > depth && depth > 1e-6);

            /// average filter size not depth
            depth = max(depth_min, depth);

            average_blocker_depth += depth * blocker;
            average_blocker_div += blocker;
        }

        average_blocker_depth /= average_blocker_div;

        /// Calculate penumbra size
        filter_size = light_width * (tc.z - average_blocker_depth) / average_blocker_depth;

#if defined(SLB_SHADOW_SPOT) || defined(SLB_SHADOW_OMNI)
        filter_size *= 2 - average_blocker_depth;
#endif

#ifdef SLB_GLSL
        filter_size += 1;
#endif

        filter_size = min(pcss_size, filter_size);

#ifdef SLB_GLSL
        // filter_size *= frac(timers.y/10.0);
        // filter_size *= cos(timers.y/5.0)*0.5 + 0.5;
        // filter_size *= abs(frac(timers.y/2.0/8.0)*2.0 - 1.0)*1.0 + 0.0;
        // filter_size *= noise.z;
        // filter_size *= 0.0;
        // if(sin(tc.x*100.0) > 0.5) {
        //     filter_size = 2.9/float(SMAP_size);
        // } else {
        //     filter_size = 3.000/float(SMAP_size);
        // }
#endif

        filter_size_real = max(pcss_size_min / 1000.0, filter_size);
        filter_size = max(pcss_size_min, filter_size);
    }

    float lightness;
    float contrast_max = 2.0 * sqrt(float(filter_samples)) / (pcss_size_min * pcss_size_min * float(SMAP_size) * float(SMAP_size));

    SLB_BRANCH
    if (slb_shadow_pcss_branch && filter_size_real < pcss_size_min)
    {
        contrast_max *= 4.0;
        lightness = SLB_Shadow_VogelFilter(tc, noise, filter_size, filter_size_real, filter_samples, true, radial_bias);
    }
    else
    {
        lightness = SLB_Shadow_VogelFilter(tc, noise, filter_size, filter_size_real, filter_samples, false, radial_bias);
    }

    float contrast = clamp(filter_size / filter_size_real, 1.0, contrast_max);
    lightness = saturate(lightness * contrast - 0.5 * contrast + 0.5);

    return lightness;
}

/// Reserved
float SLB_Shadow_Test(float4 tc, float3 noise) { return 1.0; }

float SLB_Shadow(float4 tc, float2 pos2d)
{
    uint2 noise_uv = uint2(pos2d + 0.5);
    if (slb_shadow_animated_noise)
    {
        uint noise_offset = SLB_PCG(asuint(timers.y));
        noise_uv += uint2(noise_offset, noise_offset);
    }
    float3 noise = float3(SLB_HilbertNoise_float(noise_uv * 2), SLB_HilbertNoise_float(noise_uv + 105), SLB_HilbertNoise_float(noise_uv));

    tc.xyz /= tc.w;

    float lightness = 1.0;

    if (slb_shadow_settings_method == slb_shadow_method_linear)
    {
        lightness = SLB_Shadow_Linear(tc);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_nice)
    {
        lightness = SLB_Shadow_Nice(tc);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_bicubic)
    {
        lightness = SLB_Shadow_Bicubic(tc);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_optimized_pcf)
    {
        lightness = SLB_QuadraticRational1D(SLB_Shadow_OptimizedPCF(tc));
    }
    else if (slb_shadow_settings_method == slb_shadow_method_chs)
    {
        lightness = SLB_Shadow_CHS(tc, noise);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_pcf)
    {
        lightness = SLB_Shadow_PCF(tc, noise);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_vogel_chs)
    {
        lightness = SLB_Shadow_VogelCHS(tc, noise);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_pcss)
    {
        lightness = SLB_Shadow_PCSS(tc, noise);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_pcssv2)
    {
        lightness = SLB_Shadow_PCSSv2(tc, noise);
    }
    else if (slb_shadow_settings_method == slb_shadow_method_test)
    {
        lightness = SLB_Shadow_Test(tc, noise);
    }

    return lightness;
}

#endif /// SLB_SHADOW_H
