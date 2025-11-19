#pragma once

#include "../monster_state_manager.h"

class CAI_Dog;

class CStateManagerDog : public CMonsterStateManager<CAI_Dog>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerDog, CMonsterStateManager<CAI_Dog>);

private:
    typedef CMonsterStateManager<CAI_Dog> inherited;

public:
    explicit CStateManagerDog(CAI_Dog* monster);
    ~CStateManagerDog() override = default;

    virtual void execute();
    bool check_eat();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
