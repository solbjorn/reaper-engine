#ifndef noiseH
#define noiseH

[[nodiscard]] float fractalsum3(const Fvector& v, float freq, int octaves);
[[nodiscard]] float turbulence3(const Fvector& v, float freq, int octaves);

#endif
