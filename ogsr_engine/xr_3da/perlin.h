#ifndef perlinH
#define perlinH

#define SAMPLE_SIZE 256

class CPerlinNoiseCustom
{
protected:
    s32 p[SAMPLE_SIZE + SAMPLE_SIZE + 2];

    s32 mOctaves{2};
    f32 mFrequency{1.0f};
    f32 mAmplitude{1.0f};

    xr_vector<f32> mTimes;

public:
    CPerlinNoiseCustom() { mTimes.resize(mOctaves); }

    void SetParams(s32 oct, f32 freq, f32 amp)
    {
        SetOctaves(oct);
        SetFrequency(freq);
        SetAmplitude(amp);
    }

    void SetOctaves(s32 oct)
    {
        mOctaves = oct;
        mTimes.resize(mOctaves);
    }

    void SetFrequency(f32 freq) { mFrequency = freq; }
    void SetAmplitude(f32 amp) { mAmplitude = amp; }
};

class CPerlinNoise1D : public CPerlinNoiseCustom
{
private:
    f32 g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    f32 mPrevContiniousTime{};

    [[nodiscard]] f32 noise(f32 arg);

public:
    CPerlinNoise1D();

    [[nodiscard]] f32 Get(f32 x);
    [[nodiscard]] f32 GetContinious(f32 v);
};

#endif
