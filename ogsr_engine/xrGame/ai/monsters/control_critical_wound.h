#pragma once

#include "control_combase.h"

struct SControlCriticalWoundData : public ControlCom::IComData
{
    LPCSTR animation;
};

class CControlCriticalWound : public CControl_ComCustom<SControlCriticalWoundData>
{
    RTTI_DECLARE_TYPEINFO(CControlCriticalWound, CControl_ComCustom<SControlCriticalWoundData>);

private:
    typedef CControl_ComCustom<SControlCriticalWoundData> inherited;

public:
    ~CControlCriticalWound() override = default;

    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;
    void activate() override;
    void on_release() override;
    [[nodiscard]] bool check_start_conditions() override;
};
