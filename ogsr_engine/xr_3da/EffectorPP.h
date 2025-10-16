#pragma once

#include "CameraDefs.h"

struct SPPInfo;

// постпроцесс
class CEffectorPP : public SBaseEffector
{
    RTTI_DECLARE_TYPEINFO(CEffectorPP, SBaseEffector);

public:
    EEffectorPPType eType;
    bool bFreeOnRemove;
    bool bOverlap{true};

protected:
    float fLifeTime;

public:
    CEffectorPP() : eType{ppeNext}, bFreeOnRemove{true}, fLifeTime{0.0f} {}
    CEffectorPP(EEffectorPPType type, float lifeTime, bool free_on_remove = true);
    virtual ~CEffectorPP();

    [[nodiscard]] virtual BOOL Process(SPPInfo&);
    [[nodiscard]] virtual BOOL Valid() { return fLifeTime > 0.0f; }
    [[nodiscard]] EEffectorPPType Type() const { return eType; }
    [[nodiscard]] bool FreeOnRemove() const { return bFreeOnRemove; }
    void SetType(EEffectorPPType t) { eType = t; }
    virtual void Stop(float) { fLifeTime = 0.0f; }
};
