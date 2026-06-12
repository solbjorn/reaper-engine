#pragma once

#include "../monster_state_manager.h"

class CBurer;

class CStateManagerBurer : public CMonsterStateManager<CBurer>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerBurer, CMonsterStateManager<CBurer>);

private:
    typedef CMonsterStateManager<CBurer> inherited;

public:
    explicit CStateManagerBurer(CBurer* monster);
    ~CStateManagerBurer() override = default;

    void execute() override;
    void setup_substates() override;
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
