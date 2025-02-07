//=================================================================================================
// Color grading - Color Decision List
// ACEScc compatible CDL https://github.com/ampas/aces-dev/blob/518c27f577e99cdecfddf2ebcfaa53444b1f9343/documents/LaTeX/S-2014-003/appendixB.tex#L1
//=================================================================================================

float3 ASC_CDL(float3 x, float3 Slope, float3 Offset, float3 Power)
{
    x *= Slope;
    x += Offset;

    // ACEScc has negative values that must not be clamped
    // Use this to apply power to the negative range
    float3 IsNeg = sign(x);
    x = abs(x);
    x = pow(x, Power);
    x *= IsNeg;

    return x;
}

void Color_Grading(inout float3 aces)
{
    float3 x = aces;

    // ASC-CDL (SOP-S) style color grading
    float3 Slope = {1.000, 1.000, 1.000};
    float3 Offset = {0.000, 0.000, 0.000};
    float3 Power = {1.000, 1.000, 1.000};
    float3 Saturation = {1, 1, 1};

// load custom settings from another file
#include "ACES_settings.h"

#ifdef USE_LOG_GRADING
    // to ACEScc log space
    x = lin_to_ACEScc(x);
#endif

    // apply CDL color grading
    x = ASC_CDL(x, Slope, Offset, Power);

    // apply saturation
    float3 luma = dot(x, LUMINANCE_VECTOR);
    x = luma + Saturation * (x - luma);

#ifdef USE_LOG_GRADING
    // from ACEScc log space
    x = ACEScc_to_lin(x);
#endif

    aces = x;
}
