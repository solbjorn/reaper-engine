#ifndef common_functions_h_included
#define common_functions_h_included

#include "srgb.h"
#include "cgim.h"

//	contrast function
float Contrast(float Input, float ContrastPower)
{
    // piecewise contrast function
    bool IsAboveHalf = Input > 0.5;
    float ToRaise = saturate(2 * (IsAboveHalf ? 1 - Input : Input));
    float Output = 0.5 * pow(ToRaise, ContrastPower);
    Output = IsAboveHalf ? 1 - Output : Output;
    return Output;
}

float3 vibrance(float3 img, float val)
{
#ifdef CGIM2
    return float3(lerp(Luminance(img), img, val));
#else
    float luminance = dot(float3(img.rgb), LUMINANCE_VECTOR);
    return float3(lerp(luminance, float3(img.rgb), val));
#endif
}

void tonemap(out float4 low, out float4 high, float3 rgb, float scale)
{
#if defined(ES) && !defined(CGIM2)
    rgb = SRGBToLinear(rgb);
#if !defined(ACES_GRADING) || defined(ATMOS)
    scale = SRGBToLinear(scale);
#endif
    rgb = rgb * scale;
    rgb = LinearTosRGB(rgb);
#else
    rgb = rgb * scale;
#endif

#ifdef ES
    const float fWhiteIntensity = 11.2;

#ifdef CGIM2
    low = float4(TonemapFunctionGet(rgb, fWhiteIntensity).xyz, 0);
#else
    low = float4(tonemap_sRGB(rgb, fWhiteIntensity), 0);
#endif /* !CGIM2 */
#else /* !ES */
    const float fWhiteIntensitySQR = 1.7 * 1.7;

    low = float4(((rgb * (1 + rgb / fWhiteIntensitySQR)) / (rgb + 1)).xyz, 0);
#endif /* !ES */

    high = float4(rgb / def_hdr, 0);
}

void tonemap_hipri(out float4 low, out float4 high, float3 rgb, float scale) { tonemap(low, high, rgb, scale); }

float3 compute_colored_ao(float ao, float3 albedo)
{ // https://www.activision.com/cdn/research/s2016_pbs_activision_occlusion.pptx
    float3 a = 2.0404 * albedo - 0.3324;
    float3 b = -4.7951 * albedo + 0.6417;
    float3 c = 2.7552 * albedo + 0.6903;

    return max(ao, ((ao * a + b) * ao + c) * ao);
}

// CUSTOM
float3 blend_soft(float3 a, float3 b)
{
#if defined(ES) && !defined(CGIM2)
    // gamma correct and inverse tonemap to add bloom
    a = SRGBToLinear(a); // post tonemap render
    a = a / max(0.004, 1 - a); // inverse tonemap
    // a = a / max(0.001, 1-a); //inverse tonemap
    b = SRGBToLinear(b); // bloom

#ifdef ACES_GRADING
    // constrast reduction of ACES output
    float Contrast_Amount = 0.7;
    const float mid = 0.18;
    a = pow(a, Contrast_Amount) * mid / pow(mid, Contrast_Amount);

    ACES_LMT(b); // color grading bloom
#endif /* ACES_GRADING */
#endif /* ES && !CGIM2 */

    a += b; // bloom add

#if defined(ES) && !defined(CGIM2)
#ifdef ACES_GRADING
    // Boost the contrast to match ACES RRT
    float Contrast_Boost = 1.42857;
    a = pow(a, Contrast_Boost) * mid / pow(mid, Contrast_Boost);
#endif /* ACES_GRADING */

    a = a / (1 + a); // tonemap

    a = LinearTosRGB(a);
#endif /* ES && !CGIM2 */

    return a;
}

float calc_fogging(float4 w_pos) { return dot(w_pos, fog_plane); }

float2 unpack_tc_base(float2 tc, float du, float dv)
{
    return (tc.xy + float2(du, dv)) * (32.f / 32768.f); //! Increase from 32bit to 64bit floating point
}

float3 calc_sun_r1(float3 norm_w) { return L_sun_color * saturate(dot((norm_w), -L_sun_dir_w)); }

