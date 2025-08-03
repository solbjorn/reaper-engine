///////////////////////////////////////////////////////////////
// ThornArtifact.h
// ThornArtefact - артефакт колючка
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CThornArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CThornArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CThornArtefact(void);
    virtual ~CThornArtefact(void);

    virtual void Load(LPCSTR section);
};
XR_SOL_BASE_CLASSES(CThornArtefact);
