#pragma once

#include "../monster_state_manager.h"

class CSnork;

class CStateManagerSnork : public CMonsterStateManager<CSnork>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerSnork, CMonsterStateManager<CSnork>);

private:
    typedef CMonsterStateManager<CSnork> inherited;

public:
    explicit CStateManagerSnork(CSnork* obj);
    ~CStateManagerSnork() override;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
