///////////////////////////////////////////////////////////////
// ZudaArtifact.h
// ZudaArtefact - артефакт зуда
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CZudaArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CZudaArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CZudaArtefact(void);
    virtual ~CZudaArtefact(void);

    virtual void Load(LPCSTR section);
};
XR_SOL_BASE_CLASSES(CZudaArtefact);
