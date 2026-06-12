#pragma once

#include "../state.h"
#include "state_data.h"

template <typename _Object>
class CStateMonsterHideFromPoint : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterHideFromPoint<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    SStateHideFromPoint data;

public:
    explicit CStateMonsterHideFromPoint(_Object* obj) : inherited{obj, &data} {}
    ~CStateMonsterHideFromPoint() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
};

#include "state_hide_from_point_inline.h"
