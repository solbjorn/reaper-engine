///////////////////////////////////////////////////////////////
// GraviArtifact.h
// GraviArtefact - гравитационный артефакт, прыгает на месте
// и парит над землей
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CGraviArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CGraviArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CGraviArtefact();
    ~CGraviArtefact() override;

    void Load(gsl::czstring section) override;

protected:
    void UpdateCLChild() override;

    // параметры артефакта
    float m_fJumpHeight;
    float m_fEnergy;
};
XR_SOL_BASE_CLASSES(CGraviArtefact);
