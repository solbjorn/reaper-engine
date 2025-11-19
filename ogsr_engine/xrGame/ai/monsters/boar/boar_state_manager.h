#pragma once

#include "../monster_state_manager.h"

class CAI_Boar;

class CStateManagerBoar : public CMonsterStateManager<CAI_Boar>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerBoar, CMonsterStateManager<CAI_Boar>);

private:
    typedef CMonsterStateManager<CAI_Boar> inherited;

public:
    explicit CStateManagerBoar(CAI_Boar* monster);
    ~CStateManagerBoar() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
