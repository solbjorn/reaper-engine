/**
 * @ Description: Enhanced Shaders and Color Grading 1.10
 * @ Author: https://www.moddb.com/members/kennshade
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#ifndef HMODEL_H
#define HMODEL_H
#define CUBE_MIPS 6 // mipmaps for ambient shading and specular

// gamma correction is set up to be semi gamma correct

TextureCube env_s0;
TextureCube env_s1;

uniform float4 env_color; // color.w = lerp factor

#ifdef CGIM2
float3 SmallSkyCompute(float3 s0, float3 s1, float3 uv)
{
    float3 color = lerp(s0, s1, env_color.w);

    float topToDownVec = saturate(uv.y);
    topToDownVec *= topToDownVec;

    float factor = SMALLSKY_TOP_VECTOR_POWER;
    factor = saturate(factor + (1.0 - factor) * topToDownVec) + (1.0 - factor) * 0.5f;

    color *= factor * factor;
    return color;
}
#endif

void hmodel(out float3 hdiffuse, out float3 hspecular, float m, float h, float4 alb_gloss, float3 Pnt, float3 normal)
{
    // [ SSS Test ]. Overwrite terrain material
    bool m_terrain = abs(m - 0.95) <= 0.04f;
    bool m_flora = abs(m - 0.15) <= 0.04f;
    if (m_terrain)
        m = 0;

    // PBR style
    float3 albedo = calc_albedo(alb_gloss, m);
    float3 specular = calc_specular(alb_gloss, m);
    float rough = calc_rough(alb_gloss, m);
    // calc_rain(albedo, specular, rough, alb_gloss, m, h);
    // calc_foliage(albedo, specular, rough, float4(0.05,0.05,0.05,0.05), m);

    float roughCube = rough;
    // float roughCube = sqrt(rough); //cubemap mipmaps (brdf too?)

    // float RoughMip = roughCube * CUBE_MIPS;
    float RoughMip = CUBE_MIPS - ((1 - roughCube) * CUBE_MIPS);

    // normal vector
    normal = normalize(normal);
    float3 nw = mul(m_inv_V, normal);
    // nw = normalize(nw);

    // view vector
    Pnt = normalize(Pnt);
    float3 v2Pnt = mul(m_inv_V, Pnt);
    // v2Pnt = normalize(v2Pnt);

    // normal remap
    float3 nwRemap = nw;
    float3 vnormabs = abs(nwRemap);
    float vnormmax = max(vnormabs.x, max(vnormabs.y, vnormabs.z));
    nwRemap /= vnormmax;
    if (nwRemap.y < 0.999)
        nwRemap.y = nwRemap.y * 2 - 1; // fake remapping

    // reflection vector
    float3 vreflect = reflect(v2Pnt, nw);

    // reflect remap
    float3 vreflectRemap = vreflect;
    float3 vreflectabs = abs(vreflectRemap);
    float vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
    vreflectRemap /= vreflectmax;
    if (vreflectRemap.y < 0.999)
        vreflectRemap.y = vreflectRemap.y * 2 - 1; // fake remapping

    // normalize
    nwRemap = normalize(nwRemap);
    vreflectRemap = normalize(vreflectRemap);

    // DICE reflection vector roughness
    vreflectRemap = getSpecularDominantDir(nwRemap, vreflectRemap, rough);

    // Valve style ambient cube to prevent seams
    const float Epsilon = 0.001;
    float3 nSquared = nw * nw;

    float3 e0d = 0;
    e0d += nSquared.x * (env_s0.SampleLevel(smp_base, float3(nwRemap.x, Epsilon, Epsilon), CUBE_MIPS).rgb);
    e0d += nSquared.y * (env_s0.SampleLevel(smp_base, float3(Epsilon, nwRemap.y, Epsilon), CUBE_MIPS).rgb);
    e0d += nSquared.z * (env_s0.SampleLevel(smp_base, float3(Epsilon, Epsilon, nwRemap.z), CUBE_MIPS).rgb);
    // e0d = LinearTosRGB(e0d);

    // e0d = env_s0.SampleLevel(smp_base, nwRemap, CUBE_MIPS);

    float3 e1d = 0;
    e1d += nSquared.x * (env_s1.SampleLevel(smp_base, float3(nwRemap.x, Epsilon, Epsilon), CUBE_MIPS).rgb);
    e1d += nSquared.y * (env_s1.SampleLevel(smp_base, float3(Epsilon, nwRemap.y, Epsilon), CUBE_MIPS).rgb);
    e1d += nSquared.z * (env_s1.SampleLevel(smp_base, float3(Epsilon, Epsilon, nwRemap.z), CUBE_MIPS).rgb);
    // e1d = LinearTosRGB(e1d);

    // e1d = env_s1.SampleLevel(smp_base, nwRemap, CUBE_MIPS);

    // specular color
    float3 e0s = env_s0.SampleLevel(smp_base, vreflectRemap, RoughMip);
    float3 e1s = env_s1.SampleLevel(smp_base, vreflectRemap, RoughMip);

    // lerp
#ifdef CGIM2
    float3 env_d = SmallSkyCompute(e0d, e1d, nwRemap);
    float3 env_s = SmallSkyCompute(e0s, e1s, vreflectRemap);
#else
    float3 env_d = lerp(e0d, e1d, env_color.w);
    float3 env_s = lerp(e0s, e1s, env_color.w);
#endif

    // hscale - something like diffuse reflection
    float hscale = h; //. * (.5h + .5h*nw.y);
    float hspec = .5f + .5f * dot(vreflect, v2Pnt);

    // TODO - make hscale normal mapped
    float4 light = float4(hscale, hscale, hscale, hscale);

    // tint color
    // float3 env_col = 1.0;
    float3 env_col = env_color.rgb;
    // float3 env_col = fog_color.rgb * 2.0;

    env_d *= env_col;
    env_s *= env_col;

    // lightmap ambient
    env_d *= light.xxx;
    env_s *= light.www;

    // ambient color
    float3 amb_col = L_ambient.rgb;
    env_d += amb_col;
    env_s += amb_col; //*(env_s/env_d);

    env_d = SRGBToLinear(env_d);
    env_s = SRGBToLinear(env_s); // gamma correct

    env_s *= 1.0f - m_flora * 0.75f;

    hdiffuse = Amb_BRDF(rough, albedo, specular, env_d, env_s, -v2Pnt, nw).rgb;
    hspecular = 0; // do not use hspec at all
}
#endif
