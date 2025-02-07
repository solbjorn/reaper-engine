//=================================================================================================
// Gamma Correct Tonemapping and Color Grading
#include "ACES.h"
//=================================================================================================

float3 tonemap_sRGB(float3 x, float w)
{
#ifdef USE_ACES
    // use ACES workflow for color grading and tonemapping
    x = ACES(x);
#else /* !USE_ACES */
    // convert into linear gamma space
    x = SRGBToLinear(x);

#ifdef ACES_GRADING
    // color grading
    ACES_LMT(x);

    // clamp negative values
    x = max(0.0, x);

    // Boost the contrast to match ACES RRT
    float Contrast_Boost = 1.42857;
    x = pow(x, Contrast_Boost) * 0.18 / pow(0.18, Contrast_Boost);
#endif /* ACES_GRADING */

    // reinhard tonemapping
    x = x / (x + 1);
    x /= w / (w + 1);

    // convert into sRGB gamma space
    x = LinearTosRGB(x);
#endif /* !USE_ACES */

    // return with saturate, everything should be in LDR sRGB
    return saturate(x);
}
