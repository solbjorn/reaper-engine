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
    CThornArtefact();
    ~CThornArtefact() override;

    void Load(gsl::czstring section) override;
};
XR_SOL_BASE_CLASSES(CThornArtefact);
