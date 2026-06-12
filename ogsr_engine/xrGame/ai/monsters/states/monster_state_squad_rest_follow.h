#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterSquadRestFollow : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterSquadRestFollow<_Object>, CState<_Object>);

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

    Fvector last_point;

public:
    explicit CStateMonsterSquadRestFollow(_Object* obj);
    ~CStateMonsterSquadRestFollow() override;

    void initialize() override;
    void reselect_state() override;
    void setup_substates() override;
    void check_force_state() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_squad_rest_follow_inline.h"
