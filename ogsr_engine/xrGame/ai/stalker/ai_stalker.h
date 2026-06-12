////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../object_handler.h"
#include "../../AI_PhraseDialogManager.h"
#include "../../step_manager.h"
#include "script_export_space.h"

#ifdef DEBUG
template <typename _object_type>
class CActionBase;

template <typename _object_type>
class CPropertyEvaluator;

template <typename _object_type, bool _reverse_search, typename _world_operator, typename _condition_evaluator, typename _world_operator_ptr,
          typename _condition_evaluator_ptr>
class CActionPlanner;

typedef CActionPlanner<CScriptGameObject, false, CActionBase<CScriptGameObject>, CPropertyEvaluator<CScriptGameObject>, CActionBase<CScriptGameObject>*,
                       CPropertyEvaluator<CScriptGameObject>*>
    script_planner;
#endif

namespace MonsterSpace
{
enum EMovementDirection : u32;
}

namespace StalkerSpace
{
enum EBodyAction : u32;
}

enum ECriticalWoundType : u32;

class CALifeSimulator;
class CCharacterPhysicsSupport;
class CWeapon;
class CCoverPoint;
class CCoverEvaluatorCloseToEnemy;
class CCoverEvaluatorFarFromEnemy;
class CCoverEvaluatorBest;
class CCoverEvaluatorAngle;
class CCoverEvaluatorSafe;
class CCoverEvaluatorRandomGame;
class CCoverEvaluatorAmbush;
class CCoverEvaluatorBestByTime;
class CAgentManager;
class CMotionDef;
class CStalkerAnimationManager;
class CStalkerPlanner;
class CSightManager;
class CStalkerMovementManager;
class CStalkerSoundDataVisitor;
class CWeaponShotEffector;
struct SBoneProtections;
class CDangerLocation;
class CRestrictedObject;

class CAI_Stalker : public CCustomMonster, public CObjectHandler, public CAI_PhraseDialogManager, public CStepManager
{
    RTTI_DECLARE_TYPEINFO(CAI_Stalker, CCustomMonster, CObjectHandler, CAI_PhraseDialogManager, CStepManager);

private:
    typedef CCustomMonster inherited;

public:
    using inherited::evaluate;
    using inherited::useful;

private:
    CStalkerAnimationManager* m_animation_manager;
    CStalkerPlanner* m_brain{};
    CSightManager* m_sight_manager;
    CStalkerMovementManager* m_movement_manager{};

#ifdef DEBUG
    const script_planner* m_debug_planner{};
#endif

    // ALife
private:
    SBoneProtections* m_boneHitProtection{};

    // weapon dispersion
public:
    float m_fDispBase;

private:
    float m_disp_walk_stand;
    float m_disp_walk_crouch;
    float m_disp_run_stand;
    float m_disp_run_crouch;
    float m_disp_stand_stand;
    float m_disp_stand_crouch;
    float m_disp_stand_stand_zoom;
    float m_disp_stand_crouch_zoom;
    bool m_fast_can_kill_entity;

private:
    float m_power_fx_factor{std::numeric_limits<float>::max()};

private:
    float m_fRankDisperison;
    float m_fRankVisibility;
    float m_fRankImmunity;

    // best item/ammo selection members
public:
    bool m_item_actuality;
    CInventoryItem* m_best_item_to_kill;
    float m_best_item_value;
    CInventoryItem* m_best_ammo;
    const CInventoryItem* m_best_found_item_to_kill;
    const CInventoryItem* m_best_found_ammo;

    // covers being used
public:
    CCoverEvaluatorCloseToEnemy* m_ce_close;
    CCoverEvaluatorFarFromEnemy* m_ce_far;
    CCoverEvaluatorBest* m_ce_best;
    CCoverEvaluatorAngle* m_ce_angle;
    CCoverEvaluatorSafe* m_ce_safe;
    CCoverEvaluatorRandomGame* m_ce_random_game;
    CCoverEvaluatorAmbush* m_ce_ambush;
    CCoverEvaluatorBestByTime* m_ce_best_by_time;

