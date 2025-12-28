#pragma once

template <typename _Object>
class CScanningAbility : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScanningAbility<_Object>);

private:
    _Object* object{};

    // external members
    f32 critical_value{};
    f32 scan_radius{};
    f32 velocity_threshold{};
    f32 decrease_value{};
    f32 scan_trace_time_freq{};

    ref_sound sound_scan;

    // internal members
    enum EScanState
    {
        eStateDisabled,
        eStateNotActive,
        eStateScanning
    } state{};

    f32 scan_value{};

    SPPInfo m_effector_info;
    f32 m_effector_time{};
    f32 m_effector_time_attack{};
    f32 m_effector_time_release{};

    u32 time_last_trace{};

    bool m_this_scan{};

public:
    ~CScanningAbility() override = default;

    void init_external(_Object* obj) { object = obj; }
    void on_destroy();

    void load(LPCSTR section);
    virtual void reinit();

    void schedule_update();
    void frame_update(u32 dt);

    void enable();
    void disable();

    virtual void on_scan_success() {}
    virtual void on_scanning() {}

private:
    float get_velocity(CObject* obj);

#ifdef DEBUG
public:
    float get_scan_value() { return scan_value; }
#endif
};

#include "scanning_ability_inline.h"
