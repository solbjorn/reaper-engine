#pragma once

#include "../state.h"
#include "../states/state_data.h"

template <typename _Object>
class CStateGroupSquadMoveToRadiusEx : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupSquadMoveToRadiusEx<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

protected:
    SStateDataMoveToPointEx data;

public:
    explicit CStateGroupSquadMoveToRadiusEx(_Object* obj) : inherited{obj, &data} {}
    ~CStateGroupSquadMoveToRadiusEx() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

template <typename _Object>
class CStateGroupSquadMoveToRadius : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateGroupSquadMoveToRadius<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

protected:
    SStateDataMoveToPointEx data;

public:
    explicit CStateGroupSquadMoveToRadius(_Object* obj) : inherited{obj, &data} {}
    ~CStateGroupSquadMoveToRadius() override = default;

    virtual void initialize();
    virtual void execute();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "group_state_squad_move_to_radius_inline.h"
