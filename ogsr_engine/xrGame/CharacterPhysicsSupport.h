#pragma once

#include "alife_space.h"
#include "PHSkeleton.h"
#include "entity_alive.h"
#include "PHSoundPlayer.h"
#include "PHDestroyable.h"
#include "character_hit_animations.h"
#include "PHMovementControl.h"
#include "death_anims.h"
#include "physics_shell_animated.h"

class CPhysicsShell;
class CIKLimbsController;
class interactive_motion;

class CCharacterPhysicsSupport : public CPHSkeleton, public CPHDestroyable
{
    RTTI_DECLARE_TYPEINFO(CCharacterPhysicsSupport, CPHSkeleton, CPHDestroyable);

public:
    enum EType
    {
        etActor,
        etStalker,
        etBitting
    };

    enum EState
    {
        esDead,
        esAlive,
        esRemoved
    };

private:
    EType m_eType;
    EState m_eState;
    Flags8 m_flags;
    enum Fags
    {
        fl_death_anim_on = 1 << 0,
        fl_skeleton_in_shell = 1 << 1,
        fl_specific_bonce_demager = 1 << 2,
        fl_block_hit = 1 << 3,
    };

    struct animation_movement_state
    {
        bool active;
        bool character_exist;
        void init()
        {
            active = false;
            character_exist = false;
        }
        animation_movement_state() { init(); }
    } anim_mov_state;

    CEntityAlive& m_EntityAlife;
    Fmatrix& mXFORM;
    CPhysicsShell*& m_pPhysicsShell;
    CPhysicsShell* m_physics_skeleton;
    CPHMovementControl* m_PhysicMovementControl;
    CPHSoundPlayer m_ph_sound_player;
    CIKLimbsController* m_ik_controller;
    ICollisionHitCallback* m_collision_hit_callback;
    character_hit_animation_controller m_hit_animations;
    death_anims m_death_anims;

    physics_shell_animated* m_physics_shell_animated{};

    interactive_motion* m_interactive_motion;
    // skeleton modell(!share?)
    float skel_airr_lin_factor;
    float skel_airr_ang_factor;
    float hinge_force_factor1;
    float skel_fatal_impulse_factor;
    float skel_ddelay;
    float skel_remain_time;
    /////////////////////////////////////////////////
    // bool								b_death_anim_on ; bool								b_skeleton_in_shell
    // ;
    ///////////////////////////////////////////////////////////////////////////
    float m_shot_up_factor;
    float m_after_death_velocity_factor;
    float m_BonceDamageFactor;
    // gray_wolf>Переменные для поддержки изменяющегося трения у персонажей во время смерти
    float skeleton_skin_ddelay;
    float skeleton_skin_remain_time;
    float skeleton_skin_friction_start;
    float skeleton_skin_friction_end;
    float skeleton_skin_ddelay_after_wound;
    float skeleton_skin_remain_time_after_wound;
    bool m_was_wounded;
    float m_Pred_Time; // Для вычисления дельта времени между пересчётами сопротивления в джоинтах и коэффициента NPC
    float m_time_delta;
    float pelvis_factor_low_pose_detect;
    BOOL character_have_wounded_state;

    u32 m_physics_shell_animated_time_destroy{u32(-1)};

    // gray_wolf<
public:
    // gray_wolf>
    float m_curr_skin_friction_in_death;
    // gray_wolf<
    EType Type() { return m_eType; }
    EState STate() { return m_eState; }
    void SetState(EState astate) { m_eState = astate; }
    IC bool isDead() { return m_eState == esDead; }
    IC bool isAlive() { return !m_pPhysicsShell; }

protected:
    virtual void SpawnInitPhysics(CSE_Abstract* D);
    virtual CPhysicsShellHolder* PPhysicsShellHolder() { return m_EntityAlife.PhysicsShellHolder(); }

    virtual bool CanRemoveObject();

public:
    IC CPHMovementControl* movement() { return m_PhysicMovementControl; }
    virtual const Fvector MovementVelocity() { return m_PhysicMovementControl->GetVelocity(); }
    IC CPHSoundPlayer* ph_sound_player() { return &m_ph_sound_player; }
    IC CIKLimbsController* ik_controller() { return m_ik_controller; }
    void SetRemoved();
    bool IsRemoved() { return m_eState == esRemoved; }
    bool IsSpecificDamager() { return !!m_flags.test(fl_specific_bonce_demager); }
    float BonceDamageFactor() { return m_BonceDamageFactor; }
    void set_movement_position(const Fvector& pos);
    //////////////////base hierarchi methods///////////////////////////////////////////////////
    void CreateCharacter();
    void in_UpdateCL();
    void in_shedule_Update(u32 DT);
    void in_NetSpawn(CSE_Abstract* e);
    void in_NetDestroy();
    void in_NetRelcase(CObject* O);
    void in_Init();
    void in_Load(LPCSTR section);
    void in_Hit(SHit& H, bool is_killing = false);
    void in_NetSave(NET_Packet& P);
    void in_ChangeVisual();

    void destroy_imotion();

    void on_create_anim_mov_ctrl();
    void on_destroy_anim_mov_ctrl();
    void PHGetLinearVell(Fvector& velocity);
    ICollisionHitCallback* get_collision_hit_callback();
    void set_collision_hit_callback(ICollisionHitCallback* cc);
    /////////////////////////////////////////////////////////////////
    CCharacterPhysicsSupport& operator=(CCharacterPhysicsSupport& /**asup/**/) { R_ASSERT2(false, "Can not assign it"); }
    void SyncNetState();

    CCharacterPhysicsSupport(EType atype, CEntityAlive* aentity);
    virtual ~CCharacterPhysicsSupport();

    IC physics_shell_animated* animation_collision() { return m_physics_shell_animated; }
    IC const physics_shell_animated* animation_collision() const { return m_physics_shell_animated; }

    void create_animation_collision();
    void destroy_animation_collision();

private:
    void CreateSkeleton(CPhysicsShell*& pShell);
    void CreateSkeleton();
    void ActivateShell(CObject* who);
    void KillHit(SHit& H);
    static void DeathAnimCallback(CBlend* B);
    void CreateIKController();
    void DestroyIKController();
    void CollisionCorrectObjPos(const Fvector& start_from, bool character_create = false);
    void FlyTo(const Fvector& disp);
    void TestForWounded();
    IC void UpdateFrictionAndJointResistanse();
    IC void UpdateDeathAnims();
    IC void CalculateTimeDelta();
    IC bool DoCharacterShellCollide();

    void update_animation_collision();
};
