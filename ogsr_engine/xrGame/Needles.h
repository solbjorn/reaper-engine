///////////////////////////////////////////////////////////////
// Needles.h
// Needles - артефакт иголки
///////////////////////////////////////////////////////////////

#pragma once

#include "Artifact.h"

class CNeedles : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CNeedles, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CNeedles();
    ~CNeedles() override;
};
