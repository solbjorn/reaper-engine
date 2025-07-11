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
    virtual void Move(int cmd, float val = 0, float factor = 1.0f);

    virtual void OnActivate(CCameraBase* old_cam);
    virtual void Update(Fvector& point, Fvector& noise_dangle);

    virtual float GetWorldYaw() { return -yaw; };
    virtual float GetWorldPitch() { return pitch; };
};

class CCameraLook2 : public CCameraLook
{
    RTTI_DECLARE_TYPEINFO(CCameraLook2, CCameraLook);

public:
    static Fvector m_cam_offset;

protected:
    CObject* m_locked_enemy;
    Fvector2 m_autoaim_inertion_yaw;
    Fvector2 m_autoaim_inertion_pitch;
    void UpdateAutoAim();

public:
    CCameraLook2(CObject* p, u32 flags = 0) : CCameraLook(p, flags) { m_locked_enemy = NULL; };
    virtual ~CCameraLook2() {}
    virtual void OnActivate(CCameraBase* old_cam);
    virtual void Update(Fvector& point, Fvector& noise_dangle);
    virtual void Load(LPCSTR section);
};
