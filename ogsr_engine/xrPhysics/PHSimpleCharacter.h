#pragma once

#include "PHCharacter.h"
#include "Physics.h"
#include "MathUtils.h"
#include "ElevatorState.h"
#include "IColisiondamageInfo.h"
#include "../xr_3da/GameMtlLib.h"

namespace ALife
{
enum EHitType : u32;
}

#ifdef DEBUG
#include "PHDebug.h"
#endif

class CPHSimpleCharacter : public CPHCharacter, ICollisionDamageInfo
{
    RTTI_DECLARE_TYPEINFO(CPHSimpleCharacter, CPHCharacter, ICollisionDamageInfo);

public:
    friend class CScriptActor;

private:
    collide::rq_results RQR;

protected:
    CElevatorState m_elevator_state;
    ////////////////////////////damage////////////////////////////////////////
    struct SCollisionDamageInfo
    {
        SCollisionDamageInfo();
        void Construct();
        float ContactVelocity() const;
        void HitDir(Fvector& dir) const;
        IC const Fvector& HitPos() const { return cast_fv(m_damege_contact.geom.pos); }
        void Reinit();
        dContact m_damege_contact;
        ICollisionHitCallback* m_hit_callback;
        u16 m_obj_id;
        float m_dmc_signum;
        enum
        {
            ctStatic,
            ctObject
        } m_dmc_type;
        bool is_initiated;
        mutable float m_contact_velocity;
    };
    SCollisionDamageInfo m_collision_damage_info{};
    /////////////////////////// callback
    ObjectContactCallbackFun* m_object_contact_callback{};
    ////////////////////////// geometry
    Fvector m_last_move;
    dGeomID m_geom_shell{};
    dGeomID m_wheel{};
    dGeomID m_hat{};
    dGeomID m_cap{};

    dGeomID m_hat_transform{};
    dGeomID m_wheel_transform{};
    dGeomID m_shell_transform{};
    dGeomID m_cap_transform{};

    dSpaceID m_space{};

    dReal m_radius;
    dReal m_cyl_hight;
    ///////////////////////////////////
    dJointID m_capture_joint{};
    dJointFeedback m_capture_joint_feedback;
    ////////////////////////// movement
    dVector3 m_control_force;
    Fvector m_acceleration{};
    Fvector m_cam_dir;
    dVector3 m_wall_contact_normal;
    dVector3 m_ground_contact_normal;
    dVector3 m_clamb_depart_position;
    dVector3 m_depart_position;
    dVector3 m_wall_contact_position;
    dVector3 m_ground_contact_position;
    dReal jump_up_velocity; //=6.0f;//5.6f;
    dReal m_collision_damage_factor{1.f};
    dReal m_max_velocity;

    float m_air_control_factor{};

    dVector3 m_jump_depart_position;
    dVector3 m_death_position;
    Fvector m_jump_accel;

    // movement state
    bool is_contact{};
    bool was_contact{};
    bool b_depart{};
    bool b_meet{};
    bool b_side_contact{};
    bool b_was_side_contact{};
    bool b_any_contacts{};
    bool b_air_contact_state;

    bool b_valide_ground_contact{};
    bool b_valide_wall_contact{};
    bool b_on_object{};
    bool b_was_on_object;
    bool b_on_ground;
    bool b_lose_ground{};
    bool b_collision_restrictor_touch{};
    u32 m_contact_count;

    bool is_control{};
    bool b_meet_control{};
    bool b_lose_control{};
    bool was_control;
    bool b_stop_control;
    bool b_depart_control;
    bool b_jump{};
    bool b_jumping{};
    bool b_clamb_jump{};
    bool b_external_impulse{};
    u64 m_ext_impuls_stop_step{std::numeric_limits<u64>::max()};
    Fvector m_ext_imulse{};
    bool b_death_pos{};
    bool b_foot_mtl_check{};
    dReal m_friction_factor{1.f};

public:
    CPHSimpleCharacter();
    ~CPHSimpleCharacter() override { Destroy(); }

    /////////////////CPHObject//////////////////////////////////////////////
    void PhDataUpdate(dReal step) override;
    void PhTune(dReal step) override;
    void InitContact(dContact* c, bool& do_collide, u16, u16) override;
    [[nodiscard]] dSpaceID dSpace() override { return m_space; }
    [[nodiscard]] dGeomID dSpacedGeom() override { return (dGeomID)m_space; }
    void get_spatial_params() override;
    IC dReal get_jump_up_velocity() { return jump_up_velocity; }

    /////////////////CPHCharacter////////////////////////////////////////////
    // update

