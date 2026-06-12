#pragma once

#include "../state.h"
#include "state_data.h"
#include "../../../ai_object_location.h"
#include "../../../ai_space.h"
#include "level_graph.h"

template <typename _Object>
class CStateMonsterLookToUnprotectedArea : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterLookToUnprotectedArea<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    SStateDataAction data;

    Fvector target_point;

public:
    explicit CStateMonsterLookToUnprotectedArea(_Object* obj);
    ~CStateMonsterLookToUnprotectedArea() override;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
};

#include "state_look_unprotected_area_inline.h"
