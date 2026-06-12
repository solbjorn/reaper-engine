#pragma once

#include "..\xr_3da\CameraBase.h"

class CCameraLook : public CCameraBase
{
    RTTI_DECLARE_TYPEINFO(CCameraLook, CCameraBase);

public:
    typedef CCameraBase inherited;

    Fvector2 lim_zoom;
    float dist, prev_d;

    explicit CCameraLook(CObject* p, u32 flags = 0);
    ~CCameraLook() override;

    void Load(gsl::czstring section) override;
    void Move(EGameActions cmd, f32 val = 0.0f, f32 factor = 1.0f) override;

    void OnActivate(CCameraBase* old_cam) override;
    void Update(Fvector3& point, Fvector3&) override;

    [[nodiscard]] f32 GetWorldYaw() const override { return -yaw; }
    [[nodiscard]] f32 GetWorldPitch() const override { return pitch; }
};

class CCameraLook2 : public CCameraLook
{
    RTTI_DECLARE_TYPEINFO(CCameraLook2, CCameraLook);

public:
    static Fvector m_cam_offset;

protected:
    CObject* m_locked_enemy{};
    Fvector2 m_autoaim_inertion_yaw;
    Fvector2 m_autoaim_inertion_pitch;

    void UpdateAutoAim();

public:
    explicit CCameraLook2(CObject* p, u32 flags = 0) : CCameraLook{p, flags} {}
    ~CCameraLook2() override = default;

    void OnActivate(CCameraBase* old_cam) override;
    void Update(Fvector3& point, Fvector3&) override;
    void Load(gsl::czstring section) override;
};
