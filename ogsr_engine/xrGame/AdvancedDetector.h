#pragma once

#include "CustomDetector.h"

class CUIArtefactDetectorAdv;

class CAdvancedDetector : public CCustomDetector
{
    RTTI_DECLARE_TYPEINFO(CAdvancedDetector, CCustomDetector);

private:
    typedef CCustomDetector inherited;

public:
    CAdvancedDetector();
    ~CAdvancedDetector() override = default;

    virtual void on_a_hud_attach() override;
    virtual void on_b_hud_detach() override;

protected:
    virtual void UpdateAf() override;
    virtual void CreateUI() override;
    CUIArtefactDetectorAdv& ui();
};
