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
    CZudaArtefact();
    ~CZudaArtefact() override;

    void Load(gsl::czstring section) override;
};
XR_SOL_BASE_CLASSES(CZudaArtefact);
