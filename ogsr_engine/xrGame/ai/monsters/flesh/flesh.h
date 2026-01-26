#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "script_export_space.h"

class CAI_Flesh : public CBaseMonster, public CControlledEntity<CAI_Flesh>
{
    RTTI_DECLARE_TYPEINFO(CAI_Flesh, CBaseMonster, CControlledEntity<CAI_Flesh>);

public:
    typedef CBaseMonster inherited;
    typedef CControlledEntity<CAI_Flesh> CControlled;

    CAI_Flesh();
    ~CAI_Flesh() override;

    virtual void Load(LPCSTR section);
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;

    virtual void CheckSpecParams(u32 spec_params);

    virtual bool ability_can_drag() { return true; }

private:
    bool ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CAI_Flesh);

add_to_type_list(CAI_Flesh);
#undef script_type_list
#define script_type_list save_type_list(CAI_Flesh)
