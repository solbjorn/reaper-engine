#include "common.h"
#include "shared\cloudconfig.h"

struct vi
{
    float4 p : POSITION;
    float4 dir : COLOR0; // dir0,dir1(w<->z)
    float4 color : COLOR1; // rgb. intensity
};

struct vf
{
    float4 color : COLOR0; // rgb. intensity, for SM3 - tonemap-prescaled, HI-res
    float3 tc0 : TEXCOORD0;
    float2 tc1 : TEXCOORD1;
    float4 hpos : SV_Position;
};

vf main(vi v)
{
    vf o;

    o.hpos = mul(m_WVP, v.p); // xform, input in world coords

    // generate tcs
    float2 d0 = v.dir.xy * 2 - 1;
    float2 d1 = v.dir.wz * 2 - 1;
    float2 _0 = v.p.xz * CLOUD_TILE0 + d0 * timers.z * CLOUD_SPEED0;
    float2 _1 = v.p.xz * CLOUD_TILE1 + d1 * timers.z * CLOUD_SPEED1;

    float scale = s_tonemap.Load(int3(0, 0, 0)).x;
    o.tc0 = float3(_0, scale); // copy tc
    o.tc1 = _1; // copy tc

    o.color = v.color; // copy color, low precision, cannot prescale even by 2
    o.color.w *= pow(v.p.y, 25);

    return o;
}
