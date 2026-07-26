#ifndef DISABLING_PARAMS_H
#define DISABLING_PARAMS_H

struct SOneDDOParams final
{
    void Mul(float v);
    float velocity;
    float acceleration;
};

struct SAllDDOParams final
{
    void Reset();
    void Load(CInifile* ini);
    SOneDDOParams translational;
    SOneDDOParams rotational;
    u16 L2frames;
};

struct SAllDDWParams final
{
    SAllDDOParams objects_params;
    float reanable_factor;
};

extern SAllDDWParams worldDisablingParams;

#endif
