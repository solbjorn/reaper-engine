#include "common.h"

struct vv
{
    float4 P : POSITION;
    float2 tc : TEXCOORD0;
    float4 c : COLOR0;
};

struct v2p
{
    float2 tc : TEXCOORD0;
    float4 c : COLOR0;

    //	Igor: for additional depth dest
    float4 tctexgen : TEXCOORD1;
    float fog : FOG; // Fog
    float4 hpos : SV_Position;
};

uniform float4x4 mVPTexgen;

v2p main(vv v)
{
    v2p o;

    o.hpos = mul(m_WVP, v.P); // xform, input in world coords
    o.tc = v.tc; // copy tc
    o.c = unpack_D3DCOLOR(v.c); // copy color

    //	Igor: for additional depth dest
    o.tctexgen = mul(mVPTexgen, v.P);
    o.tctexgen.z = o.hpos.z;
    o.fog = saturate(calc_fogging(v.P)); // // ForserX (Port SkyLoader fog fix): fog, input in world coords

    return o;
}
