#pragma once

#include "mosquitobald.h"

class CObjectAnimator;

class CTorridZone : public CMosquitoBald
{
    RTTI_DECLARE_TYPEINFO(CTorridZone, CMosquitoBald);

private:
    typedef CCustomZone inherited;
    CObjectAnimator* m_animator;

public:
    CTorridZone();
    ~CTorridZone() override;

    void UpdateWorkload(u32 dt) override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;

    [[nodiscard]] bool IsVisibleForZones() override { return true; }
    void GoEnabledState() override;
    void GoDisabledState() override;
};
