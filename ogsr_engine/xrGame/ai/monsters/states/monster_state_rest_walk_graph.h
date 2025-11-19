#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterRestWalkGraph : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterRestWalkGraph<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterRestWalkGraph(_Object* obj);
    ~CStateMonsterRestWalkGraph() override;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_rest_walk_graph_inline.h"
