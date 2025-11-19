#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterLookActor : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterLookActor<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterLookActor(_Object* obj) : inherited{obj} {}
    ~CStateMonsterLookActor() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

template <typename _Object>
class CStateMonsterTurnAwayFromActor : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterTurnAwayFromActor<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterTurnAwayFromActor(_Object* obj) : inherited{obj} {}
    ~CStateMonsterTurnAwayFromActor() override = default;

    virtual void execute();
};

template <typename _Object>
class CStateMonstertTestIdle : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonstertTestIdle<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonstertTestIdle(_Object* obj) : inherited{obj} {}
    ~CStateMonstertTestIdle() override = default;

    virtual void execute();
};

#include "state_test_look_actor_inline.h"
