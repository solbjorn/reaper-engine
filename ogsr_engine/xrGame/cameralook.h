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

    virtual void Load(LPCSTR section);
    void Move(EGameActions cmd, f32 val = 0.0f, f32 factor = 1.0f) override;

    virtual void OnActivate(CCameraBase* old_cam);
    void Update(Fvector& point, Fvector&) override;

    float GetWorldYaw() const override { return -yaw; }
    float GetWorldPitch() const override { return pitch; }
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

    virtual void OnActivate(CCameraBase* old_cam);
    void Update(Fvector& point, Fvector&) override;
    virtual void Load(LPCSTR section);
};
