#pragma once

#include "holder_custom.h"
#include "shootingobject.h"
#include "physicsshellholder.h"
#include "hudsound.h"

class CCartridge;
class CCameraBase;

#define DESIRED_DIR 1

class CWeaponStatMgun : public CPhysicsShellHolder, public CHolderCustom, public CShootingObject
{
    RTTI_DECLARE_TYPEINFO(CWeaponStatMgun, CPhysicsShellHolder, CHolderCustom, CShootingObject);

private:
    typedef CPhysicsShellHolder inheritedPH;
    typedef CHolderCustom inheritedHolder;
    typedef CShootingObject inheritedShooting;

    CCameraBase* camera;
    //
    static void BoneCallbackX(CBoneInstance* B);
    static void BoneCallbackY(CBoneInstance* B);
    void SetBoneCallbacks();
    void ResetBoneCallbacks();
    // casts
public:
    virtual CHolderCustom* cast_holder_custom() { return this; }

    // general
public:
    CWeaponStatMgun();
    ~CWeaponStatMgun() override;

    virtual void Load(LPCSTR section);

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void net_Export(CSE_Abstract* E);

    virtual void UpdateCL();

    virtual void Hit(SHit* pHDS);

    // shooting
private:
    u16 m_rotate_x_bone, m_rotate_y_bone, m_fire_bone, m_camera_bone;
    float m_tgt_x_rot, m_tgt_y_rot, m_cur_x_rot, m_cur_y_rot, m_bind_x_rot, m_bind_y_rot;
    Fvector m_bind_x, m_bind_y;
    Fvector m_fire_dir, m_fire_pos;

    Fmatrix m_i_bind_x_xform, m_i_bind_y_xform, m_fire_bone_xform;
    Fvector2 m_lim_x_rot, m_lim_y_rot; // in bone space
    CCartridge* m_Ammo;
    Fvector2 m_dAngle;
    Fvector m_destEnemyDir;
    bool m_allow_fire;
    HUD_SOUND sndShot;
    float camRelaxSpeed;
    float camMaxAngle;

protected:
    void UpdateBarrelDir();
    virtual const Fvector& get_CurrentFirePoint();
    virtual const Fmatrix& get_ParticlesXFORM();
    virtual bool IsHudModeNow() { return false; }

    virtual void FireStart();
    virtual void FireEnd();
    virtual void UpdateFire();
    virtual void OnShot();
    void AddShotEffector();
    void RemoveShotEffector();
    void SetDesiredDir(float h, float p);

public:
    // HolderCustom
    [[nodiscard]] bool Use(const Fvector&, const Fvector&, const Fvector&) override { return !Owner(); }
    void OnMouseMove(int x, int y) override;
    void OnKeyboardPress(int dik) override;
    void OnKeyboardRelease(int dik) override;
    void OnKeyboardHold(int) override;
    virtual CInventory* GetInventory() { return nullptr; }
    void cam_Update(float, float = 90.0f) override;

    void renderable_Render(u32 context_id, IRenderable* root) override;

    virtual bool attach_Actor(CGameObject* actor);
    virtual void detach_Actor();
    virtual bool allowWeapon() const { return false; }
    virtual bool HUDView() const { return true; }
    virtual Fvector ExitPosition() { return Fvector{}; }

    virtual CCameraBase* Camera() { return camera; }

    virtual void Action(int id, u32 flags);
    virtual void SetParam(int id, Fvector2 val);
};
