//=================================================================================================
// LMT - Channel Mixer
// For complex color effects
//=================================================================================================

void Channel_Mixer(inout float3 aces)
{
    float3 Channel_Mixer_r = color_red.rgb + color_red.w;
    float3 Channel_Mixer_g = color_green.rgb + color_green.w;
    float3 Channel_Mixer_b = color_blue.rgb + color_blue.w;

    aces = float3(dot(aces, Channel_Mixer_r), dot(aces, Channel_Mixer_g), dot(aces, Channel_Mixer_b));
    aces = max(0.0, aces);
}
