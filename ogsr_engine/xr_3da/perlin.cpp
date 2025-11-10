/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken CPerlinNoise) */

#include "stdafx.h"

#include "perlin.h"

#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE - 1)

#define N 0x1000

#define s_curve(t) (t * t * (3.0f - 2.0f * t))
#define lerp(t, a, b) (a + t * (b - a))

#define setup(i, b0, b1, r0, r1) \
    t = vec[i] + N; \
    b0 = ((s32)t) & BM; \
    b1 = (b0 + 1) & BM; \
    r0 = t - (s32)t; \
    r1 = r0 - 1.0f

//-------------------------------------------------------------------------------------------------
// CPerlinNoise1D
//-------------------------------------------------------------------------------------------------

CPerlinNoise1D::CPerlinNoise1D()
{
    s32 i, j, k;

    for (i = 0; i < B; i++)
    {
        p[i] = i;
        g1[i] = (f32)((rand() % (B + B)) - B) / B;
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = rand() % B];
        p[j] = k;
    }

    for (i = 0; i < B + 2; i++)
    {
        p[B + i] = p[i];
        g1[B + i] = g1[i];
    }
}

f32 CPerlinNoise1D::noise(f32 arg)
{
    s32 bx0, bx1;
    f32 rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;
    setup(0, bx0, bx1, rx0, rx1);

    sx = s_curve(rx0);

    u = rx0 * g1[p[bx0]];
    v = rx1 * g1[p[bx1]];

    return lerp(sx, u, v);
}

f32 CPerlinNoise1D::Get(f32 v)
{
    f32 result = 0.0f;
    f32 amp = mAmplitude;
    v *= mFrequency;

    for (s32 i = 0; i < mOctaves; i++)
    {
        result += noise(v) * amp;
        v *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}

f32 CPerlinNoise1D::GetContinious(f32 v)
{
    f32 t_v = v;

    if (!fis_zero(mPrevContiniousTime))
        v -= mPrevContiniousTime;

    mPrevContiniousTime = t_v;
    f32 result = 0.0f;
    f32 amp = mAmplitude;
    v *= mFrequency;

    for (s32 i = 0; i < mOctaves; i++)
    {
        f32 octave_time = mTimes[i];
        mTimes[i] = octave_time + v;
        result += noise(octave_time + v) * amp;
        v *= 2.0f;
        amp *= 0.5f;
    }

    return result;
}
