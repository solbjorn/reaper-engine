#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterControlled : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterControlled<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

public:
    explicit CStateMonsterControlled(_Object* obj);
    ~CStateMonsterControlled() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_controlled_inline.h"
