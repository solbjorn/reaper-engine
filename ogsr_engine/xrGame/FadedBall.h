///////////////////////////////////////////////////////////////
// FadedBall.h
// FadedBall - артефакт блеклый шар
///////////////////////////////////////////////////////////////

#pragma once

#include "artifact.h"

class CFadedBall : public CArtefact
{
    RTTI_DECLARE_TYPEINFO(CFadedBall, CArtefact);

private:
    typedef CArtefact inherited;

public:
    CFadedBall();
    ~CFadedBall() override;

    void Load(gsl::czstring section) override;
};
XR_SOL_BASE_CLASSES(CFadedBall);
