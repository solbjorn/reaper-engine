#pragma once

#include "../state.h"

template <typename _Object>
class CStateGroupHearDangerousSound : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupHearDangerousSound<_Object>, CState<_Object>);

protected:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    u32 m_target_node{};

public:
    explicit CStateGroupHearDangerousSound(_Object* obj);
    ~CStateGroupHearDangerousSound() override = default;

    void initialize() override;
    void reselect_state() override;
    void setup_substates() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "group_state_hear_danger_sound_inline.h"
