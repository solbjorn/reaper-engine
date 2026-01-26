#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "script_export_space.h"

class CAI_Boar : public CBaseMonster, public CControlledEntity<CAI_Boar>
{
    RTTI_DECLARE_TYPEINFO(CAI_Boar, CBaseMonster, CControlledEntity<CAI_Boar>);

public:
    typedef CBaseMonster inherited;
    typedef CControlledEntity<CAI_Boar> CControlled;

    CAI_Boar();
    ~CAI_Boar() override;

    virtual void Load(LPCSTR section);
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    virtual void reinit();

    tmc::task<void> UpdateCL() override;

    IC virtual bool CanExecRotationJump() { return true; }
    void CheckSpecParams(u32) override {}

    // look at enemy
    static void BoneCallback(CBoneInstance* B);

    float _velocity;
    float _cur_delta, _target_delta;
    bool look_at_enemy;

    IC virtual bool ability_can_drag() { return true; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CAI_Boar);

add_to_type_list(CAI_Boar);
#undef script_type_list
#define script_type_list save_type_list(CAI_Boar)
