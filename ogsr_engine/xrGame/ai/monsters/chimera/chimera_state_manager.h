#pragma once

#include "../monster_state_manager.h"

class CChimera;

class CStateManagerChimera : public CMonsterStateManager<CChimera>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerChimera, CMonsterStateManager<CChimera>);

private:
    typedef CMonsterStateManager<CChimera> inherited;

public:
    explicit CStateManagerChimera(CChimera* obj);
    ~CStateManagerChimera() override;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
