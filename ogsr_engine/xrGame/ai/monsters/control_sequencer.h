#pragma once

#include "control_combase.h"
#include "../../../Include\xrRender\Kinematics.h"

struct SAnimationSequencerData : public ControlCom::IComData
{
    xr_vector<MotionID> motions;
};

class CAnimationSequencer : public CControl_ComCustom<SAnimationSequencerData>
{
    RTTI_DECLARE_TYPEINFO(CAnimationSequencer, CControl_ComCustom<SAnimationSequencerData>);

private:
    u32 m_index{};

public:
    ~CAnimationSequencer() override = default;

    virtual void reset_data();
    virtual void on_capture();
    virtual void on_release();
    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;

    virtual bool check_start_conditions();

    virtual void activate();

private:
    void play_selected();
};
