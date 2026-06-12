// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HangingLampH
#define HangingLampH

#include "gameobject.h"
#include "physicsshellholder.h"
#include "PHSkeleton.h"
#include "script_export_space.h"

// refs
class CLAItem;
class CPhysicsElement;
class CSE_ALifeObjectHangingLamp;
class CPHElement;

class CHangingLamp : public CPhysicsShellHolder, public CPHSkeleton
{
    RTTI_DECLARE_TYPEINFO(CHangingLamp, CPhysicsShellHolder, CPHSkeleton);

public:
    typedef CPhysicsShellHolder inherited;

private:
    ref_light light_render;
    ref_light light_ambient;
    ref_glow glow_render;
    CLAItem* lanim;

    u16 light_bone;
    u16 ambient_bone;
    float ambient_power;

    float fHealth;
    float fBrightness;
    bool lights_turned_on{};

    void CreateBody(CSE_ALifeObjectHangingLamp* lamp);
    void Init();
    void RespawnInit();
    bool Alive() { return fHealth > 0.f; }

public:
    CHangingLamp();
    ~CHangingLamp() override;

    void TurnOn();
    void TurnOff();
    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    tmc::task<void> shedule_Update(u32 dt) override; // Called by sheduler
    tmc::task<void> UpdateCL() override; // Called each frame, so no need for dt

    void SpawnInitPhysics(CSE_Abstract* D) override;
    [[nodiscard]] CPhysicsShellHolder* PPhysicsShellHolder() override { return PhysicsShellHolder(); }
    void CopySpawnInit() override;
    void net_Save(NET_Packet& P) override;
    [[nodiscard]] BOOL net_SaveRelevant() override;

    [[nodiscard]] BOOL renderable_ShadowGenerate() override { return TRUE; }
    [[nodiscard]] BOOL renderable_ShadowReceive() override { return TRUE; }

    void Hit(SHit* pHDS) override;
    void net_Export(CSE_Abstract*) override;
    [[nodiscard]] BOOL UsedAI_Locations() override;

    void Center(Fvector& C) const override;
    [[nodiscard]] f32 Radius() const override;

    void SetLSFParams(float, float, float);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CHangingLamp);

add_to_type_list(CHangingLamp);
#undef script_type_list
#define script_type_list save_type_list(CHangingLamp)

#endif // HangingLampH
