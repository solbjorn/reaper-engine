#ifndef IMG_CORRECTIONS_H
#define IMG_CORRECTIONS_H
#include "common.h"

float3 img_corrections(float3 img)
{
#ifndef ACES_GRADING
    // exposure
    img.xyz *= color_slope.rgb + color_slope.w;

    // color grading (thanks KD and Crytek and Cjayho)
    float fLum = dot(img.xyz, LUMINANCE_VECTOR);
#ifdef ATMOS
    fLum *= 2;
#endif

    float3 cColor = lerp(0.0, 1.f - (color_power.rgb + color_power.w) / 2.f, saturate(fLum * 2.0));
    cColor = lerp(cColor, 1.0, saturate(fLum - 0.5) * 2.0);
    img.xyz = saturate(lerp(img.xyz, cColor.xyz, saturate(fLum * 0.15)));

    // saturation
    img.xyz = saturate(lerp(img.xyz, dot(img.xyz, LUMINANCE_VECTOR), 1.f - (color_saturation.rgb + color_saturation.w)));
#endif /* !ACES_GRADING */

    // gamma correction
    img.xyz = pow(img, 1.f / (color_gamma.rgb + color_gamma.w));

    // that's all :)
    return img.xyz;
}
#endif
