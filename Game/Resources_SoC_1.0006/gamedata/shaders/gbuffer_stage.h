#ifndef GBUFFER_STAGE_H
#define GBUFFER_STAGE_H

// GBuffer targets struct
struct f_deffer
{
    float4 position : SV_Target0; // xy=encoded normal, z = pz, w = encoded(m-id,hemi)
    float4 C : SV_Target1; // r, g, b,  gloss

#ifdef EXTEND_F_DEFFER
    uint mask : SV_COVERAGE;
#endif
};

// Unpacked data struct
struct gbuffer_data
{
    float3 P; // position.( mtl or sun )
    float mtl; // material id
    float3 N; // normal
    float hemi; // AO
    float3 C;
    float gloss;
};

// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
float2 OctWrap(float2 v) { return (1.0 - abs(v.yx)) * (v.xy >= 0.0 ? 1.0 : -1.0); }

float2 gbuf_pack_normal(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}

float3 gbuf_unpack_normal(float2 f)
{
    f = f * 2.0 - 1.0;

    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += n.xy >= 0.0 ? -t : t;
    return normalize(n);
}

#define USABLE_BIT_1 uint(0x00002000)
#define USABLE_BIT_2 uint(0x00004000)
#define USABLE_BIT_3 uint(0x00008000)
#define USABLE_BIT_4 uint(0x00010000)
#define USABLE_BIT_5 uint(0x00020000)
#define USABLE_BIT_6 uint(0x00040000)
#define USABLE_BIT_7 uint(0x00080000)
#define USABLE_BIT_8 uint(0x00100000)
#define USABLE_BIT_9 uint(0x00200000)
#define USABLE_BIT_10 uint(0x00400000)
#define USABLE_BIT_11 uint(0x00800000) // At least two of those four bit flags must be mutually exclusive (i.e. all 4 bits must not be set together)
#define USABLE_BIT_12 uint(0x01000000) // This is because setting 0x47800000 sets all 5 FP16 exponent bits to 1 which means infinity
#define USABLE_BIT_13 uint(0x02000000) // This will be translated to a +/-MAX_FLOAT in the FP16 render target (0xFBFF/0x7BFF), overwriting the
#define USABLE_BIT_14 uint(0x04000000) // mantissa bits where other bit flags are stored.
#define USABLE_BIT_15 uint(0x80000000)
#define MUST_BE_SET uint(0x40000000) // This flag *must* be stored in the floating-point representation of the bit flag to store

float gbuf_pack_hemi_mtl(float hemi, float mtl)
{
    uint packed_mtl = uint((mtl / 1.333333333) * 31.0);
    //	Clamp hemi max value
    uint packed = (MUST_BE_SET + (uint(saturate(hemi) * 255.9) << 13) + ((packed_mtl & uint(31)) << 21));

    if ((packed & USABLE_BIT_13) == 0)
        packed |= USABLE_BIT_14;

    if (packed_mtl & uint(16))
        packed |= USABLE_BIT_15;

    return asfloat(packed);
}

float gbuf_unpack_hemi(float mtl_hemi) { return float((asuint(mtl_hemi) >> 13) & uint(255)) * (1.0 / 254.8); }

float gbuf_unpack_mtl(float mtl_hemi)
{
    uint packed = asuint(mtl_hemi);
    uint packed_hemi = ((packed >> 21) & uint(15)) + ((packed & USABLE_BIT_15) == 0 ? 0 : 16);
    return float(packed_hemi) * (1.0 / 31.0) * 1.333333333;
}

// Packing stage
f_deffer pack_gbuffer(float4 norm, float4 pos, float4 col, uint imask)
{
    f_deffer res;

    res.position = float4(gbuf_pack_normal(norm.xyz), pos.z, gbuf_pack_hemi_mtl(norm.w, pos.w));
    res.C = col;

#ifdef EXTEND_F_DEFFER
    res.mask = imask;
#endif

    return res;
}

f_deffer pack_gbuffer(float4 norm, float4 pos, float4 col) { return pack_gbuffer(norm, pos, col, 0); }

#ifdef EXTEND_F_DEFFER
#define EXTFMASK(mask) (mask)
#else
#define EXTFMASK(mask) 0
#endif

// Unpacking stage
gbuffer_data gbuffer_load_data(float2 tc : TEXCOORD, float2 pos2d, uint iSample)
{
    gbuffer_data gbd;

#ifndef USE_MSAA
    float4 P = s_position.Sample(smp_nofilter, tc);
    float4 C = s_diffuse.Sample(smp_nofilter, tc);
#else
    float4 P = s_position.Load(int3(pos2d, 0), iSample);
    float4 C = s_diffuse.Load(int3(pos2d, 0), iSample);
#endif

    // 3d view space pos reconstruction math
    gbd.P = float3(P.z * (pos2d * pos_decompression_params.zw - pos_decompression_params.xy), P.z);

    // reconstruct N
    gbd.N = gbuf_unpack_normal(P.xy);

    // reconstruct material
    gbd.mtl = gbuf_unpack_mtl(P.w);

    // reconstruct hemi
    gbd.hemi = gbuf_unpack_hemi(P.w);

    gbd.C = C.xyz;
    gbd.gloss = C.w;

    return gbd;
}

gbuffer_data gbuffer_load_data(float2 tc : TEXCOORD, float2 pos2d) { return gbuffer_load_data(tc, pos2d, 0); }

gbuffer_data gbuffer_load_data_offset(float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d)
{
    float2 delta = ((OffsetTC - tc) * screen_res.xy);

    return gbuffer_load_data(OffsetTC, pos2d + delta, 0);
}

gbuffer_data gbuffer_load_data_offset(float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d, uint iSample)
{
    float2 delta = ((OffsetTC - tc) * screen_res.xy);

    return gbuffer_load_data(OffsetTC, pos2d + delta, iSample);
}

#endif // GBUFFER_STAGE_H
