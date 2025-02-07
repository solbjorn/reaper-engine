//=================================================================================================
// Defines for and variables for ACES and color grading
//=================================================================================================
#define USE_LOG_GRADING // Use log space for color grading
//=================================================================================================

//
// manual settings
//
/*
    Slope = float3(1.000, 1.000, 1.000);
    Offset = float3(0.000, 0.000, 0.000);
    Power = float3(1.000, 1.000, 1.000);
    Saturation = 1.000;
*/
//
// settings for supporting in-game console commands
//
Slope = color_slope.rgb + color_slope.w; // brightness
Offset = color_offset.rgb + color_offset.w; // color grading
Power = color_power.rgb + color_power.w; // color grading
Saturation = color_saturation.rgb + color_saturation.w; // saturation
