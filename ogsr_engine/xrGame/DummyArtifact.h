///////////////////////////////////////////////////////////////
// DummyArtifact.h
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#pragma once

#include "Artifact.h"

class CDummyArtefact final : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CDummyArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CDummyArtefact();
    ~CDummyArtefact() override;

    void Load(gsl::czstring section) override;
};
XR_SOL_BASE_CLASSES(CDummyArtefact);