    // physics support
public:
    CCharacterPhysicsSupport* m_pPhysics_support;

public:
    bool m_wounded{};
    bool m_headshot{};

public:
    CAI_Stalker();
    ~CAI_Stalker() override;

public:
    [[nodiscard]] CCharacterPhysicsSupport* character_physics_support() override { return m_pPhysics_support; }
    [[nodiscard]] CPHDestroyable* ph_destroyable() override;
    [[nodiscard]] CAttachmentOwner* cast_attachment_owner() override { return this; }
    [[nodiscard]] CInventoryOwner* cast_inventory_owner() override { return this; }
    [[nodiscard]] CEntityAlive* cast_entity_alive() override { return this; }
    [[nodiscard]] CEntity* cast_entity() override { return this; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] Feel::Sound* dcast_FeelSound() override { return this; }
    [[nodiscard]] CAI_Stalker* cast_stalker() override { return this; }
    [[nodiscard]] CCustomMonster* cast_custom_monster() override { return this; }
    [[nodiscard]] CScriptEntity* cast_script_entity() override { return this; }

public:
    void init();
    void Load(gsl::czstring section) override;
    void reinit() override;
    void reload(gsl::czstring section) override;
    virtual void LoadSounds(LPCSTR section);

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void net_Export(CSE_Abstract* E) override;
    tmc::task<void> net_Destroy() override;
    void net_Save(NET_Packet& P) override;
    [[nodiscard]] BOOL net_SaveRelevant() override;
    void net_Relcase(CObject* O) override;

    // save/load server serialization
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    void Think() override;
    void SelectAnimation(const Fvector&, const Fvector&, float) override;
    [[nodiscard]] BOOL UsedAI_Locations() override;

    void g_WeaponBones(s32& L, s32& R1, s32& R2) override;
    void g_fireParams(CHudItem*, Fvector& P, Fvector& D, const bool = false) override;
    void HitSignal(f32 P, Fvector& vLocalDir, CObject* who, s16 element) override;
    tmc::task<void> Die(CObject* who) override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void feel_touch_new(CObject* O) override;

    void renderable_Render(u32 context_id, IRenderable* root) override;
    void Exec_Look(f32 dt) override;
    void Hit(SHit* pHDS) override;
    void PHHit(SHit& H) override;
    [[nodiscard]] BOOL feel_vision_isRelevant(CObject* who) override;
    [[nodiscard]] f32 Radius() const override;

#ifdef DEBUG
    void OnHUDDraw(ctx_id_t context_id, CCustomHUD* hud, IRenderable* root) override;
    virtual void OnRender();
#endif

    [[nodiscard]] bool useful(const CItemManager*, const CGameObject* object) const override;
    [[nodiscard]] float evaluate(const CItemManager*, const CGameObject* object) const override;
    [[nodiscard]] bool useful(const CEnemyManager*, const CEntityAlive* object) const override;

    // PDA && Dialogs
    void UpdateAvailableDialogs(CPhraseDialogManager* partner) override;

