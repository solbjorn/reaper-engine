#pragma once

#include "control_combase.h"

struct SControlMovementData : public ControlCom::IComData
{
    float velocity_target;
    float acc;
};

class CControlMovement : public CControl_ComPure<SControlMovementData>
{
    RTTI_DECLARE_TYPEINFO(CControlMovement, CControl_ComPure<SControlMovementData>);

public:
    typedef CControl_ComPure<SControlMovementData> inherited;

    float m_velocity_current;

    ~CControlMovement() override = default;

    virtual void reinit();
    virtual void update_frame();

    float velocity_current() { return m_velocity_current; }
    float velocity_target() { return m_data.velocity_target; }

    // return object's real velocity
    float real_velocity();
};