float3 calc_model_hemi_r1(float3 norm_w) { return max(0, norm_w.y) * L_hemi_color; }

float3 calc_model_lq_lighting(float3 norm_w) { return L_material.x * calc_model_hemi_r1(norm_w) + L_ambient + L_material.y * calc_sun_r1(norm_w); }

float3 unpack_normal(float3 v) { return 2 * v - 1; }
float3 unpack_bx2(float3 v) { return 2 * v - 1; }
float3 unpack_bx4(float3 v) { return 4 * v - 2; } //! reduce the amount of stretching from 4*v-2 and increase precision
float2 unpack_tc_lmap(float2 tc) { return tc * (1.f / 32768.f); } // [-1  .. +1 ]
float4 unpack_color(float4 c) { return c.bgra; }
float4 unpack_D3DCOLOR(float4 c) { return c.bgra; }
float3 unpack_D3DCOLOR(float3 c) { return c.bgr; }

float3 p_hemi(float2 tc)
{
    float4 t_lmh = s_hemi.Sample(smp_rtlinear, tc);
    return dot(t_lmh.rgb, 1.f / 3.f);
}

float get_hemi(float4 lmh) { return dot(lmh.rgb, 1.f / 3.f); }

float get_sun(float4 lmh) { return lmh.a; }

float3 v_hemi(float3 n) { return L_hemi_color * (.5f + .5f * n.y); }

float3 v_sun(float3 n) { return L_sun_color * dot(n, -L_sun_dir_w); }

float3 calc_reflection(float3 pos_w, float3 norm_w) { return reflect(normalize(pos_w - eye_position), norm_w); }

#ifndef SKY_WITH_DEPTH
float is_sky(float depth) { return step(depth, SKY_EPS); }
float is_not_sky(float depth) { return step(SKY_EPS, depth); }
#else
float is_sky(float depth) { return step(abs(depth - SKY_DEPTH), SKY_EPS); }
float is_not_sky(float depth) { return step(SKY_EPS, abs(depth - SKY_DEPTH)); }
#endif

float hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float2 hash22(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.xx + p3.yz) * p3.zy);
}

float noise(float2 tc) { return frac(sin(dot(tc, float2(12.f, 78.f) + (timers.x))) * 43758.f) * 0.25f; }

//////////////////////////////////////////////////////////////////////////
//	Aplha to coverage code

#if MSAA_SAMPLES == 2
uint alpha_to_coverage(float alpha, float2 pos2d)
{
    uint mask;
    uint pos = uint(pos2d.x) | uint(pos2d.y);
    if (alpha < 0.3333)
        mask = 0;
    else if (alpha < 0.6666)
        mask = 1 << (pos & 1);
    else
        mask = 3;

    return mask;
}
#endif

#if MSAA_SAMPLES == 4
uint alpha_to_coverage(float alpha, float2 pos2d)
{
    uint mask;

    float off = float((uint(pos2d.x) | uint(pos2d.y)) & 3);
    alpha = saturate(alpha - off * ((0.2 / 4.0) / 3.0));
    if (alpha < 0.40)
    {
        if (alpha < 0.20)
            mask = 0;
        else if (alpha < 0.40) // only one bit set
            mask = 1;
    }
    else
    {
        if (alpha < 0.60) // 2 bits set => 1100 0110 0011 1001 1010 0101
        {
            mask = 3;
        }
        else if (alpha < 0.8) // 3 bits set => 1110 0111 1011 1101
            mask = 7;
        else
            mask = 0xf;
    }

    return mask;
}
#endif

