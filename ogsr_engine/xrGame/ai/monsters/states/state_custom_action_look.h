#pragma once

#include "../state.h"
#include "state_data.h"

template <typename _Object>
class CStateMonsterCustomActionLook : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterCustomActionLook<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    SStateDataActionLook data;

public:
    explicit CStateMonsterCustomActionLook(_Object* obj);
    ~CStateMonsterCustomActionLook() override;

    void execute() override;
    [[nodiscard]] bool check_completion() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "state_custom_action_look_inline.h"
