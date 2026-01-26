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

    virtual CPhysicsShellHolder* PPhysicsShellHolder();
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    virtual void Hit(SHit* pHDS);
    virtual void InitServerObject(CSE_Abstract* D);
    virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver() { return static_cast<CPHCollisionDamageReceiver*>(this); }
    virtual DLL_Pure* _construct();
    virtual CPhysicsShellHolder* cast_physics_shell_holder() { return this; }
    virtual CParticlesPlayer* cast_particles_player() { return this; }
    virtual CPHDestroyable* ph_destroyable() { return this; }
    tmc::task<void> shedule_Update(u32 dt) override;
    virtual bool CanRemoveObject();
    virtual void OnChangeVisual();

protected:
    void Destroy();
};
