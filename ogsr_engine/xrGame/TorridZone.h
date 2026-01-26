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

    virtual void UpdateWorkload(u32 dt);
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;

    virtual bool IsVisibleForZones() { return true; }
    virtual void GoEnabledState();
    virtual void GoDisabledState();
};
