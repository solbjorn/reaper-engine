#pragma once

#include "../BaseMonster/base_monster.h"
#include "script_export_space.h"

class CAI_PseudoDog : public CBaseMonster
{
    RTTI_DECLARE_TYPEINFO(CAI_PseudoDog, CBaseMonster);

public:
    typedef CBaseMonster inherited;

    float m_anger_hunger_threshold;
    float m_anger_loud_threshold;

    TTime m_time_became_angry;

    TTime time_growling; // время нахождения в состоянии пугания

    enum
    {
        eAdditionalSounds = MonsterSound::eMonsterSoundCustom,
        ePsyAttack = eAdditionalSounds | 0,
    };

    CAI_PseudoDog();
    ~CAI_PseudoDog() override;

    virtual DLL_Pure* _construct();

    virtual void Load(LPCSTR section);

    virtual void reinit();
    virtual void reload(LPCSTR section);

    virtual bool ability_can_drag() { return true; }
    virtual bool ability_psi_attack() { return true; }

    virtual void CheckSpecParams(u32 spec_params);
    // virtual void	play_effect_sound	();

    virtual void HitEntityInJump(const CEntity* pEntity);

    virtual IStateManagerBase* create_state_manager();

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CAI_PseudoDog);

add_to_type_list(CAI_PseudoDog);
#undef script_type_list
#define script_type_list save_type_list(CAI_PseudoDog)
