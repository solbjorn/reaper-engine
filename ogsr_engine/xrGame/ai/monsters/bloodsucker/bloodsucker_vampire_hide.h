#pragma once

#include "../state.h"

template <typename _Object>
class CStateBloodsuckerVampireHide : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateBloodsuckerVampireHide<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

public:
    explicit CStateBloodsuckerVampireHide(_Object* obj);
    ~CStateBloodsuckerVampireHide() override = default;

    virtual void reselect_state();
    virtual void setup_substates();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "bloodsucker_vampire_hide_inline.h"
