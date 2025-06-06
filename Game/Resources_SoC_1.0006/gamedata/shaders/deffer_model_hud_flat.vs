#include "common.h"
#include "skin.h"

v2p_flat _main(v_model I, float3 psp)
{
    // world-space  N
    float3 N_w = mul(m_W, I.N);

    // Eye-space pos/normal
    v2p_flat O;
    float3 Pe = mul(m_WV, I.P);
    O.hpos = mul(m_WVP, I.P);
    O.N = mul((float3x3)m_WV, (float3)I.N);

    O.tcdh = float4(I.tc.xyyy);

    //  Hemi cube lighting
    float3 Nw = mul((float3x3)m_W, (float3)I.N);
    float3 hc_pos = (float3)hemi_cube_pos_faces;
    float3 hc_neg = (float3)hemi_cube_neg_faces;
    float3 hc_mixed = (Nw < 0) ? hc_neg : hc_pos;
    float hemi_val = dot(hc_mixed, abs(Nw));
    hemi_val = saturate(hemi_val);

    O.position = float4(Pe, hemi_val); // Use L_material.x for old behaviour;

#ifdef USE_TDETAIL
    O.tcdbump = O.tcdh * dt_params; // dt tc
#endif

    // HUD Rain drops - SSS Update 17
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
    if (!all(psp))
        O.RDrops.xyz = I.P.xyz;
    else
        O.RDrops.xyz = psp;

    O.RDrops.w = Nw.y; // Normal-Y [ World Space ]

    return O;
}

/////////////////////////////////////////////////////////////////////////
#ifdef SKIN_NONE
v2p_flat main(v_model v) { return _main(v, 0); }
#endif

#ifdef SKIN_0
v2p_flat main(v_model_skinned_0 v) { return _main(skinning_0(v), v.P); }
#endif

#ifdef SKIN_1
v2p_flat main(v_model_skinned_1 v) { return _main(skinning_1(v), v.P); }
#endif

#ifdef SKIN_2
v2p_flat main(v_model_skinned_2 v) { return _main(skinning_2(v), v.P); }
#endif

#ifdef SKIN_3
v2p_flat main(v_model_skinned_3 v) { return _main(skinning_3(v), v.P); }
#endif

#ifdef SKIN_4
v2p_flat main(v_model_skinned_4 v) { return _main(skinning_4(v), v.P); }
#endif

FXVS;
