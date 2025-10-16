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
    virtual void reinit();
    virtual void update_schedule();
    void on_event(ControlCom::EEventType type, ControlCom::IEventData* data) override;
    virtual void activate();
    virtual void on_release();
    virtual bool check_start_conditions();
};
