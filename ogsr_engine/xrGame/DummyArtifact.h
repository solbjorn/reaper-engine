///////////////////////////////////////////////////////////////
// DummyArtifact.h
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#pragma once
#include "artifact.h"

class CDummyArtefact : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CDummyArtefact, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CDummyArtefact(void);
    virtual ~CDummyArtefact(void);

    virtual void Load(LPCSTR section);
};
XR_SOL_BASE_CLASSES(CDummyArtefact);
