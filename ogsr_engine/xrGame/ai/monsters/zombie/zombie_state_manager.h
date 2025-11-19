#pragma once

#include "../monster_state_manager.h"

class CZombie;

class CStateManagerZombie : public CMonsterStateManager<CZombie>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerZombie, CMonsterStateManager<CZombie>);

private:
    typedef CMonsterStateManager<CZombie> inherited;

public:
    explicit CStateManagerZombie(CZombie* obj);
    ~CStateManagerZombie() override;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
