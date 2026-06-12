#ifndef WeaponMountedH
#define WeaponMountedH

#include "holder_custom.h"
#include "shootingobject.h"

#include "hudsound.h"
#include "weaponammo.h"
#include "physicsshellholder.h"

class CWeaponMounted : public CPhysicsShellHolder, public CHolderCustom, public CShootingObject
{
    RTTI_DECLARE_TYPEINFO(CWeaponMounted, CPhysicsShellHolder, CHolderCustom, CShootingObject);

private:
    //////////////////////////////////////////////////////////////////////////
    //  General
    //////////////////////////////////////////////////////////////////////////
    typedef CPhysicsShellHolder inherited;
    CCameraBase* camera;
    u16 fire_bone;
    u16 actor_bone;
    u16 rotate_x_bone;
    u16 rotate_y_bone;
    u16 camera_bone;

    Fvector fire_pos, fire_dir;
    Fmatrix fire_bone_xform;
    Fvector2 m_dAngle;
    static void BoneCallbackX(CBoneInstance* B);
    static void BoneCallbackY(CBoneInstance* B);

public:
    CWeaponMounted();
    ~CWeaponMounted() override;

    // for shooting object
    [[nodiscard]] const Fvector3& get_CurrentFirePoint() override { return fire_pos; }
    [[nodiscard]] const Fmatrix& get_ParticlesXFORM() override;
    [[nodiscard]] bool IsHudModeNow() override { return false; }

    //////////////////////////////////////////////////
    // непосредственно обработка стрельбы
    //////////////////////////////////////////////////
protected:
    void FireStart() override;
    void FireEnd() override;
    virtual void UpdateFire();
    virtual void OnShot();
    void AddShotEffector();
    void RemoveShotEffector();

protected:
    shared_str m_sAmmoType;
    CCartridge m_CurrentAmmo;

    // звук стрельбы
    HUD_SOUND sndShot;

    // для отдачи
    float camRelaxSpeed;
    float camMaxAngle;

    /////////////////////////////////////////////////
    // Generic
    /////////////////////////////////////////////////
public:
    [[nodiscard]] CHolderCustom* cast_holder_custom() override { return this; }
    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;

    void renderable_Render(u32 context_id, IRenderable* root) override;

    [[nodiscard]] BOOL UsedAI_Locations() override { return FALSE; }

    // control functions
    void OnMouseMove(int x, int y) override;
    void OnKeyboardPress(EGameActions cmd) override;
    void OnKeyboardRelease(EGameActions cmd) override;
    void OnKeyboardHold(EGameActions) override;

    [[nodiscard]] CInventory* GetInventory() override { return nullptr; }

    tmc::task<void> cam_Update(f32, f32 = 90.0f) override;

    [[nodiscard]] bool Use(const Fvector3&, const Fvector3&, const Fvector3&) override;
    [[nodiscard]] bool attach_Actor(CGameObject* actor) override;
    void detach_Actor() override;
    [[nodiscard]] Fvector ExitPosition() override;
    [[nodiscard]] bool allowWeapon() const override { return false; }
    [[nodiscard]] bool HUDView() const override { return true; }

    [[nodiscard]] CCameraBase* Camera() override;
};

#endif // WeaponMountedH
