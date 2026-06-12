#pragma once

#include "PHCollisionDamageReceiver.h"
#include "PHDestroyable.h"
#include "PhysicObject.h"
#include "damage_manager.h"
#include "hit_immunity.h"

class CDestroyablePhysicsObject : public CPhysicObject, public CPHDestroyable, public CPHCollisionDamageReceiver, public CHitImmunity, public CDamageManager
{
    RTTI_DECLARE_TYPEINFO(CDestroyablePhysicsObject, CPhysicObject, CPHDestroyable, CPHCollisionDamageReceiver, CHitImmunity, CDamageManager);

public:
    typedef CPhysicObject inherited;

    f32 m_fHealth{1.0f};
    ref_sound m_destroy_sound;
    shared_str m_destroy_particles;

    CDestroyablePhysicsObject();
    ~CDestroyablePhysicsObject() override;

    [[nodiscard]] CPhysicsShellHolder* PPhysicsShellHolder() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void Hit(SHit* pHDS) override;
    void InitServerObject(CSE_Abstract* D) override;
    [[nodiscard]] CPHCollisionDamageReceiver* PHCollisionDamageReceiver() override { return static_cast<CPHCollisionDamageReceiver*>(this); }
    [[nodiscard]] DLL_Pure* _construct() override;
    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CParticlesPlayer* cast_particles_player() override { return this; }
    [[nodiscard]] CPHDestroyable* ph_destroyable() override { return this; }
    tmc::task<void> shedule_Update(u32 dt) override;
    [[nodiscard]] bool CanRemoveObject() override;
    void OnChangeVisual() override;

protected:
    void Destroy();
};
