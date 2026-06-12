#pragma once

#include "../BaseMonster/base_monster.h"
#include "script_export_space.h"

class CSnork : public CBaseMonster
{
    RTTI_DECLARE_TYPEINFO(CSnork, CBaseMonster);

public:
    typedef CBaseMonster inherited;

    SVelocityParam m_fsVelocityJumpPrepare;
    SVelocityParam m_fsVelocityJumpGround;

    CSnork();
    ~CSnork() override;

    void Load(gsl::czstring section) override;
    void reinit() override;
    tmc::task<void> UpdateCL() override;
    void CheckSpecParams(u32 spec_params) override;
    void jump(const Fvector3& position, f32 factor) override;
    [[nodiscard]] bool ability_jump_over_physics() override { return true; }
    [[nodiscard]] bool ability_distant_feel() override { return true; }
    void HitEntityInJump(const CEntity* pEntity) override;

    bool find_geometry(Fvector& dir);
    float trace(const Fvector& dir);

    bool trace_geometry(const Fvector& d, float& range);

    [[nodiscard]] bool check_start_conditions(ControlCom::EControlType type) override;
    void on_activate_control(ControlCom::EControlType) override;

    [[nodiscard]] bool run_home_point_when_enemy_inaccessible() const override { return false; }

    u32 m_target_node;
    bool start_threaten;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CSnork);

add_to_type_list(CSnork);
#undef script_type_list
#define script_type_list save_type_list(CSnork)
