#pragma once

#include "../BaseMonster/base_monster.h"
#include "../anim_triple.h"
#include "script_export_space.h"
#include "../controlled_actor.h"

class CControllerAnimation;
class CControllerDirection;
class SndShockEffector;
class CControllerPsyHit;
class CControllerAura;

class CController : public CBaseMonster, public CControlledActor
{
    RTTI_DECLARE_TYPEINFO(CController, CBaseMonster, CControlledActor);

public:
    typedef CBaseMonster inherited;

    u8 m_max_controlled_number;
    ref_sound control_start_sound; // звук, который играется в голове у актера
    ref_sound control_hit_sound; // звук, который играется в голове у актера

    ref_sound m_sound_hit_fx;
    SndShockEffector* m_sndShockEffector;

    SAttackEffector m_control_effector;

    u32 time_control_hit_started;
    bool active_control_fx;

    LPCSTR particles_fire;

    CControllerAnimation* m_custom_anim_base;
    CControllerDirection* m_custom_dir_base;

    u32 m_psy_fire_start_time;
    u32 m_psy_fire_delay;

    bool m_tube_at_once;

    float aura_radius;
    float aura_damage;

    //////////////////////////////////////////////////////////////////////////

    CControllerPsyHit* m_psy_hit;

    ref_sound m_sound_aura_left_channel;
    ref_sound m_sound_aura_right_channel;
    ref_sound m_sound_aura_hit_left_channel;
    ref_sound m_sound_aura_hit_right_channel;

    ref_sound m_sound_tube_start;
    ref_sound m_sound_tube_pull;
    ref_sound m_sound_tube_hit_left;
    ref_sound m_sound_tube_hit_right;

    ref_sound m_sound_tube_prepare;

public:
    SVelocityParam m_velocity_move_fwd;
    SVelocityParam m_velocity_move_bkwd;

public:
    CControllerAnimation& custom_anim() { return (*m_custom_anim_base); }
    CControllerDirection& custom_dir() { return (*m_custom_dir_base); }

public:
    xr_vector<CEntity*> m_controlled_objects;

public:
    CController();
    ~CController() override;

    void Load(gsl::czstring section) override;
    void reload(gsl::czstring section) override;
    void reinit() override;
    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<void> Die(CObject* who) override;

    tmc::task<void> net_Destroy() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void net_Relcase(CObject* O) override;

    void CheckSpecParams(u32 spec_params) override;
    void InitThink() override;

    void create_base_controls() override;
    [[nodiscard]] const MonsterSpace::SBoneRotation& head_orientation() const override;

    void TranslateActionToPathParams() override;
    [[nodiscard]] bool ability_pitch_correction() override { return false; }

    //-------------------------------------------------------------------

    [[nodiscard]] bool is_relation_enemy(const CEntityAlive* tpEntityAlive) const override;

    xr_vector<shared_str> m_friend_community_overrides;

    void load_friend_community_overrides(LPCSTR section);
    bool is_community_friend_overrides(const CEntityAlive* tpEntityAlive) const;
    //-------------------------------------------------------------------
    // Controller ability
    bool HasUnderControl() { return (!m_controlled_objects.empty()); }
    void TakeUnderControl(CEntity*);
    void UpdateControlled();
    void FreeFromControl();
    void OnFreedFromControl(const CEntity*); // если монстр сам себя освободил (destroyed || die)

    void set_controlled_task(u32 task);

    tmc::task<void> play_control_sound_start(std::array<std::byte, 16>&);
    tmc::task<void> play_control_sound_hit(std::array<std::byte, 16>&);

    void control_hit();

    void psy_fire();
    bool can_psy_fire();

    void tube_fire();
    bool can_tube_fire();

    float m_tube_damage;
    u32 m_tube_condition_see_duration;
    u32 m_tube_condition_min_delay;
    float m_tube_condition_min_distance;

    void set_psy_fire_delay_zero();
    void set_psy_fire_delay_default();

    float get_tube_min_distance() const { return m_tube_condition_min_distance; }
    bool tube_ready() const;

    //-------------------------------------------------------------------

public:
    void draw_fire_particles();

    void test_covers();

public:
    enum EMentalState
    {
        eStateIdle,
        eStateDanger
    } m_mental_state;

    void set_mental_state(EMentalState state);
    void HitEntity(const CEntity* pEntity, f32 fDamage, f32 impulse, Fvector3& dir, ALife::EHitType hit_type, bool draw_hit_marks) override;

public:
    [[nodiscard]] bool use_center_to_aim() const override { return true; }

    SAnimationTripleData anim_triple_control;

private:
    float m_stamina_hit;

public:
    [[nodiscard]] bool run_home_point_when_enemy_inaccessible() const override { return false; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CController);

add_to_type_list(CController);
#undef script_type_list
#define script_type_list save_type_list(CController)
