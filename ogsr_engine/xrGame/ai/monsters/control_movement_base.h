#pragma once

#include "control_combase.h"
#include "ai_monster_defs.h"

class CControlMovementBase : public CControl_ComBase
{
    RTTI_DECLARE_TYPEINFO(CControlMovementBase, CControl_ComBase);

private:
    using inherited = CControl_ComBase;
    using VELOCITY_MAP = xr_map<u32, SVelocityParam>;

    VELOCITY_MAP m_velocities;

    f32 m_velocity{};
    f32 m_accel{};

public:
    ~CControlMovementBase() override = default;

    virtual void load(LPCSTR section);

    virtual void reinit();
    virtual void update_frame();

    void load_velocity(LPCSTR section, LPCSTR line, u32 param);
    SVelocityParam& get_velocity(u32 velocity_id);

    void stop();
    void stop_accel();
    void set_velocity(float val, bool max_acc = false);
    void set_accel(float val) { m_accel = val; }

    // services
    float get_velocity_from_path();
};
