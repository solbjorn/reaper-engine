/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 19
 * @ Description: Trees - Trunk
 * @ Modified time: 2023-12-16 13:58
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#include "common.h"
#include "check_screenspace.h"

uniform float3x4 m_xform;
uniform float3x4 m_xform_v;
uniform float4 consts; // {1/quant,1/quant,???,???}
uniform float4 c_scale, c_bias, wind, wave;
uniform float2 c_sun; // x=*, y=+

#ifdef SSFX_WIND
#include "screenspace_wind.h"
#endif

v2p_bumped main(v_tree I)
{
    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);
    float H = pos.y - m_xform._24; // height of vertex

#ifndef SSFX_WIND
    float dp = calc_cyclic(wave.w + dot(pos, (float3)wave));
    float frac = I.tc.z * consts.x; // fractional (or rigidity)
    float inten = H * dp; // intensity
    float2 wind_result = calc_xz_wave(wind.xz * inten, frac);
#else
    float2 wind_result = ssfx_wind_tree_trunk(pos, H, ssfx_wind_setup()).xy;
#endif

#ifdef USE_TREEWAVE
    wind_result = 0;
#endif
    float4 w_pos = float4(pos.x + wind_result.x, pos.y, pos.z + wind_result.y, 1);
    float2 tc = (I.tc * consts).xy;
    float hemi = clamp(I.Nh.w * c_scale.w + c_bias.w, 0.3f, 1.0f); // Limit hemi - SSS Update 14.5
    //	float 	hemi 	= I.Nh.w;

    // Eye-space pos/normal
    v2p_bumped O;
    float3 Pe = mul(m_V, w_pos);
    O.tcdh = float4(tc.xyyy);
    O.hpos = mul(m_VP, w_pos);
    O.position = float4(Pe, hemi);

    // Calculate the 3x3 transform from tangent space to eye-space
    // TangentToEyeSpace = object2eye * tangent2object
    //		     = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
    // Normal mapping

    // FLORA FIXES & IMPROVEMENTS - SSS Update 14
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
    // Use real tree Normal, Tangent and Binormal.
    float3 N = unpack_bx4(I.Nh);
    float3 T = unpack_bx4(I.T);
    float3 B = unpack_bx4(I.B);
    float3x3 xform = mul((float3x3)m_xform_v, float3x3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z));

    // The pixel shader operates on the bump-map in [0..1] range
    // Remap this range in the matrix, anyway we are pixel-shader limited :)
    // ...... [ 2  0  0  0]
    // ...... [ 0  2  0  0]
    // ...... [ 0  0  2  0]
    // ...... [-1 -1 -1  1]
    // issue: strange, but it's slower :(
    // issue: interpolators? dp4? VS limited? black magic?

    // Feed this transform to pixel shader
    O.M1 = xform[0];
    O.M2 = xform[1];
    O.M3 = xform[2];

#ifdef USE_TDETAIL
    O.tcdbump = O.tcdh * dt_params; // dt tc
#endif

    return O;
}
FXVS;
