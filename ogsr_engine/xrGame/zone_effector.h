#pragma once

class CActor;
class CPostprocessAnimatorLerp;

class CZoneEffector
{
    float r_min_perc{};
    float r_max_perc{};
    float radius{1.f};
    float m_factor{0.1f};
    CPostprocessAnimatorLerp* m_pp_effector{};
    shared_str m_pp_fname;

public:
    CActor* m_pActor{};

    CZoneEffector();
    ~CZoneEffector();

    void Load(LPCSTR section);
    void SetRadius(float r);
    void Update(float dist);
    void Stop();
    float GetFactor();

private:
    void Activate();
};
