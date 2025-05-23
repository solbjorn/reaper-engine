#ifndef common_cbuffers_h_included
#define common_cbuffers_h_included

// Used by dynamic lights and volumetric effects
cbuffer dynamic_light
{
    uniform float4 Ldynamic_color; // dynamic light color (rgb1) - spot/point/sun
    uniform float4 Ldynamic_pos; // dynamic light pos+1/range(w) - spot/point
    uniform float4 Ldynamic_dir; // dynamic light direction - sun
}

#endif // common_cbuffers_h_included
