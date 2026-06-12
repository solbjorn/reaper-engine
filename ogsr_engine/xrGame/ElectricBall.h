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

    void Load(gsl::czstring section) override;

protected:
    void UpdateCLChild() override;
};
XR_SOL_BASE_CLASSES(CElectricBall);
