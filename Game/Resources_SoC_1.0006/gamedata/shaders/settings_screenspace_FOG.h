// [ SETTINGS ] [ FOG ]

#ifdef ATMOS
#define G_FOG_HEIGHT 8.15f // Height of the ground fog.
#define G_FOG_HEIGHT_INTENSITY 1.05f // Intensity of the ground fog.
#define G_FOG_HEIGHT_DENSITY 1.35f // Density of the ground fog.
#else
#define G_FOG_HEIGHT 8.0f // Height of the ground fog.
#define G_FOG_HEIGHT_INTENSITY 1.0f // Intensity of the ground fog.
#define G_FOG_HEIGHT_DENSITY 1.3f // Density of the ground fog.
#endif

#define G_FOG_SUNCOLOR_INTENSITY 0.10f // Intensity of sun color in the fog. [ 0.0f = 0% | 1.0f = 100% ]

#define G_FOG_USE_SCATTERING // Enable/Disable fog scattering
