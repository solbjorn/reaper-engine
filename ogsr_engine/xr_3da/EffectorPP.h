#pragma once

#include "CameraDefs.h"

struct SPPInfo;

// постпроцесс
class CEffectorPP : public SBaseEffector
{
    RTTI_DECLARE_TYPEINFO(CEffectorPP, SBaseEffector);

public:
    EEffectorPPType eType{};
    bool bFreeOnRemove;

protected:
    float fLifeTime;

public:
    CEffectorPP(EEffectorPPType type, f32 lifeTime, bool free_on_remove = true);
    CEffectorPP() : bFreeOnRemove(true), fLifeTime(0.0f), bOverlap(true) {};
    virtual ~CEffectorPP();
    virtual BOOL Process(SPPInfo& PPInfo);
    virtual BOOL Valid() { return fLifeTime > 0.0f; }
    IC EEffectorPPType Type() const { return eType; }
    IC bool FreeOnRemove() const { return bFreeOnRemove; }
    IC void SetType(EEffectorPPType t) { eType = t; }
    virtual void Stop(float speed) { fLifeTime = 0.0f; };
    bool bOverlap;
};