    // Check state
    [[nodiscard]] bool ContactWas() override
    {
        if (b_meet_control)
        {
            b_meet_control = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    [[nodiscard]] EEnvironment CheckInvironment() override;
    void GroundNormal(Fvector& norm) override;
    [[nodiscard]] const ICollisionDamageInfo* CollisionDamageInfo() const override { return this; }
    [[nodiscard]] ICollisionDamageInfo* CollisionDamageInfo() override { return this; }

private:
    [[nodiscard]] f32 ContactVelocity() const override { return m_collision_damage_info.ContactVelocity(); }
    void HitDir(Fvector& dir) const override { return m_collision_damage_info.HitDir(dir); }
    [[nodiscard]] const Fvector& HitPos() const override { return m_collision_damage_info.HitPos(); }
    [[nodiscard]] u16 DamageInitiatorID() const override;
    [[nodiscard]] CObject* DamageInitiator() const override;
    [[nodiscard]] ALife::EHitType HitType() const override;
    void SetInitiated() override;
    [[nodiscard]] bool IsInitiated() const override;
    [[nodiscard]] bool GetAndResetInitiated() override;
    [[nodiscard]] ICollisionHitCallback* HitCallback() const override;
    void Reinit() override { m_collision_damage_info.Reinit(); }

public:
    // Creating
    void Create(dVector3 sizes) override;
    void Destroy() override;
    void Disable() override;
    void EnableObject(CPHObject* obj) override;
    void Enable() override;
    void SetBox(const dVector3& sizes) override;
    [[nodiscard]] bool UpdateRestrictionType(CPHCharacter* ach) override;
    // get-set
    void SetObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void SetObjectContactCallbackData(void* data) override;
    void SetWheelContactCallback(ObjectContactCallbackFun* callback) override;

private:
    void RemoveObjectContactCallback(ObjectContactCallbackFun* callback);
    void AddObjectContactCallback(ObjectContactCallbackFun* callback);
    static void TestRestrictorContactCallbackFun(bool& do_colide, bool bo1, dContact& c, SGameMtl*, SGameMtl*);

public:
    [[nodiscard]] ObjectContactCallbackFun* ObjectContactCallBack() override;
    void SetStaticContactCallBack(ContactCallbackFun* calback);
    void SwitchOFFInitContact() override;
    void SwitchInInitContact() override;
    void SetAcceleration(Fvector accel) override;
    [[nodiscard]] Fvector GetAcceleration() override { return m_acceleration; }
    void SetCamDir(const Fvector& cam_dir) override;
    [[nodiscard]] const Fvector& CamDir() const override { return m_cam_dir; }
    void SetMaterial(u16 material) override;
    void SetPosition(Fvector pos) override;
    void GetVelocity(Fvector& vvel) override;
    void GetSmothedVelocity(Fvector& vvel) override;
    void SetVelocity(Fvector vel) override;
    void SetAirControlFactor(f32 factor) override { m_air_control_factor = factor; }
    void SetElevator(CClimableObject* climable) override { m_elevator_state.SetElevator(climable); }
    [[nodiscard]] CElevatorState* ElevatorState() override;
    void SetCollisionDamageFactor(f32 f) override { m_collision_damage_factor = f; }
    void GetPosition(Fvector& vpos) override;
    virtual void GetPreviousPosition(Fvector& pos);
    [[nodiscard]] f32 FootRadius() override;
    void DeathPosition(Fvector& deathPos) override;
    void IPosition(Fvector& pos) override;
    [[nodiscard]] u16 ContactBone() override;
    void ApplyImpulse(const Fvector& dir, const dReal P) override;
    void ApplyForce(const Fvector& force) override;
    void ApplyForce(const Fvector& dir, f32 force) override;
    void ApplyForce(f32 x, f32 y, f32 z) override;
    void AddControlVel(const Fvector& vel) override;
    void SetMaximumVelocity(dReal vel) override { m_max_velocity = vel; }
    [[nodiscard]] dReal GetMaximumVelocity() override { return m_max_velocity; }
    void SetJupmUpVelocity(dReal velocity) override { jump_up_velocity = velocity; }
    [[nodiscard]] bool JumpState() override { return b_jumping || b_jump; }
    [[nodiscard]] const Fvector& ControlAccel() const override { return m_acceleration; }
    [[nodiscard]] bool TouchRestrictor(ERestrictionType rttype) override;
    [[nodiscard]] f32& FrictionFactor() override { return m_friction_factor; }
    void SetMas(dReal mass) override;
    [[nodiscard]] f32 Mass() override { return m_mass; }
    void SetPhysicsRefObject(CPhysicsShellHolder* ref_object) override;

    virtual void CaptureObject(dBodyID body, const dReal* anchor);
    virtual void CapturedSetPosition(const dReal* position);
    virtual void doCaptureExist(bool& do_exist);

    void get_State(SPHNetState& state) override;
    void set_State(const SPHNetState& state) override;
    virtual void ValidateWalkOn();
    bool ValidateWalkOnMesh();
    bool ValidateWalkOnObject();

private:
    void CheckCaptureJoint();
    void ApplyAcceleration();

    u16 RetriveContactBone();
    void SafeAndLimitVelocity();
    void UpdateStaticDamage(dContact* c, SGameMtl* tri_material, bool bo1);
    void UpdateDynamicDamage(dContact* c, u16 obj_material_idx, dBodyID b, bool bo1);
    IC void FootProcess(dContact* c, bool& do_collide, bool bo);
    IC void foot_material_update(u16 tri_material, u16 foot_material_idx);
    static void TestPathCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);

private:
    void step(f32 dt) override { CPHObject::step(dt); }

#ifdef DEBUG
public:
    virtual void OnRender();
#endif
};

constexpr inline dReal def_spring_rate{0.5f};
constexpr inline dReal def_dumping_rate{20.1f};
