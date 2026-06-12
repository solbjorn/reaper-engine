#pragma once

#include "../state.h"

template <typename _Object>
class CStateCustomGroup : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateCustomGroup<_Object>, CState<_Object>);

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

public:
    explicit CStateCustomGroup(_Object* obj);
    ~CStateCustomGroup() override;

    void execute() override;
    void setup_substates() override;
    [[nodiscard]] bool check_completion() override { return object->b_state_end; }
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "group_state_custom_inline.h"
