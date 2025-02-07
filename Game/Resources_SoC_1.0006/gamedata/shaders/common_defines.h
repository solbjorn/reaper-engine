#ifndef common_defines_h_included
#define common_defines_h_included

// Shader style definitions

// Enhanced Shaders color space transformations (sRGB <-> linear)
#define ES

// "Mip fog" from Enhanced Shaders when SSS height fog is disabled
#define MIP_FOG

// ACES color grading and tonemapping from Enhanced Shaders
// Depends on ES
#define ACES_GRADING

// Bloom aspect correction fix from Enhanced Shaders
#define BLOOM_FIX

// Shader overrides shipped with Atmospherics
// Depends on ES, BLOOM_FIX
// Excludes CGIM2
#define ATMOS

// Build-ish cartoon-ish tonemapping from CGIM2
// Depends on ES, ACES_GRADING
// Excludes BLOOM_FIX, ATMOS
// #define CGIM2

// Anomaly Shaders Look Better shadows algos
#define SLB

#ifdef SLB
#define SLB_SMUL(x) (x)
#else
#define SLB_SMUL(x) 1.f
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//
#define def_gloss float(2.f / 255.f)
#define def_aref float(200.f / 255.f)
#define def_dbumph float(0.333f)
#define def_virtualh float(0.05f) // 5cm
#define def_distort float(0.05f) // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr float(9.f) // hight luminance range float(3.h)
#define def_hdr_clip float(0.75f) //

#ifdef CGIM2
#define LUMINANCE_VECTOR float3(0.2126, 0.7152, 0.0722)
#elif defined(ES)
#define LUMINANCE_VECTOR float3(0.2125, 0.7154, 0.0721)
#else
#define LUMINANCE_VECTOR float3(0.3f, 0.38f, 0.22f)
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// #define SKY_WITH_DEPTH // sky renders with depth to avoid some problems with reflections
#define SKY_DEPTH float(10000.0)
#define SKY_EPS float(0.001)

#endif //	common_defines_h_included
