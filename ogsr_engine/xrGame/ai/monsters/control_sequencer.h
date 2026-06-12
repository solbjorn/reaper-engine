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

    void reset_data() override;
    void on_capture() override;
    void on_release() override;
    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;

    [[nodiscard]] bool check_start_conditions() override;

    void activate() override;

private:
    void play_selected();
};
