#pragma once

class CDestroyablePhysicsObject : public CPhysicObject, public CPHDestroyable, public CPHCollisionDamageReceiver, public CHitImmunity, public CDamageManager
{
    RTTI_DECLARE_TYPEINFO(CDestroyablePhysicsObject, CPhysicObject, CPHDestroyable, CPHCollisionDamageReceiver, CHitImmunity, CDamageManager);

public:
    typedef CPhysicObject inherited;
    float m_fHealth;
    ref_sound m_destroy_sound;
    shared_str m_destroy_particles;

    CDestroyablePhysicsObject();
    virtual ~CDestroyablePhysicsObject();
    virtual CPhysicsShellHolder* PPhysicsShellHolder();
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void Hit(SHit* pHDS);
    virtual void InitServerObject(CSE_Abstract* D);
    virtual CPHCollisionDamageReceiver* PHCollisionDamageReceiver() { return static_cast<CPHCollisionDamageReceiver*>(this); }
    virtual DLL_Pure* _construct();
    virtual CPhysicsShellHolder* cast_physics_shell_holder() { return this; }
    virtual CParticlesPlayer* cast_particles_player() { return this; }
    virtual CPHDestroyable* ph_destroyable() { return this; }
    virtual void shedule_Update(u32 dt);
    virtual bool CanRemoveObject();
    virtual void OnChangeVisual();

protected:
    void Destroy();
};
