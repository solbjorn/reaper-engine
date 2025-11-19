#pragma once

#include "../state.h"

template <typename _Object>
class CStateGroupPanic : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupPanic<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

public:
    explicit CStateGroupPanic(_Object* obj);
    ~CStateGroupPanic() override;

    virtual void initialize();
    virtual void reselect_state();
    virtual void check_force_state();
    virtual void setup_substates();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "group_state_panic_inline.h"
