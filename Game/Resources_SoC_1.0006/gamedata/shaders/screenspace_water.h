/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 22
 * @ Description: Water implementation
 * @ Modified time: 2024-11-16 07:21
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#include "screenspace_common.h"
#include "settings_screenspace_WATER.h"

static const int2 q_steps[5] = {int2(8, 3), int2(16, 2), int2(24, 2), int2(32, 2), int2(64, 1)};

float3 SSFX_ssr_water_ray(float3 ray_start_vs, float3 ray_dir_vs, float noise, out float HitDepth, float2 tc, uint iSample : SV_SAMPLEINDEX)
{
    // Some vars to use later...
    float4 prev_step;
    int prev_sign;
    float3 behind_hit = 0;

    HitDepth = 0;

    float RayThick = clamp(48.0f / q_steps[SSFX_WATER_QUALITY].x, 1.0f, 3.0f);

    // Initialize Ray
    RayTrace ssr_ray = SSFX_ray_init(ray_start_vs, ray_dir_vs, 150, q_steps[SSFX_WATER_QUALITY].x, noise);

    // Horizontal stretch to avoid borders
    float2 stretch_uv = 1.0f - smoothstep(0.9f, 1.0f, float2(tc.x, 1.0 - tc.x));

    // Apply stretch
    ssr_ray.r_step.x *= saturate(stretch_uv.x * stretch_uv.y + saturate(eye_direction.y * 3.0f));

    // Adjust the starting Point
    ssr_ray.r_pos = ssr_ray.r_start + ssr_ray.r_step * noise;

    // Depth from the start of the ray
    float ray_depthstart = SSFX_get_depth(ssr_ray.r_start, iSample);

    // Ray-march
    [unroll(q_steps[SSFX_WATER_QUALITY].x)] for (int st = 1; st <= q_steps[SSFX_WATER_QUALITY].x; st++)
    {
        // Ray out of screen...
        if (!SSFX_is_valid_uv(ssr_ray.r_pos.xy))
            return 0;

        // Ray intersect check ( x = difference | y = depth sample )
        float3 ray_check = SSFX_ray_intersect(ssr_ray, iSample);

        HitDepth = ray_check.y;

        // Sampled depth is not weapon or sky ( SKY_EPS float(0.001) )
        bool NoWpnSky = ray_check.y > 1.3f;

        // Don't want interaction with weapons or sky
        ray_check.x *= NoWpnSky;

        // Depth difference positive...
        if (ray_check.x > 0)
        {
            // Conditions to use as reflection...
            if (ray_check.x <= RayThick || (ray_depthstart + 40.0f < ray_check.y))
                return float3(ssr_ray.r_pos, ray_check.y);

            // Current ray pos & step to use later...
            prev_step.xy = ssr_ray.r_pos;
            prev_step.zw = ssr_ray.r_step;

            prev_sign = -1;

            // Binary Search
            for (int x = 0; x < q_steps[SSFX_WATER_QUALITY].y; x++)
            {
                // Half and flip depending on depth difference sign
                if (sign(ray_check.x) != prev_sign)
                {
                    ssr_ray.r_step *= -0.5f;
                    prev_sign = sign(ray_check.x);
                }

                // Step ray
                ssr_ray.r_pos += ssr_ray.r_step;

                // Ray intersect check
                ray_check = SSFX_ray_intersect(ssr_ray, iSample);

                // Depth test... Conditions to use as reflections...
                if (abs(ray_check.x) <= RayThick)
                    return float3(ssr_ray.r_pos, ray_check.y);
            }

            // Restore previous ray position & step
            ssr_ray.r_pos = prev_step.xy;
            ssr_ray.r_step = prev_step.zw;
        }
        else
        {
            // Sample behind... Let's use this coor to fill...
            behind_hit = float3(ssr_ray.r_pos, ray_check.y);

            // Reset or keep depending on... ( > 1.3f = no interaction with weapons and sky )
            behind_hit *= (ray_depthstart - 2.0f < ray_check.y) && NoWpnSky;
        }

        // Pass through condition
        bool PTh = (!NoWpnSky && ray_check.y > 0.01f && st > q_steps[SSFX_WATER_QUALITY].x * 0.4f);

        // Step ray... Try to pass through closer objects ( Like weapons and sights )
        ssr_ray.r_pos += ssr_ray.r_step * (1.0f + 2.5f * PTh);
    }

    return behind_hit;
}
