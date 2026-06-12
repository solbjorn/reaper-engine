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

    void Load(gsl::czstring section) override;
    void Move(EGameActions cmd, f32 val = 0.0f, f32 factor = 1.0f) override;

    void OnActivate(CCameraBase* old_cam) override;
    void Update(Fvector3& point, Fvector3& noise_angle) override;

    [[nodiscard]] f32 GetWorldYaw() const override { return -yaw; }
    [[nodiscard]] f32 GetWorldPitch() const override { return pitch; }
};

#endif // __CAMERALOOK_H__
