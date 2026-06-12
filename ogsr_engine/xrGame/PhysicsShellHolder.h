#pragma once

#include "GameObject.h"
#include "ParticlesPlayer.h"

#include "../xr_3da/IPhysicsDefinitions.h"

class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class IDamageSource;
class CPHSkeleton;
class CCharacterPhysicsSupport;
class ICollisionDamageInfo;
class CIKLimbsController;
class CPHCapture;

class XR_NOVTABLE ICollisionHitCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ICollisionHitCallback);

public:
    ~ICollisionHitCallback() override = 0;

    virtual void call(CPhysicsShellHolder* obj, float min_cs, float max_cs, float& cs, float& hl, ICollisionDamageInfo* di) = 0;
};

inline ICollisionHitCallback::~ICollisionHitCallback() = default;

class CPhysicsShellHolder : public CGameObject, public CParticlesPlayer, public IObjectPhysicsCollision
{
    RTTI_DECLARE_TYPEINFO(CPhysicsShellHolder, CGameObject, CParticlesPlayer, IObjectPhysicsCollision);

public:
    bool b_sheduled;
    CPHSoundPlayer* m_ph_sound_player;

    void SheduleRegister()
    {
        if (!IsSheduled())
            shedule_register();
        b_sheduled = true;
    }
    void SheduleUnregister()
    {
        if (IsSheduled())
            shedule_unregister();
        b_sheduled = false;
    }
    IC bool IsSheduled() { return b_sheduled; }

public:
    typedef CGameObject inherited;

    CPhysicsShell* m_pPhysicsShell;

    CPhysicsShellHolder();
    ~CPhysicsShellHolder() override;

    IC CPhysicsShell*& PPhysicsShell() { return m_pPhysicsShell; }

    [[nodiscard]] CPhysicsShellHolder* PhysicsShellHolder() { return this; }
    [[nodiscard]] virtual CPHDestroyable* ph_destroyable() { return nullptr; }
    [[nodiscard]] virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver() { return nullptr; }
    [[nodiscard]] virtual CPHSkeleton* PHSkeleton() { return nullptr; }
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] virtual IDamageSource* cast_IDamageSource() { return nullptr; }
    [[nodiscard]] virtual CPHSoundPlayer* ph_sound_player() { return m_ph_sound_player; }
    [[nodiscard]] virtual CCharacterPhysicsSupport* character_physics_support() { return nullptr; }
    [[nodiscard]] virtual CCharacterPhysicsSupport* character_physics_support() const { return nullptr; }
    [[nodiscard]] virtual CIKLimbsController* character_ik_controller() { return nullptr; }
    [[nodiscard]] virtual ICollisionHitCallback* get_collision_hit_callback() { return nullptr; }
    virtual void set_collision_hit_callback(ICollisionHitCallback*) {}
    virtual void enable_notificate() {}

    [[nodiscard]] IPhysicsShell* physics_shell() const override;
    [[nodiscard]] IPhysicsElement* physics_character() const override;

    [[nodiscard]] const IObjectPhysicsCollision* physics_collision() override;

public:
    virtual void PHGetLinearVell(Fvector& velocity);
    virtual void PHSetLinearVell(Fvector& velocity);
    virtual void PHSetMaterial(LPCSTR m);
    virtual void PHSetMaterial(u16 m);
    void PHSaveState(NET_Packet& P);
    void PHLoadState(IReader& P);
    [[nodiscard]] virtual f32 GetMass();
    virtual void PHHit(SHit& H);
    void Hit(SHit* pHDS) override;
    ///////////////////////////////////////////////////////////////////////
    [[nodiscard]] virtual u16 PHGetSyncItemsNumber();
    [[nodiscard]] virtual CPHSynchronize* PHGetSyncItem(u16 item);
    virtual void PHUnFreeze();
    virtual void PHFreeze();
    [[nodiscard]] virtual f32 EffectiveGravity();
    ///////////////////////////////////////////////////////////////
    virtual void create_physic_shell();
    virtual void activate_physic_shell();
    virtual void setup_physic_shell();
    virtual void deactivate_physics_shell();

    tmc::task<void> net_Destroy() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    void Load(gsl::czstring section) override;

    void init();

    void OnChangeVisual() override;
    // для наследования CParticlesPlayer
    tmc::task<void> UpdateCL() override;
    void correct_spawn_pos();

public:
    [[nodiscard]] bool register_schedule() const override;
    bool ActorCanCapture() const;
    bool hasFixedBones() const;
    Fvector2 CollideSndDist() const;

public: // IPhysicsShellHolder
    CPHCapture* PHCapture();

private:
    Fvector m_overriden_activation_speed;
    bool m_activation_speed_is_overriden;

    Fvector2 m_collide_snd_dist;

public:
    virtual bool ActivationSpeedOverriden(Fvector& dest, bool clear_override);
    virtual void SetActivationSpeedOverride(Fvector const& speed);
};
