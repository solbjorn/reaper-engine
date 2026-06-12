#pragma once

#include "../../../CustomMonster.h"

#include "../monster_enemy_memory.h"
#include "../monster_corpse_memory.h"
#include "../monster_sound_memory.h"
#include "../monster_hit_memory.h"

#include "../monster_enemy_manager.h"
#include "../monster_corpse_manager.h"

#include "../../../step_manager.h"
#include "../monster_event_manager.h"
#include "../melee_checker.h"
#include "../monster_morale.h"

#include "../control_manager.h"
#include "../control_sequencer.h"

#include "../ai_monster_utils.h"

#include "../control_manager_custom.h"
#include "../ai_monster_shared_data.h"
#include "../monster_sound_defs.h"
#include "../monster_aura.h"
#include "../../../inventoryowner.h"
#include "../ai_monster_squad.h"

class CCharacterPhysicsSupport;
class CMonsterCorpseCoverEvaluator;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorCloseToEnemy;
class CMonsterEventManager;
class CJumping;
class CControlledEntityBase;
class CMovementManager;
class IStateManagerBase;
class CAnomalyDetector;

class CControlAnimationBase;
class CControlMovementBase;
class CControlPathBuilderBase;
class CControlDirectionBase;
class CMonsterCoverManager;

class CMonsterHome;

// Lain: added
class CMonsterSquad;
class squad_grouping_behaviour;

class anti_aim_ability;

class CBaseMonster : public CCustomMonster, public CStepManager, public CInventoryOwner
{
    RTTI_DECLARE_TYPEINFO(CBaseMonster, CCustomMonster, CStepManager, CInventoryOwner);

public:
    typedef CCustomMonster inherited;

    CBaseMonster();
    ~CBaseMonster() override;

    [[nodiscard]] Feel::Sound* dcast_FeelSound() override { return this; }
    [[nodiscard]] CCharacterPhysicsSupport* character_physics_support() override { return m_pPhysics_support; }
    [[nodiscard]] CPHDestroyable* ph_destroyable() override;
    [[nodiscard]] CEntityAlive* cast_entity_alive() override { return this; }
    [[nodiscard]] CEntity* cast_entity() override { return this; }
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] CCustomMonster* cast_custom_monster() override { return this; }
    [[nodiscard]] CScriptEntity* cast_script_entity() override { return this; }
    [[nodiscard]] CBaseMonster* cast_base_monster() override { return this; }

    [[nodiscard]] CInventoryOwner* cast_inventory_owner() override { return this; }
    [[nodiscard]] bool unlimited_ammo() override { return false; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }

