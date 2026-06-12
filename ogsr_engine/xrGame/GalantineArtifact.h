///////////////////////////////////////////////////////////////
// GalantineArtifact.h
// GalantineArtefact - артефакт ведбмин студень
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CGalantineArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CGalantineArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CGalantineArtefact();
    ~CGalantineArtefact() override;

    void Load(gsl::czstring section) override;
};
XR_SOL_BASE_CLASSES(CGalantineArtefact);
