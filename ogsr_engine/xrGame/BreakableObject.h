// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BreakableObjectH
#define BreakableObjectH

#include "physicsshellholder.h"

class CPHStaticGeomShell;
struct dContact;
struct SGameMtl;

class CBreakableObject : public CPhysicsShellHolder
{
    RTTI_DECLARE_TYPEINFO(CBreakableObject, CPhysicsShellHolder);

public:
    typedef CPhysicsShellHolder inherited;

private:
    float m_max_frame_damage;
    static float m_damage_threshold;
    static float m_health_threshhold;
    static float m_immunity_factor;
    Fvector m_contact_damage_pos;
    Fvector m_contact_damage_dir;

    float fHealth;
    CPHStaticGeomShell* m_pUnbrokenObject;
    CPhysicsShell* m_Shell;
    static u32 m_remove_time;
    u32 m_break_time;
    bool b_resived_damage;
    bool bRemoved;

public:
    CBreakableObject();
    ~CBreakableObject() override;

    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override; // Called by sheduler
    tmc::task<void> UpdateCL() override;
    [[nodiscard]] BOOL renderable_ShadowGenerate() override { return FALSE; }
    [[nodiscard]] BOOL renderable_ShadowReceive() override { return TRUE; }

    void Hit(SHit* pHDS) override;

    void net_Export(CSE_Abstract*) override;
    [[nodiscard]] BOOL UsedAI_Locations() override;

private:
    void Init();
    void CreateUnbroken();
    void CreateBroken();
    void DestroyUnbroken();
    void ActivateBroken();
    void Split();
    void Break();
    void ApplyExplosion(const Fvector& dir, float impulse);
    void CheckHitBreak(float power, ALife::EHitType hit_type);
    void ProcessDamage();
    void SendDestroy();
    void enable_notificate() override;

    static void ObjectContactCallback(bool&, bool, dContact& c, SGameMtl*, SGameMtl*);
};

#endif // BreakableObjectH
