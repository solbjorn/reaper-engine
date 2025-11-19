#pragma once

#include "../BaseMonster/base_monster.h"
#include "script_export_space.h"

class CChimera : public CBaseMonster
{
    RTTI_DECLARE_TYPEINFO(CChimera, CBaseMonster);

public:
    CChimera();
    ~CChimera() override;

    virtual void Load(LPCSTR section);
    virtual void reinit();
    virtual void UpdateCL();

    void CheckSpecParams(u32) override;
    virtual void HitEntityInJump(const CEntity* pEntity);
    virtual void jump(Fvector const& position, float factor);

private:
    virtual EAction CustomVelocityIndex2Action(u32 velocity_index);

    typedef CBaseMonster inherited;

    SVelocityParam m_velocity_rotate;
    SVelocityParam m_velocity_jump_start;

    struct attack_params
    {
        float attack_radius;
        TTime prepare_jump_timeout;
        TTime attack_jump_timeout;
        TTime stealth_timeout;
        float force_attack_distance;
        u32 num_attack_jumps;
        u32 num_prepare_jumps;
    };

    attack_params m_attack_params;

public:
    attack_params const& get_attack_params() const { return m_attack_params; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CChimera);

add_to_type_list(CChimera);
#undef script_type_list
#define script_type_list save_type_list(CChimera)
