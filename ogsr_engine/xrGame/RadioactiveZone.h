#pragma once

#include "CustomZone.h"

class CRadioactiveZone : public CCustomZone
{
    RTTI_DECLARE_TYPEINFO(CRadioactiveZone, CCustomZone);

private:
    typedef CCustomZone inherited;

public:
    CRadioactiveZone();
    ~CRadioactiveZone() override;

    void Load(gsl::czstring section) override;
    void Affect(SZoneObjectInfo* O) override;

    void feel_touch_new(CObject* O) override;
    void UpdateWorkload(u32 dt) override; // related to fast-mode optimizations
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;

protected:
    [[nodiscard]] bool BlowoutState() override;
};
