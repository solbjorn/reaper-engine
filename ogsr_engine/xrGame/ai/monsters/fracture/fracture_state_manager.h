#pragma once

#include "../monster_state_manager.h"

class CFracture;

class CStateManagerFracture : public CMonsterStateManager<CFracture>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerFracture, CMonsterStateManager<CFracture>);

private:
    typedef CMonsterStateManager<CFracture> inherited;

public:
    explicit CStateManagerFracture(CFracture* obj);
    ~CStateManagerFracture() override;

    void execute() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
