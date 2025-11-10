#pragma once

struct XR_TRIVIAL Flight
{
public:
    u32 type; /* Type of light source */
    Fcolor diffuse; /* Diffuse color of light */
    Fcolor specular; /* Specular color of light */
    Fcolor ambient; /* Ambient color of light */
    Fvector position; /* Position in world space */
    Fvector direction; /* Direction in world space */
    float range; /* Cutoff range */
    float falloff; /* Falloff */
    float attenuation0; /* Constant attenuation */
    float attenuation1; /* Linear attenuation */
    float attenuation2; /* Quadratic attenuation */
    float theta; /* Inner angle of spotlight cone */
    float phi; /* Outer angle of spotlight cone */

    constexpr void set(u32 ltType, f32 x, f32 y, f32 z)
    {
#ifdef XR_TRIVIAL_BROKEN
        XR_DIAG_PUSH();
        XR_DIAG_IGNORE("-Wnontrivial-memcall");
#endif

        std::memset(this, 0, sizeof(Flight));

#ifdef XR_TRIVIAL_BROKEN
        XR_DIAG_POP();
#endif

        type = ltType;
        diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
        specular.set(diffuse);
        position.set(x, y, z);
        direction.set(x, y, z);
        direction.normalize_safe();
        range = _sqrt(flt_max);
    }

    constexpr void mul(f32 brightness)
    {
        diffuse.mul_rgb(brightness);
        ambient.mul_rgb(brightness);
        specular.mul_rgb(brightness);
    }
};
XR_TRIVIAL_ASSERT(Flight);
