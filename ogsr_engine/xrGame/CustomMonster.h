// CustomMonster.h: interface for the CCustomMonster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "entity_alive.h"
#include "script_entity.h"
#include "..\xr_3da\feel_vision.h"
#include "..\xr_3da\feel_sound.h"
#include "..\xr_3da\feel_touch.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "associative_vector.h"
#include "ai\monsters\anomaly_detector.h"

namespace MonsterSpace
{
struct SBoneRotation;
}

class CMotionDef;
class IKinematicsAnimated;
class CMemoryManager;
class CItemManager;
class CEnemyManager;
class CDangerManager;
class CMovementManager;
class CSoundPlayer;
class CAI_Stalker;
class CDangerObject;
class CCustomMonsterScript;

class CCustomMonster : public CEntityAlive, public CScriptEntity, public Feel::Vision, public Feel::Sound, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CCustomMonster, CEntityAlive, CScriptEntity, Feel::Vision, Feel::Sound, Feel::Touch);

private:
    typedef CEntityAlive inherited;
    friend class CCustomMonsterScript;

private:
    CMemoryManager* m_memory_manager{};
    CMovementManager* m_movement_manager{};
    CSoundPlayer* m_sound_player{};

private:
    u32 m_client_update_delta;
    u32 m_last_client_update_time;

protected:
    struct SAnimState
    {
        MotionID fwd;
        MotionID back;
        MotionID ls;
        MotionID rs;

        void Create(IKinematicsAnimated* K, LPCSTR base);
    };

private:
    xr_vector<CLASS_ID> m_killer_clsids;

public:
    // Eyes
    Fmatrix eye_matrix;
    int eye_bone;
    float eye_fov;
    float eye_range;

    float m_fCurSpeed;

    u32 eye_pp_stage;
    u32 eye_pp_timestamp;
    Fvector m_tEyeShift;
    float m_fEyeShiftYaw;
    BOOL NET_WasExtrapolating;

    Fvector tWatchDirection;

    virtual void Think() = 0;

    float m_fTimeUpdateDelta;
    u32 m_dwLastUpdateTime;
    u32 m_current_update;
    //	Fmatrix				m_tServerTransform;

    u32 m_dwCurrentTime; // time updated in UpdateCL

    struct net_update
    {
        u32 dwTimeStamp; // server(game) timestamp
        float o_model; // model yaw
        SRotation o_torso; // torso in world coords
        Fvector p_pos; // in world coords
        float fHealth;

        // non-exported (temporal)

        net_update()
        {
            dwTimeStamp = 0;
            o_model = 0;
            o_torso.yaw = 0;
            o_torso.pitch = 0;
            p_pos.set(0, 0, 0);
            fHealth = 0.f;
        }
        void lerp(net_update& A, net_update& B, float f);
    };
    xr_deque<net_update> NET;
    net_update NET_Last;
    //------------------------------

    [[nodiscard]] BOOL feel_touch_on_contact(CObject*) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject*) override;
    // utils
    void mk_orientation(Fvector& dir, Fmatrix& mR);
    void mk_rotation(Fvector& dir, SRotation& R);

    // stream executors
    virtual void Exec_Action(float);
    virtual void Exec_Look(f32 dt);
    tmc::task<void> Exec_Visibility();
    void eye_pp_s0();
    void eye_pp_s1();
    void eye_pp_s2();

    virtual tmc::task<void> UpdateCamera();

public:
    CCustomMonster();
    ~CCustomMonster() override;

public:
    [[nodiscard]] CEntityAlive* cast_entity_alive() override { return this; }
    [[nodiscard]] CEntity* cast_entity() override { return this; }

public:
    [[nodiscard]] DLL_Pure* _construct() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> Die(CObject* who) override;

    virtual void HitSignal(float, Fvector&, CObject*);
    void g_WeaponBones(s32&, s32&, s32&) override {}
    tmc::task<void> shedule_Update(u32 DT) override;
    tmc::task<void> UpdateCL() override;

    void net_Export(CSE_Abstract*) override;
    void net_Relcase(CObject* O) override;

    virtual void SelectAnimation(const Fvector& _view, const Fvector& _move, float speed) = 0;

    [[nodiscard]] virtual bool is_base_monster_with_enemy() { return false; }

    // debug
#ifdef DEBUG
    virtual void OnRender();
    void OnHUDDraw(ctx_id_t, CCustomHUD*, IRenderable*) override;
#endif

    virtual bool bfExecMovement() { return false; }

    IC bool angle_lerp_bounds(float& a, float b, float c, float d);
    IC void vfNormalizeSafe(Fvector& Vector);

