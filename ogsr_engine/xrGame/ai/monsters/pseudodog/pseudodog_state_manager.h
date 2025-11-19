#pragma once

#include "../monster_state_manager.h"

class CAI_PseudoDog;

class CStateManagerPseudodog : public CMonsterStateManager<CAI_PseudoDog>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerPseudodog, CMonsterStateManager<CAI_PseudoDog>);

private:
    typedef CMonsterStateManager<CAI_PseudoDog> inherited;

public:
    explicit CStateManagerPseudodog(CAI_PseudoDog* monster);
    ~CStateManagerPseudodog() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
