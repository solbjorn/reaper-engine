#pragma once

#include "control_combase.h"

class CControlRunAttack : public CControl_ComCustom<>
{
    RTTI_DECLARE_TYPEINFO(CControlRunAttack, CControl_ComCustom<>);

private:
    f32 m_min_dist{};
    f32 m_max_dist{};

    u32 m_min_delay{};
    u32 m_max_delay{};

    u32 m_time_next_attack{};

public:
    ~CControlRunAttack() override = default;

    void load(gsl::czstring section) override;
    void reinit() override;

    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;
    void activate() override;
    void on_release() override;
    [[nodiscard]] bool check_start_conditions() override;
};
