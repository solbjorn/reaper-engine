///////////////////////////////////////////////////////////////
// ElectricBall.h
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CElectricBall : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CElectricBall, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CElectricBall();
    ~CElectricBall() override;

    virtual void Load(LPCSTR section);

protected:
    virtual void UpdateCLChild();
};
XR_SOL_BASE_CLASSES(CElectricBall);
