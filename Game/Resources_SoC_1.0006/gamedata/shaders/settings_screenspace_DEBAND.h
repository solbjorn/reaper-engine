// [ SETTINGS ] [ SKY DEBANDING ]

#ifdef ATMOS
#define G_DEBANDING_QUALITY 3 // Deband iterations [ 1 = Low, 2 = Normal, 3 = High, 4 = Ultra ]
#define G_DEBANDING_RADIUS 50 // Max pixels to search for banding. Higher values improve smoothness, but also add more noise in some circumstances.
#else
#define G_DEBANDING_QUALITY 2 // Deband iterations [ 1 = Low, 2 = Normal, 3 = High, 4 = Ultra ]
#define G_DEBANDING_RADIUS 48 // Max pixels to search for banding. Higher values improve smoothness, but also add more noise in some circumstances.
#endif
