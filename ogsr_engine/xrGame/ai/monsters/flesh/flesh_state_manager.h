#pragma once

#include "../monster_state_manager.h"

class CAI_Flesh;

class CStateManagerFlesh : public CMonsterStateManager<CAI_Flesh>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerFlesh, CMonsterStateManager<CAI_Flesh>);

private:
    typedef CMonsterStateManager<CAI_Flesh> inherited;

public:
    explicit CStateManagerFlesh(CAI_Flesh* monster);
    ~CStateManagerFlesh() override = default;

    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};
