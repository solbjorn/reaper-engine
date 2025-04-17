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

    virtual void load(LPCSTR section);
    virtual void reinit();

    void on_event(ControlCom::EEventType type, ControlCom::IEventData*) override;
    virtual void activate();
    virtual void on_release();
    virtual bool check_start_conditions();
};
