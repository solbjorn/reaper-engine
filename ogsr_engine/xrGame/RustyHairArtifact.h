///////////////////////////////////////////////////////////////
// RustyHairArtifact.h
// RustyHairArtefact - артефакт ржавые волосы
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CRustyHairArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CRustyHairArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CRustyHairArtefact();
    ~CRustyHairArtefact() override;

    virtual void Load(LPCSTR section);
};
XR_SOL_BASE_CLASSES(CRustyHairArtefact);
