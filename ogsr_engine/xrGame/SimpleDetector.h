#pragma once

#include "CustomDetector.h"

class CUIArtefactDetectorSimple;

class CSimpleDetector : public CCustomDetector
{
    RTTI_DECLARE_TYPEINFO(CSimpleDetector, CCustomDetector);

private:
    typedef CCustomDetector inherited;

public:
    CSimpleDetector();
    ~CSimpleDetector() override = default;

protected:
    void UpdateAf() override;
    void CreateUI() override;
    CUIArtefactDetectorSimple& ui();
};
