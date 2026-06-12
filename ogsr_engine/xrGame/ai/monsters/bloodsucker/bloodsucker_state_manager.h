#pragma once

#include "../monster_state_manager.h"

class CAI_Bloodsucker;

class CStateManagerBloodsucker : public CMonsterStateManager<CAI_Bloodsucker>
{
    RTTI_DECLARE_TYPEINFO(CStateManagerBloodsucker, CMonsterStateManager<CAI_Bloodsucker>);

private:
    typedef CMonsterStateManager<CAI_Bloodsucker> inherited;

public:
    explicit CStateManagerBloodsucker(CAI_Bloodsucker* monster);
    ~CStateManagerBloodsucker() override = default;

    void execute() override;
    void update() override;
    [[nodiscard]] bool check_vampire();
    void remove_links(CObject* object) override { inherited::remove_links(object); }
};
