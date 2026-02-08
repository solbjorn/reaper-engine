#ifndef __CAMERA_FE_H__
#define __CAMERA_FE_H__

#include "../xr_3da/camerabase.h"

class CCameraFirstEye : public CCameraBase
{
    RTTI_DECLARE_TYPEINFO(CCameraFirstEye, CCameraBase);

public:
    typedef CCameraBase inherited;

    explicit CCameraFirstEye(CObject* p, u32 flags = 0);
    ~CCameraFirstEye() override;

    virtual void Load(LPCSTR section);
    void Move(EGameActions cmd, f32 val = 0.0f, f32 factor = 1.0f) override;

    virtual void OnActivate(CCameraBase* old_cam);
    virtual void Update(Fvector& point, Fvector& noise_angle);

    float GetWorldYaw() const override { return -yaw; }
    float GetWorldPitch() const override { return pitch; }
};

#endif // __CAMERALOOK_H__
