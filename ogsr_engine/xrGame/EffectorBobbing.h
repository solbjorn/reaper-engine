#pragma once

#include "CameraEffector.h"
#include "../xr_3da/cameramanager.h"

class CEffectorBobbing : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CEffectorBobbing, CEffectorCam);

public:
    float fTime;
    u32 dwMState{};
    float fReminderFactor;
    bool is_limping;
    bool m_bZoomMode{};

    float m_fAmplitudeRun;
    float m_fAmplitudeWalk;
    float m_fAmplitudeLimp;

    float m_fSpeedRun;
    float m_fSpeedWalk;
    float m_fSpeedLimp;

    CEffectorBobbing();
    ~CEffectorBobbing() override;

    virtual BOOL ProcessCam(SCamEffectorInfo& info);
    void SetState(u32 st, bool limping, bool ZoomMode);
};
