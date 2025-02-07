#ifndef SLB_SHADOW_SETTINGS_H
#define SLB_SHADOW_SETTINGS_H

/// Macro mess
#define SLB_SHADOW_SETTINGS_ADD_PRESET(preset, method, pcss_filter_samples, pcss_size, pcss_size_min, pcss_light_width) \
    SLB_STATIC_CONST uint slb_shadow_settings_##preset##_method = method; \
    SLB_STATIC_CONST int slb_shadow_settings_##preset##_pcss_filter_samples = pcss_filter_samples; \
    SLB_STATIC_CONST float slb_shadow_settings_##preset##_pcss_size = pcss_size; \
    SLB_STATIC_CONST float slb_shadow_settings_##preset##_pcss_size_min = pcss_size_min; \
    SLB_STATIC_CONST float slb_shadow_settings_##preset##_pcss_light_width = pcss_light_width;

#define SLB_SHADOW_SETTINGS_APPLY_PRESET(preset) \
    SLB_STATIC_CONST uint slb_shadow_settings_method = slb_shadow_settings_##preset##_method; \
    SLB_STATIC_CONST int slb_shadow_settings_pcss_filter_samples = max(1, int(slb_shadow_settings_##preset##_pcss_filter_samples * slb_shadow_pcss_filter_samples_multiplier)); \
    SLB_STATIC_CONST float slb_shadow_settings_pcss_size = max(1, int(slb_shadow_settings_##preset##_pcss_size * slb_shadow_pcss_size_multiplier)); \
    SLB_STATIC_CONST float slb_shadow_settings_pcss_size_min = max(1, int(slb_shadow_settings_##preset##_pcss_size_min * slb_shadow_pcss_size_min_multiplier)); \
    SLB_STATIC_CONST float slb_shadow_settings_pcss_light_width = slb_shadow_settings_##preset##_pcss_light_width;

/// Enums
SLB_STATIC_CONST uint slb_shadow_method_linear = 0x252827bd;
SLB_STATIC_CONST uint slb_shadow_method_nice = 0xeaa1fbca;
SLB_STATIC_CONST uint slb_shadow_method_bicubic = 0xcb35b7d8;
SLB_STATIC_CONST uint slb_shadow_method_optimized_pcf = 0xcf7d64d5;
SLB_STATIC_CONST uint slb_shadow_method_chs = 0x15e654ad;
SLB_STATIC_CONST uint slb_shadow_method_pcf = 0x92726cc9;
SLB_STATIC_CONST uint slb_shadow_method_vogel_chs = 0x6f241210;
SLB_STATIC_CONST uint slb_shadow_method_pcss = 0xa9fa8994;
SLB_STATIC_CONST uint slb_shadow_method_pcssv2 = 0x4ab5f8c2;
SLB_STATIC_CONST uint slb_shadow_method_test = 0xfb1a7c5a;

/// Real settings starts here
/// slb_shadow_animated_noise: change noise every frame
/// slb_shadow_pcss_branch: higher quality for small filter radius
SLB_STATIC_CONST bool slb_shadow_animated_noise = false; /// default false
SLB_STATIC_CONST bool slb_shadow_pcss_branch = true; /// default true
SLB_STATIC_CONST float slb_shadow_pcss_filter_samples_multiplier = 1.0; /// default 1.0
SLB_STATIC_CONST float slb_shadow_pcss_size_multiplier = 1.0; /// default 1.0
SLB_STATIC_CONST float slb_shadow_pcss_size_min_multiplier = 1.0; /// default 1.0

/// Explanation:
///   sun_near - preset for sun near you
///   sun_near - preset for sun far from you
///   spot     - preset for spot lights e.g. flashlights
///   omni     - preset for omni lights e.g. some lamps
///   fallback - preset for ... rainlayer.ps IDK what it does

#ifndef SUN_QUALITY /// Low quality

SLB_SHADOW_SETTINGS_ADD_PRESET(sun_near, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(sun_far, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(spot, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(omni, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(fallback, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.1)

#elif SUN_QUALITY == 1 /// medium quality

SLB_SHADOW_SETTINGS_ADD_PRESET(sun_near, slb_shadow_method_optimized_pcf, 16, 8.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(sun_far, slb_shadow_method_optimized_pcf, 16, 8.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(spot, slb_shadow_method_optimized_pcf, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(omni, slb_shadow_method_optimized_pcf, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(fallback, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.1)

#elif SUN_QUALITY == 2 /// high quality

SLB_SHADOW_SETTINGS_ADD_PRESET(sun_near, slb_shadow_method_pcss, 8, 4.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(sun_far, slb_shadow_method_pcss, 8, 1.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(spot, slb_shadow_method_pcss, 8, 4.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(omni, slb_shadow_method_pcss, 8, 4.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(fallback, slb_shadow_method_bicubic, 8, 4.0, 2.0, 0.1)

#elif SUN_QUALITY == 3 /// ultra quality

SLB_SHADOW_SETTINGS_ADD_PRESET(sun_near, slb_shadow_method_pcss, 16, 8.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(sun_far, slb_shadow_method_pcss, 16, 2.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(spot, slb_shadow_method_pcss, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(omni, slb_shadow_method_pcss, 16, 8.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(fallback, slb_shadow_method_bicubic, 16, 8.0, 2.0, 0.1)

#elif SUN_QUALITY >= 4 /// extreme quality

SLB_SHADOW_SETTINGS_ADD_PRESET(sun_near, slb_shadow_method_pcssv2, 32, 16.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(sun_far, slb_shadow_method_pcssv2, 32, 4.0, 2.0, 0.1)
SLB_SHADOW_SETTINGS_ADD_PRESET(spot, slb_shadow_method_pcssv2, 32, 16.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(omni, slb_shadow_method_pcssv2, 32, 16.0, 2.0, 0.02)
SLB_SHADOW_SETTINGS_ADD_PRESET(fallback, slb_shadow_method_bicubic, 32, 16.0, 2.0, 0.1)

#endif

/// Real settings ends here

#if defined(SLB_SHADOW_SUN_NEAR)
SLB_SHADOW_SETTINGS_APPLY_PRESET(sun_near)
#elif defined(SLB_SHADOW_SUN_FAR)
SLB_SHADOW_SETTINGS_APPLY_PRESET(sun_far)
#elif defined(SLB_SHADOW_SPOT)
SLB_SHADOW_SETTINGS_APPLY_PRESET(spot)
#elif defined(SLB_SHADOW_OMNI)
SLB_SHADOW_SETTINGS_APPLY_PRESET(omni)
#else
SLB_SHADOW_SETTINGS_APPLY_PRESET(fallback)
#endif

#endif /// SLB_SHADOW_SETTINGS_H
