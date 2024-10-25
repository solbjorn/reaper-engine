#pragma once

class CActor;
class CAnimatorCamLerpEffector;
class CPostprocessAnimatorLerp;

class CZoneEffector
{
private:
    f32 r_min_perc{};
    f32 r_max_perc{};
    f32 radius{1.0f};
    f32 m_factor{0.1f};

    CPostprocessAnimatorLerp* m_pp_effector{};
    CAnimatorCamLerpEffector* m_cam_effector{};
    shared_str m_pp_fname;
    shared_str m_cam_fname;

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