    // scripts
    virtual CWeapon* GetCurrentWeapon() const;
    virtual u32 GetWeaponAmmo() const;
    //	virtual CInventoryItem				*GetCurrentEquipment	() const; <- moved to InventoryOwner::GetCurrentOutfit
    virtual CInventoryItem* GetMedikit() const;
    virtual CInventoryItem* GetFood() const;
    [[nodiscard]] bool bfAssignMovement(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignWatch(CScriptEntityAction* tpEntityAction) override;
    void ResetScriptData(void* P = nullptr) override;
    [[nodiscard]] bool bfAssignObject(CScriptEntityAction* tpEntityAction) override;
    [[nodiscard]] bool bfAssignAnimation(CScriptEntityAction* tpEntityAction) override;

    // physics
    [[nodiscard]] u16 PHGetSyncItemsNumber() override { return inherited ::PHGetSyncItemsNumber(); }
    [[nodiscard]] CPHSynchronize* PHGetSyncItem(u16 item) override { return inherited ::PHGetSyncItem(item); }
    void PHUnFreeze() override { return inherited ::PHUnFreeze(); }
    void PHFreeze() override { return inherited ::PHFreeze(); }

    // miscellanious functions
    void DropItemSendMessage(CObject* O);
    [[nodiscard]] ALife::ERelationType tfGetRelationType(const CEntityAlive* tpEntityAlive) const override;
    [[nodiscard]] const SRotation Orientation() const override;
    [[nodiscard]] const MonsterSpace::SBoneRotation& head_orientation() const override;

    // InventoryOwner stuff
    bool CanPutInSlot(PIItem item, u32 slot) override;

    //////////////////////////////////////////////////////////////////////////
    // action/evaluators support functions
    //////////////////////////////////////////////////////////////////////////
public:
    void OnItemTake(CInventoryItem* inventory_item) override;
    void OnItemDrop(CInventoryItem* inventory_item) override;
    bool item_to_kill();
    bool item_can_kill();
    bool remember_item_to_kill();
    bool remember_ammo();
    bool ready_to_kill();
    bool ready_to_detour();
    void update_best_item_info();
    [[nodiscard]] f32 GetWeaponAccuracy() const override;
    [[nodiscard]] bool unlimited_ammo() override;
    void spawn_supplies() override;
    CAgentManager& agent_manager() const;

    [[nodiscard]] bool human_being() const override { return true; }

    bool undetected_anomaly();
    bool inside_anomaly();

private:
    bool m_can_kill_member;
    bool m_can_kill_enemy;
    float m_pick_distance;
    u32 m_pick_frame_id;
    collide::rq_results rq_storage;

private:
    void can_kill_entity(const Fvector& position, const Fvector& direction, float distance, collide::rq_results& rq_storage);
    void can_kill_entity_from(const Fvector& position, Fvector direction, float distance);
    void update_can_kill_info();

public:
    bool can_kill_member();
    bool can_kill_enemy();
    float pick_distance();
    IC float start_pick_distance() const;
    bool fire_make_sense();
    bool can_fire_to_enemy(const CEntityAlive* enemy);

    [[nodiscard]] gsl::czstring Name() const override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_on_contact(CObject* O) override;

    // флаги, какие действия совершал актер по отношению к сталкеру
    //(помог, атаковал и т.д.)
    Flags32 m_actor_relation_flags;

    // ALife
private:
    struct CTradeItem
    {
        CInventoryItem* m_item;
        ALife::_OBJECT_ID m_owner_id;
        ALife::_OBJECT_ID m_new_owner_id;

        IC CTradeItem(CInventoryItem* item, ALife::_OBJECT_ID owner_id, ALife::_OBJECT_ID new_owner_id)
        {
            m_item = item;
            m_owner_id = owner_id;
            m_new_owner_id = new_owner_id;
        }

        IC bool operator<(const CTradeItem& trade_item) const;
        IC bool operator==(u16 id) const;
    };

private:
    CGameObject* m_trader_game_object;
    CInventoryOwner* m_current_trader;
    xr_vector<CTradeItem> m_temp_items;
    u32 m_total_money;
    bool m_sell_info_actuality;

protected:
    u32 fill_items(CInventory& inventory, CGameObject* old_owner, ALife::_OBJECT_ID new_owner_id);

    IC void buy_item_virtual(CTradeItem& item);
    void attach_available_ammo(CWeapon* weapon);
    void choose_food();
    void choose_weapon(ALife::EWeaponPriorityType weapon_priority_type);
    void choose_medikit();
    // void						choose_detector					();
    void choose_equipment();

    void select_items();
    void transfer_item(CInventoryItem* item, CGameObject* old_owner, CGameObject* new_owner);

    void update_sell_info();
    bool tradable_item(CInventoryItem* inventory_item, const u16& current_owner_id);
    bool can_sell(CInventoryItem const* item);
    bool can_take(CInventoryItem const* item);

    bool non_conflicted(const CInventoryItem* item, const CWeapon* new_weapon) const;
    bool enough_ammo(const CWeapon* new_weapon) const;
    bool conflicted(const CInventoryItem* item, const CWeapon* new_weapon, bool new_wepon_enough_ammo, int new_weapon_rank) const;
    void update_conflicted(CInventoryItem* item, const CWeapon* new_weapon);
    void remove_personal_only_ammo(const CInventoryItem* item);
    void on_after_take(const CGameObject* object);
    [[nodiscard]] bool AllowItemToTrade(CInventoryItem const* item, EItemPlace) const override;

public:
    IC CStalkerAnimationManager& animation() const;
    IC CStalkerPlanner& brain() const;
    IC CSightManager& sight() const;

private:
    CStalkerSoundDataVisitor* m_sound_user_data_visitor{};

protected:
    [[nodiscard]] CSound_UserDataVisitor* create_sound_visitor() override;
    [[nodiscard]] CMemoryManager* create_memory_manager() override;
    [[nodiscard]] CMovementManager* create_movement_manager() override;

public:
    IC CStalkerMovementManager& movement() const;
    [[nodiscard]] DLL_Pure* _construct() override;

private:
    IC bool frame_check(u32& frame);
    [[nodiscard]] bool natural_weapon() const override { return false; }
    [[nodiscard]] bool natural_detector() const override { return false; }
    [[nodiscard]] bool use_center_to_aim() const override;
    void process_enemies();

private:
    bool m_group_behaviour{true};

public:
    IC bool group_behaviour() const;
    void update_range_fov(f32& new_range, f32& new_fov, f32 start_range, f32 start_fov) override;
    tmc::task<void> update_object_handler();
    bool zoom_state() const;
    void react_on_grenades();
    void react_on_member_death();

private:
    CWeaponShotEffector* m_weapon_shot_effector;

public:
    void on_weapon_shot_start(CWeapon* weapon) override;
    void on_weapon_shot_stop(CWeapon*) override;
    void on_weapon_hide(CWeapon*) override;
    IC CWeaponShotEffector& weapon_shot_effector() const;
    IC Fvector weapon_shot_effector_direction(const Fvector& current) const;
    tmc::task<void> UpdateCamera() override;
    [[nodiscard]] bool can_attach(const CInventoryItem* inventory_item) const override;
    [[nodiscard]] bool use_simplified_visual() const override { return already_dead(); }

#ifdef DEBUG
    void debug_planner(const script_planner* planner);
#endif

private:
    u32 m_min_queue_size_far;
    u32 m_max_queue_size_far;
    u32 m_min_queue_interval_far;
    u32 m_max_queue_interval_far;

    u32 m_min_queue_size_medium;
    u32 m_max_queue_size_medium;
    u32 m_min_queue_interval_medium;
    u32 m_max_queue_interval_medium;

    u32 m_min_queue_size_close;
    u32 m_max_queue_size_close;
    u32 m_min_queue_interval_close;
    u32 m_max_queue_interval_close;

public:
    IC u32 min_queue_size_far() const;
    IC u32 max_queue_size_far() const;
    IC u32 min_queue_interval_far() const;
    IC u32 max_queue_interval_far() const;

    IC u32 min_queue_size_medium() const;
    IC u32 max_queue_size_medium() const;
    IC u32 min_queue_interval_medium() const;
    IC u32 max_queue_interval_medium() const;

    IC u32 min_queue_size_close() const;
    IC u32 max_queue_size_close() const;
    IC u32 min_queue_interval_close() const;
    IC u32 max_queue_interval_close() const;

public:
    using on_best_cover_changed_delegate = CallMe::Delegate<void(const CCoverPoint*, const CCoverPoint*)>;

private:
    typedef xr_vector<on_best_cover_changed_delegate> cover_delegates;

private:
    cover_delegates m_cover_delegates;
    const CCoverPoint* m_best_cover;
    float m_best_cover_value;
    bool m_best_cover_actual;
    bool m_best_cover_can_try_advance;
    const CCoverPoint* m_best_cover_advance_cover;

private:
    float best_cover_value(const Fvector& position_to_cover_from);
    const CCoverPoint* find_best_cover(const Fvector& position_to_cover_from);
    void update_best_cover_actuality(const Fvector& position_to_cover_from);
    void on_best_cover_changed(const CCoverPoint* new_cover, const CCoverPoint* old_cover);

public:
    void best_cover_can_try_advance();
    const CCoverPoint* best_cover(const Fvector& position_to_cover_from);

    void subscribe_on_best_cover_changed(const on_best_cover_changed_delegate& delegate);
    void unsubscribe_on_best_cover_changed(const on_best_cover_changed_delegate& delegate);

    void on_enemy_change(const CEntityAlive* enemy) override;
    void on_restrictions_change() override;
    void on_cover_blocked();
    void on_danger_location_add(const CDangerLocation& location);
    void on_danger_location_remove(const CDangerLocation& location);

    void wounded(bool value);
    bool wounded(const CRestrictedObject* object) const;
    IC bool wounded() const;

    // throwing grenades
private:
    // actuality parameters
    bool m_throw_actual;
    Fvector m_computed_object_position;
    Fvector m_computed_object_direction;
    // target parameters
    Fvector m_throw_target_position;
    CObject* m_throw_ignore_object;
    // computed
    Fvector m_throw_position;
    Fvector m_throw_velocity;
    // collision prediction
    Fvector m_throw_collide_position;
    bool m_throw_enabled;

    u32 m_last_throw_time;
    u32 m_throw_time_interval;

#ifdef DEBUG
    xr_vector<Fvector> m_throw_picks;
#endif // DEBUG

public:
    IC const bool& throw_enabled();

private:
    bool m_can_throw_grenades;

public:
    IC const bool& can_throw_grenades() const;
    IC void can_throw_grenades(const bool& value);

private:
    bool throw_check_error(float low, float high, const Fvector& start, const Fvector& velocity, const Fvector& gravity);
    void check_throw_trajectory(const float& throw_time);
    void throw_target_impl(const Fvector& position, CObject* throw_ignore_object);
    void compute_throw_miss(u32 const vertex_id);

public:
    [[nodiscard]] bool use_default_throw_force() override;
    [[nodiscard]] f32 missile_throw_force() override;
    [[nodiscard]] bool use_throw_randomness() override;
    void throw_target(const Fvector& position, CObject* throw_ignore_object = nullptr);
    void throw_target(const Fvector& position, u32 const vertex_id, CObject* throw_ignore_object = nullptr);
    IC const Fvector& throw_target() const;
    void update_throw_params();
    void on_throw_completed();
    IC const u32& last_throw_time() const;

#ifdef DEBUG
public:
    void dbg_draw_vision();
    void dbg_draw_visibility_rays();
#endif

    //////////////////////////////////////////////////////////////////////////
    // Critical Wounds
    //////////////////////////////////////////////////////////////////////////
private:
    void load_critical_wound_bones() override;
    [[nodiscard]] bool critical_wound_external_conditions_suitable() override;
    void critical_wounded_state_start() override;

    void fill_bones_body_parts(LPCSTR bone_id, const ECriticalWoundType& wound_type);

public:
    typedef xr_vector<float> CRITICAL_WOUND_WEIGHTS;

private:
    CRITICAL_WOUND_WEIGHTS m_critical_wound_weights;

public:
    bool critically_wounded();
    IC const CRITICAL_WOUND_WEIGHTS& critical_wound_weights() const;

private:
    bool can_cry_enemy_is_wounded() const;
    void on_critical_wound_initiator();
    void on_enemy_wounded_or_killed();
    void notify_on_wounded_or_killed(CObject* object);
    void notify_on_wounded_or_killed();
    void remove_critical_hit();
    //////////////////////////////////////////////////////////////////////////
private:
    bool m_registered_in_combat_on_migration{};

public:
    void on_before_change_team() override;
    void on_after_change_team() override;

private:
    bool m_sight_enabled_before_animation_controller;

public:
    void create_anim_mov_ctrl(CBlend* b) override;
    void destroy_anim_mov_ctrl() override;

private:
    bool m_can_select_items;

public:
    IC const u32& throw_time_interval() const;
    IC void throw_time_interval(const u32& value);

public:
    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CAI_Stalker);

add_to_type_list(CAI_Stalker);
#undef script_type_list
#define script_type_list save_type_list(CAI_Stalker)

#include "ai_stalker_inline.h"
