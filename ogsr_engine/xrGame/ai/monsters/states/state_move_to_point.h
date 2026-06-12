#pragma once

#include "../state.h"
#include "state_data.h"

template <typename _Object>
class CStateMonsterMoveToPoint : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterMoveToPoint<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    SStateDataMoveToPoint data;

public:
    explicit CStateMonsterMoveToPoint(_Object* obj) : inherited{obj, &data} {}
    ~CStateMonsterMoveToPoint() override = default;

    void initialize() override;
    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }

    [[nodiscard]] bool check_completion() override;
};

template <typename _Object>
class CStateMonsterMoveToPointEx : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterMoveToPointEx<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

protected:
    SStateDataMoveToPointEx data;

public:
    explicit CStateMonsterMoveToPointEx(_Object* obj) : inherited{obj, &data} {}
    ~CStateMonsterMoveToPointEx() override = default;

    void initialize() override;
    void execute() override;
    [[nodiscard]] bool check_completion() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "state_move_to_point_inline.h"
