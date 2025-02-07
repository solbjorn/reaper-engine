#ifndef SLB_SHADOW_OPTIMIZED_PCF_H
#define SLB_SHADOW_OPTIMIZED_PCF_H

/// Source https://github.com/TheRealMJP/Shadows/blob/master/Shadows/Mesh.hlsl

float SLB_Shadow_OptimizedPCF_Sample(in float2 base_uv, in float u, in float v, in float depth)
{
    float2 shadowMapSize = float2(int2(SMAP_size, SMAP_size));
    float2 shadowMapSizeInv = 1.0 / shadowMapSize;

    float2 uv = base_uv + float2(u, v) * shadowMapSizeInv;

    const float compare_window = 0.0001;
    {
        float4 depths = SLB_Shadow_Gather(uv);
        float4 notblockers = saturate((depths - depth + compare_window) / compare_window);
#ifdef SLB_GLSL
        notblockers = depths;
#endif

        float s1 = notblockers.x;
        float s2 = notblockers.y;
        float s3 = notblockers.z;
        float s4 = notblockers.w;

        float2 fract = frac(uv * float(SMAP_size) + (-0.5 + 1.0 / 512.0));
        float notblocker = lerp(lerp(s4, s3, fract.x), lerp(s1, s2, fract.x), fract.y);

        return notblocker;
    }
}

float SLB_Shadow_OptimizedPCF(float4 tc)
{
    const int filter_size = 7;

    float2 shadowMapSize = float2(int2(SMAP_size, SMAP_size));
    float2 shadowMapSizeInv = 1.0 / shadowMapSize;
    float2 shadowPos = tc.xy;
    float lightDepth = tc.z;

    float2 uv = shadowPos.xy * shadowMapSize; // 1 unit - 1 texel

    float2 base_uv;
    base_uv.x = floor(uv.x + 0.5);
    base_uv.y = floor(uv.y + 0.5);

    float s = (uv.x + 0.5 - base_uv.x);
    float t = (uv.y + 0.5 - base_uv.y);

    base_uv -= float2(0.5, 0.5);
    base_uv *= shadowMapSizeInv;

    float sum = 0;

    if (filter_size == 2)
    {
        return SLB_Shadow_SampleCmp(shadowPos.xy, lightDepth);
    }
    else if (filter_size == 3)
    {
        float uw0 = (3 - 2 * s);
        float uw1 = (1 + 2 * s);

        float u0 = (2 - s) / uw0 - 1;
        float u1 = s / uw1 + 1;

        float vw0 = (3 - 2 * t);
        float vw1 = (1 + 2 * t);

        float v0 = (2 - t) / vw0 - 1;
        float v1 = t / vw1 + 1;

        sum += uw0 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v0, lightDepth);
        sum += uw1 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v0, lightDepth);
        sum += uw0 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v1, lightDepth);
        sum += uw1 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v1, lightDepth);

        return sum * 1.0f / 16;
    }
    else if (filter_size == 5)
    {
        float uw0 = (4 - 3 * s);
        float uw1 = 7;
        float uw2 = (1 + 3 * s);

        float u0 = (3 - 2 * s) / uw0 - 2;
        float u1 = (3 + s) / uw1;
        float u2 = s / uw2 + 2;

        float vw0 = (4 - 3 * t);
        float vw1 = 7;
        float vw2 = (1 + 3 * t);

        float v0 = (3 - 2 * t) / vw0 - 2;
        float v1 = (3 + t) / vw1;
        float v2 = t / vw2 + 2;

        sum += uw0 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v0, lightDepth);
        sum += uw1 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v0, lightDepth);
        sum += uw2 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v0, lightDepth);

        sum += uw0 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v1, lightDepth);
        sum += uw1 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v1, lightDepth);
        sum += uw2 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v1, lightDepth);

        sum += uw0 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v2, lightDepth);
        sum += uw1 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v2, lightDepth);
        sum += uw2 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v2, lightDepth);

        return sum * 1.0f / 144;
    }
    else if (filter_size == 7)
    {
        float uw0 = (5 * s - 6);
        float uw1 = (11 * s - 28);
        float uw2 = -(11 * s + 17);
        float uw3 = -(5 * s + 1);

        float u0 = (4 * s - 5) / uw0 - 3;
        float u1 = (4 * s - 16) / uw1 - 1;
        float u2 = -(7 * s + 5) / uw2 + 1;
        float u3 = -s / uw3 + 3;

        float vw0 = (5 * t - 6);
        float vw1 = (11 * t - 28);
        float vw2 = -(11 * t + 17);
        float vw3 = -(5 * t + 1);

        float v0 = (4 * t - 5) / vw0 - 3;
        float v1 = (4 * t - 16) / vw1 - 1;
        float v2 = -(7 * t + 5) / vw2 + 1;
        float v3 = -t / vw3 + 3;

        sum += uw0 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v0, lightDepth);
        sum += uw1 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v0, lightDepth);
        sum += uw2 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v0, lightDepth);
        sum += uw3 * vw0 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u3, v0, lightDepth);

        sum += uw0 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v1, lightDepth);
        sum += uw1 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v1, lightDepth);
        sum += uw2 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v1, lightDepth);
        sum += uw3 * vw1 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u3, v1, lightDepth);

        sum += uw0 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v2, lightDepth);
        sum += uw1 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v2, lightDepth);
        sum += uw2 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v2, lightDepth);
        sum += uw3 * vw2 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u3, v2, lightDepth);

        sum += uw0 * vw3 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u0, v3, lightDepth);
        sum += uw1 * vw3 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u1, v3, lightDepth);
        sum += uw2 * vw3 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u2, v3, lightDepth);
        sum += uw3 * vw3 * SLB_Shadow_OptimizedPCF_Sample(base_uv, u3, v3, lightDepth);

        return sum * 1.0f / 2704;
    }
    else
    {
        return 1.0;
    }
}

#endif /// SLB_SHADOW_OPTIMIZED_PCF_H
