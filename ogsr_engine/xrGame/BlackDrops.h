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
    CBlackDrops();
    ~CBlackDrops() override;

    virtual void Load(LPCSTR section);
};
XR_SOL_BASE_CLASSES(CBlackDrops);
