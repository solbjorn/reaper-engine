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
    [[nodiscard]] CHolderCustom* cast_holder_custom() override { return this; }

    // general
public:
    CWeaponStatMgun();
    ~CWeaponStatMgun() override;

    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    tmc::task<void> UpdateCL() override;

    void Hit(SHit* pHDS) override;

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
    [[nodiscard]] const Fvector& get_CurrentFirePoint() override;
    [[nodiscard]] const Fmatrix& get_ParticlesXFORM() override;
    [[nodiscard]] bool IsHudModeNow() override { return false; }

    void FireStart() override;
    void FireEnd() override;
    virtual void UpdateFire();
    virtual void OnShot();
    void AddShotEffector();
    void RemoveShotEffector();
    void SetDesiredDir(float h, float p);

public:
    // HolderCustom
    [[nodiscard]] bool Use(const Fvector&, const Fvector&, const Fvector&) override { return !Owner(); }
    void OnMouseMove(int x, int y) override;
    void OnKeyboardPress(EGameActions cmd) override;
    void OnKeyboardRelease(EGameActions cmd) override;
    void OnKeyboardHold(EGameActions) override;
    [[nodiscard]] CInventory* GetInventory() override { return nullptr; }
    tmc::task<void> cam_Update(f32, f32 = 90.0f) override;

    void renderable_Render(u32 context_id, IRenderable* root) override;

    [[nodiscard]] bool attach_Actor(CGameObject* actor) override;
    void detach_Actor() override;
    [[nodiscard]] bool allowWeapon() const override { return false; }
    [[nodiscard]] bool HUDView() const override { return true; }
    [[nodiscard]] Fvector3 ExitPosition() override { return Fvector3{}; }

    [[nodiscard]] CCameraBase* Camera() override { return camera; }

    void Action(EGameActions id, u32 flags) override;
    void SetParam(s32 id, Fvector2 val) override;
};
