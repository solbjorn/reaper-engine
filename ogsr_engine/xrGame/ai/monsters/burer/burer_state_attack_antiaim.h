#pragma once

#include "../state.h"

template <typename Object>
class CStateBurerAntiAim : public CState<Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBurerAntiAim<Object>, CState<Object>);

private:
    typedef CState<Object> inherited;
    using inherited::object;

public:
    explicit CStateBurerAntiAim(Object* obj);
    ~CStateBurerAntiAim() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
    void critical_finalize() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_start_conditions() override;
    [[nodiscard]] bool check_completion() override;

private:
    TTime m_last_shield_started;
    TTime m_next_particle_allowed;
    float m_shield_start_anim_length_sec;
    bool m_started;
    bool m_allow_anti_aim;
};

#include "burer_state_attack_antiaim_inline.h"
