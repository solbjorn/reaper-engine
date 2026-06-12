#pragma once

#include "CameraDefs.h"

class CEffectorCam : public SBaseEffector
{
    RTTI_DECLARE_TYPEINFO(CEffectorCam, SBaseEffector);

    friend class CCameraManager;

protected:
    ECamEffectorType eType;
    float fLifeTime;
    bool bHudAffect{true};

public:
    CEffectorCam() : eType{cefDemo}, fLifeTime{0.0f} {}
    explicit CEffectorCam(ECamEffectorType type, float tm) : eType{type}, fLifeTime{tm} {}
    ~CEffectorCam() override = default;

    void SetType(ECamEffectorType type) { eType = type; }
    void SetHudAffect(bool val) { bHudAffect = val; }
    bool GetHudAffect() { return bHudAffect; }
    IC ECamEffectorType GetType() { return eType; }
    [[nodiscard]] virtual BOOL Valid() { return fLifeTime > 0.0f; }

    virtual tmc::task<bool> ProcessCam(SCamEffectorInfo&)
    {
        fLifeTime -= Device.fTimeDelta;
        co_return Valid();
    }

    virtual void ProcessIfInvalid(SCamEffectorInfo&) {}
    [[nodiscard]] virtual BOOL AllowProcessingIfInvalid() { return FALSE; }
    [[nodiscard]] virtual bool AbsolutePositioning() { return false; }
};
