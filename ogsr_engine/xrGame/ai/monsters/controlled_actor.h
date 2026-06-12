#pragma once

#include "../../actor_input_handler.h"

class CControlledActor : public CActorInputHandler
{
    RTTI_DECLARE_TYPEINFO(CControlledActor, CActorInputHandler);

public:
    using inherited = CActorInputHandler;

    Fvector m_target_point{};

    bool m_turned_yaw{};
    bool m_turned_pitch{};
    bool m_need_turn{};

    bool m_lock_run{};
    u32 m_lock_run_started{};
    u32 m_lock_run_period{};

    ~CControlledActor() override = default;

    void reinit() override;
    [[nodiscard]] f32 mouse_scale_factor() override { return flt_max; }
    void release() override;
    void install(CActor*) override;
    void install() override;
    [[nodiscard]] bool authorized(EGameActions cmd) override;

    void look_point(const Fvector& point);
    bool is_turning();
    bool is_installed();

    void frame_update();
    bool is_controlling() { return !!m_actor; }

    void dont_need_turn() { m_need_turn = false; }

private:
    void reset();
    void update_turn();
};
