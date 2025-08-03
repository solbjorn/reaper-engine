///////////////////////////////////////////////////////////////
// BlackDrops.h
// BlackDrops - черные капли
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CBlackDrops : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CBlackDrops, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CBlackDrops(void);
    virtual ~CBlackDrops(void);

    virtual void Load(LPCSTR section);

protected:
};
XR_SOL_BASE_CLASSES(CBlackDrops);
