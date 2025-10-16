#pragma once

#include "..\xr_3da\CameraBase.h"

class CCameraLook : public CCameraBase
{
    RTTI_DECLARE_TYPEINFO(CCameraLook, CCameraBase);

public:
    typedef CCameraBase inherited;

    Fvector2 lim_zoom;
    float dist, prev_d;

    CCameraLook(CObject* p, u32 flags = 0);
    virtual ~CCameraLook();

    virtual void Load(LPCSTR section);
    virtual void Move(int cmd, float val = 0.0f, float factor = 1.0f);

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
    CCameraLook2(CObject* p, u32 flags = 0) : CCameraLook{p, flags} {}
    virtual ~CCameraLook2() {}

    virtual void OnActivate(CCameraBase* old_cam);
    void Update(Fvector& point, Fvector&) override;
    virtual void Load(LPCSTR section);
};
