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
    virtual ~CAI_Flesh();

    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);

    virtual void CheckSpecParams(u32 spec_params);

    virtual bool ability_can_drag() { return true; }

private:
    bool ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Flesh)
#undef script_type_list
#define script_type_list save_type_list(CAI_Flesh)
