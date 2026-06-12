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

    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