public:
    [[nodiscard]] f32 ffGetFov() const override { return eye_fov; }
    [[nodiscard]] f32 ffGetRange() const override { return eye_range; }
    void set_fov(float new_fov);
    void set_range(float new_range);
    virtual BOOL feel_visible_isRelevant(CObject* O);
    [[nodiscard]] Feel::Sound* dcast_FeelSound() override { return this; }
    void Hit(SHit* pHDS) override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    tmc::task<void> net_Destroy() override;
    [[nodiscard]] BOOL UsedAI_Locations() override;
    ///////////////////////////////////////////////////////////////////////
    [[nodiscard]] u16 PHGetSyncItemsNumber() override { return inherited::PHGetSyncItemsNumber(); }
    [[nodiscard]] CPHSynchronize* PHGetSyncItem(u16 item) override { return inherited::PHGetSyncItem(item); }
    void PHUnFreeze() override { return inherited::PHUnFreeze(); }
    void PHFreeze() override { return inherited::PHFreeze(); }
    ///////////////////////////////////////////////////////////////////////

    void Load(gsl::czstring section) override;
    void reinit() override;
    void reload(gsl::czstring section) override;
    [[nodiscard]] const SRotation Orientation() const override;
    virtual float get_custom_pitch_speed(float def_speed) { return def_speed; }
    [[nodiscard]] bool human_being() const override { return false; }
    virtual void PitchCorrection();

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;
    [[nodiscard]] BOOL net_SaveRelevant() override { return inherited::net_SaveRelevant(); }

    [[nodiscard]] virtual const MonsterSpace::SBoneRotation& head_orientation() const;

    virtual void UpdatePositionAnimation();
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] CCustomMonster* cast_custom_monster() override { return this; }
    [[nodiscard]] CScriptEntity* cast_script_entity() override { return this; }

    void load_killer_clsids(LPCSTR section);
    bool is_special_killer(CObject* obj);

    IC CMemoryManager& memory() const;
    [[nodiscard]] f32 feel_vision_mtl_transp(CObject* O, u32 element) override;
    void feel_sound_new(CObject* who, s32 type, CSound_UserDataPtr user_data, const Fvector3& Position, f32 power, f32 time_to_stop) override;

    [[nodiscard]] virtual bool useful(const CItemManager*, const CGameObject* object) const;
    [[nodiscard]] virtual float evaluate(const CItemManager*, const CGameObject* object) const;
    [[nodiscard]] virtual bool useful(const CEnemyManager*, const CEntityAlive* object) const;
    [[nodiscard]] virtual float evaluate(const CEnemyManager*, const CEntityAlive* object) const;
    [[nodiscard]] virtual bool useful(const CDangerManager*, const CDangerObject& object) const;
    [[nodiscard]] virtual float evaluate(const CDangerManager*, const CDangerObject& object) const;

protected:
    float m_panic_threshold;

public:
    IC float panic_threshold() const;

private:
    CSound_UserDataVisitor* m_sound_user_data_visitor{};

protected:
    [[nodiscard]] virtual CSound_UserDataVisitor* create_sound_visitor();
    [[nodiscard]] virtual CMemoryManager* create_memory_manager();
    [[nodiscard]] virtual CMovementManager* create_movement_manager();

public:
    IC CMovementManager& movement() const;
    IC CSoundPlayer& sound() const;
    IC CSound_UserDataVisitor* sound_user_data_visitor() const;

protected:
    float m_far_plane_factor;
    float m_fog_density_factor;

public:
    virtual void update_range_fov(f32& new_range, f32& new_fov, f32 start_range, f32 start_fov);

public:
    tmc::task<void> update_sound_player();
    virtual void on_restrictions_change();
    [[nodiscard]] gsl::czstring visual_name(CSE_Abstract* server_entity) override;

private:
    bool m_already_dead{};

public:
    IC const bool& already_dead() const { return m_already_dead; }
    [[nodiscard]] bool use_simplified_visual() const override { return already_dead(); }
    virtual void on_enemy_change(const CEntityAlive*);
    [[nodiscard]] CVisualMemoryManager* visual_memory() const override;

public:
    IC float client_update_fdelta() const;
    IC const u32& client_update_delta() const;
    IC const u32& last_client_update_time() const;

    //////////////////////////////////////////////////////////////////////////
    // Critical Wounds
    //////////////////////////////////////////////////////////////////////////
public:
    typedef u32 CriticalWoundType;

private:
    typedef associative_vector<u16, CriticalWoundType> BODY_PART;

protected:
    u32 m_last_hit_time;
    float m_critical_wound_threshold;
    float m_critical_wound_decrease_quant;
    float m_critical_wound_accumulator;
    CriticalWoundType m_critical_wound_type;
    BODY_PART m_bones_body_parts;

protected:
    virtual void load_critical_wound_bones() {}

    [[nodiscard]] virtual bool critical_wound_external_conditions_suitable() { return true; }
    virtual void critical_wounded_state_start() {}

    bool update_critical_wounded(const u16& bone_id, const float& power);

public:
    IC void critical_wounded_state_stop();

public:
    IC bool critically_wounded();
    IC const u32& critical_wound_type() const;

    //////////////////////////////////////////////////////////////////////////
private:
    bool m_invulnerable{};

public:
    IC void invulnerable(const bool& invulnerable);
    IC bool invulnerable() const;

protected:
    bool m_update_rotation_on_frame;

private:
    bool m_movement_enabled_before_animation_controller;

public:
    void create_anim_mov_ctrl(CBlend* b) override;
    void destroy_anim_mov_ctrl() override;

    bool m_visible_for_zones{true};

    [[nodiscard]] bool IsVisibleForZones() override { return m_visible_for_zones; }
    void ForceTransform(const Fmatrix& m) override;

    // -----------------------------------------------------------------------------
    // Anomaly Detector
private:
    CAnomalyDetector* m_anomaly_detector;

public:
    CAnomalyDetector& anomaly_detector() { return (*m_anomaly_detector); }
    // -----------------------------------------------------------------------------
};
XR_SOL_BASE_CLASSES(CCustomMonster);

// console_commands.cpp
extern int g_AI_inactive_time;

#include "custommonster_inline.h"
