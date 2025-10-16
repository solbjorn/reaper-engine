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
    CEffectorCam(ECamEffectorType type, float tm) : eType{type}, fLifeTime{tm} {}
    virtual ~CEffectorCam() {}

    void SetType(ECamEffectorType type) { eType = type; }
    void SetHudAffect(bool val) { bHudAffect = val; }
    bool GetHudAffect() { return bHudAffect; }
    IC ECamEffectorType GetType() { return eType; }
    virtual BOOL Valid() { return fLifeTime > 0.0f; }

    virtual BOOL ProcessCam(SCamEffectorInfo&)
    {
        fLifeTime -= Device.fTimeDelta;
        return Valid();
    }

    virtual void ProcessIfInvalid(SCamEffectorInfo&) {}
    virtual BOOL AllowProcessingIfInvalid() { return FALSE; }
    virtual bool AbsolutePositioning() { return false; }
};
