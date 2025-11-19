// EffectorShot.h: interface for the CCameraShotEffector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "../xr_3da/cameramanager.h"
#include "Actor.h"

class CWeaponShotEffector : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CWeaponShotEffector);

protected:
    float fAngleVert;
    float fAngleVertMax;
    float fAngleVertFrac;
    float fAngleHorz;
    float fAngleHorzMax;
    float fAngleHorzStep;
    float fRelaxSpeed;

    float fLastDeltaVert;
    float fLastDeltaHorz;

protected:
    BOOL bActive;
    BOOL bSingleShoot;
    BOOL bSSActive;

public:
    CWeaponShotEffector();
    ~CWeaponShotEffector() override = default;

    void Initialize(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
    IC BOOL IsActive() { return bActive; }
    virtual void SetActive(BOOL Active) { bActive = Active; }
    IC BOOL IsSingleShot() { return bSingleShoot; }
    virtual void SetSingleShoot(BOOL Single) { bSingleShoot = Single; }
    void Update();

    virtual void Shot(float angle);
    virtual void GetDeltaAngle(Fvector& delta_angle);
    virtual void GetLastDelta(Fvector& delta_angle);
    virtual void Clear();

    virtual void ApplyLastAngles(float* pitch, float* yaw);
    virtual void ApplyDeltaAngles(float* pitch, float* yaw);
};

class CCameraShotEffector : public CWeaponShotEffector, public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CCameraShotEffector, CWeaponShotEffector, CEffectorCam);

protected:
    CActor* m_pActor;

public:
    explicit CCameraShotEffector(float max_angle, float relax_speed, float max_angle_horz, float step_angle_horz, float angle_frac);
    ~CCameraShotEffector() override;

    virtual BOOL ProcessCam(SCamEffectorInfo& info);

    virtual void SetActor(CActor* pActor) { m_pActor = pActor; }

    virtual CCameraShotEffector* cast_effector_shot() { return this; }
};
