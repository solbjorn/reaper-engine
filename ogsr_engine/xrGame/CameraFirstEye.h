#ifndef __CAMERA_FE_H__
#define __CAMERA_FE_H__

#include "../xr_3da/camerabase.h"

class CCameraFirstEye : public CCameraBase
{
    RTTI_DECLARE_TYPEINFO(CCameraFirstEye, CCameraBase);

public:
    typedef CCameraBase inherited;

    CCameraFirstEye(CObject* p, u32 flags = 0);
    virtual ~CCameraFirstEye();

    virtual void Load(LPCSTR section);
    virtual void Move(int cmd, float val = 0, float factor = 1.0f);

    virtual void OnActivate(CCameraBase* old_cam);
    virtual void Update(Fvector& point, Fvector& noise_angle);

    virtual float GetWorldYaw() { return -yaw; };
    virtual float GetWorldPitch() { return pitch; };
};

#endif // __CAMERALOOK_H__
