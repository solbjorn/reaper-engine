#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../ai_monster_bones.h"
#include "../anim_triple.h"
#include "script_export_space.h"

#define FAKE_DEATH_TYPES_COUNT 4

class CZombie : public CBaseMonster, public CControlledEntity<CZombie>
{
    RTTI_DECLARE_TYPEINFO(CZombie, CBaseMonster, CControlledEntity<CZombie>);

public:
    typedef CBaseMonster inherited;
    typedef CControlledEntity<CZombie> CControlled;

    bonesManipulation Bones;

    CZombie();
    virtual ~CZombie();

    virtual void Load(LPCSTR section);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void reinit();
    virtual void reload(LPCSTR section);

    virtual void Hit(SHit* pHDS);

    virtual bool ability_pitch_correction() { return false; }

    virtual void shedule_Update(u32 dt);

    static void BoneCallback(CBoneInstance* B);
    void vfAssignBones();

    virtual bool use_center_to_aim() const { return true; }

    CBoneInstance* bone_spine;
    CBoneInstance* bone_head;

    SAnimationTripleData anim_triple_death[FAKE_DEATH_TYPES_COUNT];
    u8 active_triple_idx;

    u32 time_dead_start;
    u32 last_hit_frame;
    u32 time_resurrect;

    u8 fake_death_count;
    float health_death_threshold;
    u8 fake_death_left;

    bool fake_death_fall_down(); // return true if everything is ok
    void fake_death_stand_up();

    /*
    #ifdef DEBUG
        virtual void	debug_on_key			(int key);
    #endif
    */

    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CZombie)
#undef script_type_list
#define script_type_list save_type_list(CZombie)
