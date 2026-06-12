#pragma once

#include "control_combase.h"

struct SControlThreatenData : public ControlCom::IComData
{
    LPCSTR animation;
    float time;
};

class CControlThreaten : public CControl_ComCustom<SControlThreatenData>
{
    RTTI_DECLARE_TYPEINFO(CControlThreaten, CControl_ComCustom<SControlThreatenData>);

private:
    typedef CControl_ComCustom<SControlThreatenData> inherited;

public:
    ~CControlThreaten() override = default;

    void reinit() override;
    void update_schedule() override;
    void on_event(ControlCom::EEventType type, ControlCom::IEventData* data) override;
    void activate() override;
    void on_release() override;
    [[nodiscard]] bool check_start_conditions() override;
};