#if MSAA_SAMPLES == 8
uint alpha_to_coverage(float alpha, float2 pos2d)
{
    uint mask;

    float off = float((uint(pos2d.x) | uint(pos2d.y)) & 3);
    alpha = saturate(alpha - off * ((0.1111 / 8.0) / 3.0));
    if (alpha < 0.4444)
    {
        if (alpha < 0.2222)
        {
            if (alpha < 0.1111)
                mask = 0;
            else // only one bit set 0.2222
                mask = 1;
        }
        else
        {
            if (alpha < 0.3333) // 2 bits set0=> 10000001 + 11000000 .. 00000011 : 8 // 0.2222
                                //        set1=> 10100000 .. 00000101 + 10000010 + 01000001 : 8
                                //		set2=> 10010000 .. 00001001 + 10000100 + 01000010 + 00100001 : 8
                                //		set3=> 10001000 .. 00010001 + 10001000 + 01000100 + 00100010 + 00010001 : 8
            {
                mask = 3;
            }
            else // 3 bits set0 => 11100000 .. 00000111 + 10000011 + 11000001 : 8 ? 0.4444 // 0.3333
                 //        set1 => 10110000 .. 00001011 + 10000101 + 11000010 + 01100001: 8
                 //        set2 => 11010000 .. 00001101 + 10000110 + 01000011 + 10100001: 8
                 //        set3 => 10011000 .. 00010011 + 10001001 + 11000100 + 01100010 + 00110001 : 8
                 //        set4 => 11001000 .. 00011001 + 10001100 + 01000110 + 00100011 + 10010001 : 8
            {
                mask = 0x7;
            }
        }
    }
    else
    {
        if (alpha < 0.6666)
        {
            if (alpha < 0.5555) // 4 bits set0 => 11110000 .. 00001111 + 10000111 + 11000011 + 11100001 : 8 // 0.5555
                                //        set1 => 11011000 .. 00011011 + 10001101 + 11000110 + 01100011 + 10110001 : 8
                                //        set2 => 11001100 .. 00110011 + 10011001 : 4 make 8
                                //        set3 => 11000110 + 01100011 + 10110001 + 11011000 + 01101100 + 00110110 + 00011011 + 10001101 : 8
                                //        set4 => 10111000 .. 00010111 + 10001011 + 11000101 + 11100010 + 01110001 : 8
                                //        set5 => 10011100 .. 00100111 + 10010011 + 11001001 + 11100100 + 01110010 + 00111001 : 8
                                //        set6 => 10101010 .. 01010101 : 2 make 8
                                //        set7 => 10110100 +  01011010 + 00101101 + 10010110 + 01001011 + 10100101 + 11010010 + 01101001 : 8
                                //        set8 => 10011010 +  01001101 + 10100110 + 01010011 + 10101001 + 11010100 + 01101010 + 00110101 : 8
            {
                mask = 0xf;
            }
            else // 5 bits set0 => 11111000 01111100 00111110 00011111 10001111 11000111 11100011 11110001 : 8  // 0.6666
                 //        set1 => 10111100 : 8
                 //        set2 => 10011110 : 8
                 //        set3 => 11011100 : 8
                 //        set4 => 11001110 : 8
                 //        set5 => 11011010 : 8
                 //        set6 => 10110110 : 8
            {
                mask = 0x1F;
            }
        }
        else
        {
            if (alpha < 0.7777) // 6 bits set0 => 11111100 01111110 00111111 10011111 11001111 11100111 11110011 11111001 : 8
                                //        set1 => 10111110 : 8
                                //        set2 => 11011110 : 8
            {
                mask = 0x3F;
            }
            else if (alpha < 0.8888) // 7 bits set0 => 11111110 :8
            {
                mask = 0x7F;
            }
            else // all 8 bits set
                mask = 0xFF;
        }
    }

    return mask;
}
#endif

//////////////////////////////////////////[SWM]///////////////////////////////////////////
uniform float4 m_blender_mode; // x = [0 - default, 1 - night vision, 2 - thermal vision]; y = [0.0f / 1.0f - происходит ли в данный момент рендеринг картинки для прицела]; z =
                               // [0.0f / 1.0f - выключен или включён двойной рендер]; w - зарезервировано на будущее.

// Активен-ли двойной рендер?
inline bool isSecondVPActive() { return (m_blender_mode.z == 1.f); }

// Рендерится ли в данный момент кадр для прицела?
inline bool IsSVPFrame() { return (m_blender_mode.y == 1.f); }
//////////////////////////////////////////////////////////////////////////////////////////

#endif //	common_functions_h_included
