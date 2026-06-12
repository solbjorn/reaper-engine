#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterFindEnemyWalkAround : public CState<_Object>
{
    RTTI_DECLARE_TYPEINFO(CStateMonsterFindEnemyWalkAround<_Object>, CState<_Object>);

private:
    typedef CState<_Object> inherited;
    using inherited::object;

public:
    explicit CStateMonsterFindEnemyWalkAround(_Object* obj) : inherited{obj} {}
    ~CStateMonsterFindEnemyWalkAround() override = default;

    void execute() override;
    [[nodiscard]] bool check_completion() override { return false; }
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};

#include "monster_state_find_enemy_walk_inline.h"
