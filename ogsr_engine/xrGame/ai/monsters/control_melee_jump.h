#pragma once

#include "control_combase.h"
#include "../../../Include\xrRender\Kinematics.h"

struct SControlMeleeJumpData : public ControlCom::IComData
{
    MotionID anim_ls;
    MotionID anim_rs;
};

class CControlMeleeJump : public CControl_ComCustom<SControlMeleeJumpData>
{
    RTTI_DECLARE_TYPEINFO(CControlMeleeJump, CControl_ComCustom<SControlMeleeJumpData>);

private:
    using inherited = CControl_ComCustom<SControlMeleeJumpData>;

    u32 m_time_next_melee_jump{};

public:
    ~CControlMeleeJump() override = default;

    void reinit() override;

    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;
    void activate() override;
    void on_release() override;
    [[nodiscard]] bool check_start_conditions() override;
};
