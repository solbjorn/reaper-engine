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
    virtual const Fvector& get_CurrentFirePoint() { return fire_pos; }
    virtual const Fmatrix& get_ParticlesXFORM();
    virtual bool IsHudModeNow() { return false; }

    //////////////////////////////////////////////////
    // непосредственно обработка стрельбы
    //////////////////////////////////////////////////
protected:
    virtual void FireStart();
    virtual void FireEnd();
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
    virtual CHolderCustom* cast_holder_custom() { return this; }
    virtual void Load(LPCSTR section);

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    virtual void net_Export(CSE_Abstract* E);

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;

    void renderable_Render(u32 context_id, IRenderable* root) override;

    virtual BOOL UsedAI_Locations() { return FALSE; }

    // control functions
    void OnMouseMove(int x, int y) override;
    void OnKeyboardPress(int dik) override;
    void OnKeyboardRelease(int dik) override;
    void OnKeyboardHold(int) override;

    virtual CInventory* GetInventory() { return nullptr; }

    void cam_Update(float, float = 90.0f) override;

    [[nodiscard]] bool Use(const Fvector&, const Fvector&, const Fvector&) override;
    virtual bool attach_Actor(CGameObject* actor);
    virtual void detach_Actor();
    virtual Fvector ExitPosition();
    virtual bool allowWeapon() const { return false; }
    virtual bool HUDView() const { return true; }

    virtual CCameraBase* Camera();
};

#endif // WeaponMountedH