public:
    [[nodiscard]] BOOL renderable_ShadowReceive() override { return TRUE; }
    tmc::task<void> Die(CObject* who) override;
    void HitSignal(f32 amount, Fvector3& vLocalDir, CObject* who, s16 element) override;
    void Hit(SHit* pHDS) override;
    void PHHit(SHit& H) override;
    void SelectAnimation(const Fvector&, const Fvector&, float) override;

    void Load(gsl::czstring section) override;
    virtual void PostLoad(gsl::czstring);
    [[nodiscard]] DLL_Pure* _construct() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Save(NET_Packet& P) override;
    [[nodiscard]] BOOL net_SaveRelevant() override;
    void net_Export(CSE_Abstract* E) override;
    void net_Relcase(CObject* O) override;

    // save/load server serialization
    void save(NET_Packet& output_packet) override { inherited::save(output_packet); }
    void load(IReader& input_packet) override { inherited::load(input_packet); }

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;

    virtual void InitThink() {}
    void Think() override;
    void reinit() override;
    void reload(gsl::czstring section) override;

    virtual void init() {}

    void feel_sound_new(CObject* who, s32 eType, CSound_UserDataPtr user_data, const Fvector3& Position, f32 power, f32 time_to_stop) override;
    [[nodiscard]] BOOL feel_vision_isRelevant(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_on_contact(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject*) override;

    [[nodiscard]] bool useful(const CItemManager*, const CGameObject* object) const override;
    [[nodiscard]] float evaluate(const CItemManager*, const CGameObject*) const override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void OnHUDDraw(ctx_id_t context_id, CCustomHUD* hud, IRenderable* root) override { inherited::OnHUDDraw(context_id, hud, root); }
    [[nodiscard]] u16 PHGetSyncItemsNumber() override { return inherited::PHGetSyncItemsNumber(); }
    [[nodiscard]] CPHSynchronize* PHGetSyncItem(u16 item) override { return inherited::PHGetSyncItem(item); }
    void PHUnFreeze() override { return inherited::PHUnFreeze(); }
    void PHFreeze() override { return inherited::PHFreeze(); }
    [[nodiscard]] BOOL UsedAI_Locations() override { return inherited::UsedAI_Locations(); }

    [[nodiscard]] const SRotation Orientation() const override { return inherited::Orientation(); }
    void renderable_Render(u32 context_id, IRenderable* root) override { inherited::renderable_Render(context_id, root); }

    void on_restrictions_change() override;

    virtual void SetAttackEffector();
    virtual void update_fsm();
    virtual void post_fsm_update();

    void squad_notify();

    [[nodiscard]] bool IsTalkEnabled() override { return false; }

    virtual void HitEntity(const CEntity* pEntity, f32 fDamage, f32 impulse, Fvector3& dir, ALife::EHitType hit_type = ALife::eHitTypeWound,
                           bool draw_hit_marks = true);
    virtual void HitEntityInJump(const CEntity*) {}

    void on_before_sell(CInventoryItem* item) override;
    float GetSatiety() { return 0.5f; }
    void ChangeSatiety(float) {}
    // ---------------------------------------------------------------------------------
    // Process scripts
    // ---------------------------------------------------------------------------------
    [[nodiscard]] bool bfAssignMovement(CScriptEntityAction* tpEntityAction) override;
    bool AssignGamePathIfNeeded(Fvector target_pos, u32 level_vertex);
    [[nodiscard]] bool bfAssignObject(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignWatch(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignAnimation(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignMonsterAction(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignSound(CScriptEntityAction* tpEntityAction) override;

    void vfFinishAction(CScriptEntityAction* tpEntityAction) override;

    void ProcessScripts() override;

    [[nodiscard]] CEntity* GetCurrentEnemy() override;
    [[nodiscard]] CEntity* GetCurrentCorpse() override;
    [[nodiscard]] s32 get_enemy_strength() override;

    void SetScriptControl(const bool bScriptControl, shared_str caSciptName) override;

    virtual void SetEnemy(const CEntityAlive* sent);
    bool m_force_real_speed;
    bool m_script_processing_active;
    bool m_script_state_must_execute;

    virtual void jump(const Fvector3&, f32) {}

    bool m_skip_transfer_enemy;
    IC void skip_transfer_enemy(bool value) { m_skip_transfer_enemy = value; }

    IC int Rank() { return m_rank; }

    //----------------------------------------------------------------------------------

    virtual void SetTurnAnimation(bool turn_left);

    // установка специфических анимаций
    virtual void CheckSpecParams(u32) {}
    virtual void ForceFinalAnimation() {}
    virtual void LookPosition(Fvector to_point); // каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)

    // Team
    void ChangeTeam(s32 team, s32 squad, s32 group) override;

    // ---------------------------------------------------------------------------------
    // Abilities
    // ---------------------------------------------------------------------------------
    [[nodiscard]] virtual bool ability_invisibility() { return false; }
    [[nodiscard]] virtual bool ability_can_drag() { return false; }
    [[nodiscard]] virtual bool ability_psi_attack() { return false; }
    [[nodiscard]] virtual bool ability_earthquake() { return false; }
    virtual bool ability_can_jump() { return false; }
    [[nodiscard]] virtual bool ability_distant_feel() { return false; }
    virtual bool ability_run_attack() { return false; }
    virtual bool ability_rotation_jump() { return false; }
    [[nodiscard]] virtual bool ability_jump_over_physics() { return false; }
    [[nodiscard]] virtual bool ability_pitch_correction() { return true; }
    // ---------------------------------------------------------------------------------

    void event_on_step() override {}
    virtual void on_threaten_execute() {}
    // ---------------------------------------------------------------------------------
    // Memory
    void UpdateMemory();

    // Cover
    bool GetCorpseCover(Fvector& position, u32& vertex_id);
    bool GetCoverFromEnemy(const Fvector& enemy_pos, Fvector& position, u32& vertex_id);
    bool GetCoverFromPoint(const Fvector& pos, Fvector& position, u32& vertex_id, float min_dist, float max_dist, float radius);
    bool GetCoverCloseToPoint(const Fvector& dest_pos, float min_dist, float max_dist, float deviation, float radius, Fvector& position, u32& vertex_id);

    // Movement Manager
protected:
    CControlPathBuilder* m_movement_manager;

protected:
    [[nodiscard]] CMovementManager* create_movement_manager() override;

    // --------------------------------------------------------------------------------------
    // Monster Settings
    SMonsterSettings m_current_settings;

    // members
public:
    void set_force_anti_aim(bool force_anti_aim) { m_force_anti_aim = force_anti_aim; }
    bool get_force_anti_aim() const { return m_force_anti_aim; }

    void settings_read(CInifile* ini, LPCSTR section, SMonsterSettings& data);
    void settings_load(LPCSTR section);
    void settings_overrides();

    SMonsterSettings& db() { return (m_current_settings); }
    // --------------------------------------------------------------------------------------

    CCharacterPhysicsSupport* m_pPhysics_support;

    CMonsterCorpseCoverEvaluator* m_corpse_cover_evaluator;
    CCoverEvaluatorFarFromEnemy* m_enemy_cover_evaluator;
    CCoverEvaluatorCloseToEnemy* m_cover_evaluator_close_point;

    // ---------------------------------------------------------------------------------
    IStateManagerBase* StateMan{};
    // ---------------------------------------------------------------------------------

    CMonsterEnemyMemory EnemyMemory;
    CMonsterSoundMemory SoundMemory;
    CMonsterCorpseMemory CorpseMemory;
    CMonsterHitMemory HitMemory;

    CMonsterEnemyManager EnemyMan;
    CMonsterCorpseManager CorpseMan;

    const CEntityAlive* EatedCorpse{};
    // Lain: added
    bool check_eated_corpse_draggable();
    [[nodiscard]] bool is_base_monster_with_enemy() override { return !!EnemyMan.get_enemy(); }

    bool hear_dangerous_sound;
    bool hear_interesting_sound;

    // -----------------------------------------------------------------------------
    CMonsterEventManager EventMan;
    // -----------------------------------------------------------------------------

    CMeleeChecker MeleeChecker;
    CMonsterMorale Morale;

    // -----------------------------------------------------------------------------

    CMonsterCoverManager* CoverMan;

    // -----------------------------------------------------------------------------

    CControlledEntityBase* m_controlled{};

    // -----------------------------------------------------------------------------
    enum EMonsterType
    {
        eMonsterTypeUniversal = u32(0),
        eMonsterTypeIndoor,
        eMonsterTypeOutdoor,
    } m_monster_type;

    // -----------------------------------------------------------------------------
    // Home
    CMonsterHome* Home;

private:
    bool m_force_anti_aim;

    //--------------------------------------------------------------------
    // Berserk
    //--------------------------------------------------------------------
public:
    u32 time_berserk_start;
    IC void set_berserk() { time_berserk_start = time(); }
    bool berserk_always;

    //--------------------------------------------------------------------
    // Panic Threshold (extension for scripts)
    //--------------------------------------------------------------------

    float m_default_panic_threshold;
    IC void set_custom_panic_threshold(float value);
    IC void set_default_panic_threshold();
    //--------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // -----------------------------------------------------------------------------
    // Special Services (refactoring needed)

    void on_kill_enemy(const CEntity* obj);
    void Hit_Psy(CObject* object, float value);
    void Hit_Wound(CObject* object, float value, const Fvector& dir, float impulse);
    CParticlesObject* PlayParticles(const shared_str& name, const Fvector& position, const Fvector& dir, BOOL auto_remove = TRUE, BOOL xformed = TRUE);
    void load_effector(LPCSTR section, LPCSTR line, SAttackEffector& effector);

    // --------------------------------------------------------------------------------------
    // Kill From Here
    // --------------------------------------------------------------------------------------
    // State flags
    bool m_bDamaged;
    bool m_bAngry;
    bool m_bGrowling;
    bool m_bAggressive;
    bool m_bSleep;
    bool m_bRunTurnLeft;
    bool m_bRunTurnRight;

    void set_aggressive(bool val = true) { m_bAggressive = val; }

    //---------------------------------------------------------------------------------------

    u32 m_prev_sound_type;
    u32 get_attack_rebuild_time();

    [[nodiscard]] virtual EAction CustomVelocityIndex2Action(u32) { return ACT_STAND_IDLE; }
    virtual void TranslateActionToPathParams();

    bool state_invisible;

    void set_action(EAction action);
    void set_state_sound(u32 type, bool once = false);
    IC void fall_asleep() { m_bSleep = true; }
    IC void wake_up() { m_bSleep = false; }

    // Temp
    u32 m_time_last_attack_success;
    int m_rank;
    float m_melee_rotation_factor;

private:
    bool ignore_collision_hit;

public:
    IC void set_ignore_collision_hit(bool value) { ignore_collision_hit = value; }
    // -----------------------------------------------------------------------------
    //////////////////////////////////////////////////////////////////////////

public:
    CControl_Manager& control() { return (*m_control_manager); }

    CControlAnimationBase& anim() { return (*m_anim_base); }
    CControlMovementBase& move() { return (*m_move_base); }
    CControlPathBuilderBase& path() { return (*m_path_base); }
    CControlDirectionBase& dir() { return (*m_dir_base); }

    CControlManagerCustom& com_man() { return m_com_manager; }

    [[nodiscard]] virtual bool check_start_conditions(ControlCom::EControlType);
    virtual void on_activate_control(ControlCom::EControlType) {}

protected:
    CControl_Manager* m_control_manager;

    CControlAnimationBase* m_anim_base;
    CControlMovementBase* m_move_base;
    CControlPathBuilderBase* m_path_base;
    CControlDirectionBase* m_dir_base;

    CControlManagerCustom m_com_manager;

    virtual void create_base_controls();

    //////////////////////////////////////////////////////////////////////////
    // Critical Wounded
    //////////////////////////////////////////////////////////////////////////
    enum
    {
        critical_wound_type_head = u32(0),
        critical_wound_type_torso,
        critical_wound_type_legs
    };

    void load_critical_wound_bones() override;
    [[nodiscard]] bool critical_wound_external_conditions_suitable() override;
    void critical_wounded_state_start() override;

    void fill_bones_body_parts(LPCSTR body_part, CriticalWoundType wound_type);

    LPCSTR m_critical_wound_anim_head;
    LPCSTR m_critical_wound_anim_torso;
    LPCSTR m_critical_wound_anim_legs;

    //////////////////////////////////////////////////////////////////////////

public:
    virtual void on_attack_on_run_hit() {}

//////////////////////////////////////////////////////////////////////////
// DEBUG stuff
#ifdef DEBUG
public:
    struct SDebugInfo
    {
        bool active;
        float x;
        float y;
        float delta_y;
        u32 color;
        u32 delimiter_color;

        SDebugInfo() : active(false) {}
        SDebugInfo(float px, float py, float dy, u32 c, u32 dc) : active(true), x(px), y(py), delta_y(dy), color(c), delimiter_color(dc) {}
    };

    u8 m_show_debug_info; // 0 - none, 1 - first column, 2 - second column
    void set_show_debug_info(u8 show = 1) { m_show_debug_info = show; }
    virtual SDebugInfo show_debug_info();

    void debug_fsm();

    virtual void debug_on_key(xr::key_id key) {}
#endif
    //////////////////////////////////////////////////////////////////////////

public:
    bool is_jumping();

    //-------------------------------------------------------------------
    // CBaseMonster's      Steering Behaviour
    //-------------------------------------------------------------------
public:
    steering_behaviour::manager* get_steer_manager();

    float get_feel_enemy_who_just_hit_max_distance() { return m_feel_enemy_who_just_hit_max_distance; }
    float get_feel_enemy_who_made_sound_max_distance() { return m_feel_enemy_who_made_sound_max_distance; }
    float get_feel_enemy_max_distance() { return m_feel_enemy_max_distance; }
    [[nodiscard]] virtual bool can_use_agressive_jump(const CObject*) { return false; }

private:
    steering_behaviour::manager* m_steer_manager{};
    squad_grouping_behaviour* m_grouping_behaviour{}; // freed by manager

    void update_enemy_accessible_and_at_home_info();
    // updates position by applying little "pushing" force
    // so that monsters rarely intersect
    void update_pos_by_grouping_behaviour();
    TTime m_last_grouping_behaviour_update_tick{};

    float m_feel_enemy_who_made_sound_max_distance{};
    float m_feel_enemy_who_just_hit_max_distance{};
    float m_feel_enemy_max_distance{};

    //-------------------------------------------------------------------
    // CBaseMonster's  Atack on Move Parameters
    //-------------------------------------------------------------------
public:
    struct attack_on_move_params_t
    {
        bool enabled;
        float max_go_close_time;
        float far_radius;
        float prepare_radius;
        float prepare_time;
        float attack_radius;
        float update_side_period;
        float prediction_factor;
    };

    bool can_attack_on_move();
    float get_attack_on_move_max_go_close_time();
    float get_attack_on_move_far_radius();
    float get_attack_on_move_attack_radius();
    float get_attack_on_move_update_side_period();
    float get_attack_on_move_prediction_factor();
    float get_attack_on_move_prepare_radius();
    float get_attack_on_move_prepare_time();

    bool enemy_accessible();
    bool at_home();

protected:
    attack_on_move_params_t m_attack_on_move_params;

    //-------------------------------------------------------------------
    // CBaseMonster's  Auras
    //-------------------------------------------------------------------
public:
    float get_psy_influence();
    float get_radiation_influence();
    float get_fire_influence();
    void play_detector_sound();

private:
    monster_aura m_psy_aura;
    monster_aura m_radiation_aura;
    monster_aura m_fire_aura;
    monster_aura m_base_aura;

protected:
    //-------------------------------------------------------------------
    // CBaseMonster's  Anti-Aim Ability
    //-------------------------------------------------------------------
    anti_aim_ability* m_anti_aim{};

private:
    pcstr m_head_bone_name;
    pcstr m_left_eye_bone_name;
    pcstr m_right_eye_bone_name;

public:
    pcstr get_head_bone_name() const { return m_head_bone_name; }
    anti_aim_ability* get_anti_aim() { return m_anti_aim; }

private:
    void update_eyes_visibility();
    float get_screen_space_coverage_diagonal();

    void GenerateNewOffsetFromLeader();
    u32 m_offset_from_leader_chosen_tick;
    Fvector m_offset_from_leader;

    // very special copies, used when pos is not on ai-map
    // in that situation m_action_target_node is close node
    Fvector m_action_target_pos;
    u32 m_action_target_node;

    TTime m_first_tick_enemy_inaccessible{};
    TTime m_last_tick_enemy_inaccessible{};
    TTime m_first_tick_object_not_at_home{};

public:
    [[nodiscard]] virtual bool run_home_point_when_enemy_inaccessible() const { return true; }
    [[nodiscard]] virtual bool need_shotmark() const { return true; }
};
XR_SOL_BASE_CLASSES(CBaseMonster);

#include "base_monster_inline.h"
