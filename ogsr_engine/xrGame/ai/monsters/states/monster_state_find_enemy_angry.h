#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterFindEnemyAngry : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterFindEnemyAngry<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

public:
    explicit CStateMonsterFindEnemyAngry(_Object* obj);
    ~CStateMonsterFindEnemyAngry() override;

    virtual void execute();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_find_enemy_angry_inline.h"
