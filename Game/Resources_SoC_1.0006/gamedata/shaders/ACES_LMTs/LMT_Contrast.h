//=================================================================================================
// LMT - Contrast in linear space
// Change constrast per RGB channel
//=================================================================================================

void Contrast(inout float3 aces)
{
    float3 Contrast_Amount = color_contrast.rgb + color_contrast.w;

    // aces = (aces - ACEScc_MIDGRAY) * Contrast_Amount + ACEScc_MIDGRAY; //log contrast
    aces = pow(aces, Contrast_Amount) * 0.18 / pow(0.18, Contrast_Amount);
}
