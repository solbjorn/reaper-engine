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

    void on_a_hud_attach() override;
    void on_b_hud_detach() override;

protected:
    void UpdateAf() override;
    void CreateUI() override;
    CUIArtefactDetectorAdv& ui();
};
