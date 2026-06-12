#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterTestState : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterTestState<_Object>, CState<_Object>);

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
    explicit CStateMonsterTestState(_Object* obj);
    ~CStateMonsterTestState() override = default;

    void reselect_state() override;
    void setup_substates() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

template <typename _Object>
class CStateMonsterTestCover : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterTestCover<_Object>, CState<_Object>);

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

    u32 m_last_node{};

public:
    explicit CStateMonsterTestCover(_Object* obj);
    ~CStateMonsterTestCover() override = default;

    void initialize() override;
    void check_force_state() override;
    void reselect_state() override;
    void setup_substates() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "state_test_state_inline.h"
