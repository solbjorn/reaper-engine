#pragma once

#include "../monster_state_manager.h"

class CTushkano;

class CStateManagerTushkano : public CMonsterStateManager<CTushkano>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerTushkano, CMonsterStateManager<CTushkano>);

private:
    typedef CMonsterStateManager<CTushkano> inherited;

public:
    explicit CStateManagerTushkano(CTushkano* obj);
    ~CStateManagerTushkano() override;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
