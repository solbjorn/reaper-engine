#pragma once

#include "PHObject.h"
#include "PHInterpolation.h"
#include "PHSynchronize.h"
#include "PHDisabling.h"
#include "PhysicsShell.h"

class CPhysicsShellHolder;
class CClimableObject;
class CGameObject;
class ICollisionDamageInfo;
class CElevatorState;
class CPHActorCharacter;
class CPHAICharacter;

/* static*/ enum EEnvironment
{
    peOnGround,
    peAtWall,
    peInAir
};

class XR_NOVTABLE CPHCharacter : public CPHObject,
                                 public CPHSynchronize,
                                 public CPHDisablingTranslational,
                                 public IPhysicsElement
#ifdef DEBUG
    ,
                                 public pureRender
#endif
{
    RTTI_DECLARE_TYPEINFO(CPHCharacter, CPHObject, CPHSynchronize, CPHDisablingTranslational, IPhysicsElement
#ifdef DEBUG
                          ,
                          pureRender
#endif
    );

public:
    u64 m_creation_step;
    bool b_exist{};

protected:
    ////////////////////////// dynamic

    CPHInterpolation m_body_interpolation;
    dBodyID m_body;
    CPhysicsShellHolder* m_phys_ref_object{};

    dReal m_mass{};
    bool was_enabled_before_freeze{};

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    u16* p_lastMaterialIDX;
    u16 lastMaterialIDX;
    ///////////////////////////////////////////////////////////////////////////
    dVector3 m_safe_velocity;
    dVector3 m_safe_position{};
    dReal m_mean_y;

public:
    enum ERestrictionType
    {

        rtStalker = 0,
        rtStalkerSmall,
        rtMonsterMedium,
        rtNone,
        rtActor
    };

protected:
    ERestrictionType m_new_restriction_type;
    ERestrictionType m_restriction_type;
    bool b_actor_movable;

    bool b_in_touch_resrtrictor;
    float m_current_object_radius;

public:
    virtual ECastType CastType() { return CPHObject::tpCharacter; }
    virtual CPHActorCharacter* CastActorCharacter() { return nullptr; }
    virtual CPHAICharacter* CastAICharacter() { return nullptr; }
    ERestrictionType RestrictionType() { return m_restriction_type; }
    void SetNewRestrictionType(ERestrictionType rt) { m_new_restriction_type = rt; }
    void SetRestrictionType(ERestrictionType rt) { m_new_restriction_type = m_restriction_type = rt; }
    void SetObjectRadius(float R) { m_current_object_radius = R; }
    float ObjectRadius() { return m_current_object_radius; }
    virtual void ChooseRestrictionType(ERestrictionType, float, CPHCharacter*) {}
    virtual bool UpdateRestrictionType(CPHCharacter* ach) = 0;
    virtual void FreezeContent();
    virtual void UnFreezeContent();
    virtual dBodyID get_body() { return m_body; }
    virtual dSpaceID dSpace() = 0;
    virtual void Disable();
    virtual void ReEnable() {}
    virtual void Enable(); //!!
    virtual void SwitchOFFInitContact() = 0;
    virtual void SwitchInInitContact() = 0;
    bool IsEnabled()
    {
        if (!b_exist)
            return false;
        return !!dBodyIsEnabled(m_body);
    }
    bool ActorMovable() { return b_actor_movable; }
    void SetActorMovable(bool v) { b_actor_movable = v; }
    virtual const ICollisionDamageInfo* CollisionDamageInfo() const = 0;
    virtual ICollisionDamageInfo* CollisionDamageInfo() = 0;
    virtual void Reinit() = 0;
    void SetPLastMaterialIDX(u16* p) { p_lastMaterialIDX = p; }
    const u16& LastMaterialIDX() const { return *p_lastMaterialIDX; }
    virtual bool TouchRestrictor(ERestrictionType rttype) = 0;
    virtual void SetElevator(CClimableObject*) {}
    virtual void SetMaterial(u16 material) = 0;
    virtual void SetMaximumVelocity(dReal) {} //!!
    virtual dReal GetMaximumVelocity() { return 0; }
    virtual void SetJupmUpVelocity(dReal) {} //!!
    virtual void IPosition(Fvector&) {}
    virtual u16 ContactBone() { return 0; }
    virtual void DeathPosition(Fvector&) {}
    virtual void ApplyImpulse(const Fvector&, const dReal) {}
    virtual void ApplyForce(const Fvector& force) = 0;
    virtual void ApplyForce(const Fvector& dir, float force) = 0;
    virtual void ApplyForce(float x, float y, float z) = 0;
    virtual void AddControlVel(const Fvector& vel) = 0;
    virtual void Jump(const Fvector& jump_velocity) = 0;
    virtual bool JumpState() = 0;
    virtual EEnvironment CheckInvironment() = 0;
    virtual bool ContactWas() = 0;
    virtual void GroundNormal(Fvector& norm) = 0;
    virtual void Create(dVector3) = 0;
    virtual void Destroy(void) = 0;
    virtual void SetBox(const dVector3& sizes) = 0;
    virtual void SetAcceleration(Fvector accel) = 0;
    virtual void SetForcedPhysicsControl(bool) {}
    virtual bool ForcedPhysicsControl() { return false; }
    virtual void SetCamDir(const Fvector& cam_dir) = 0;
    virtual const Fvector& CamDir() const = 0;
    virtual Fvector GetAcceleration() = 0;
    virtual void SetPosition(Fvector pos) = 0;
    virtual void SetApplyGravity(BOOL flag) { dBodySetGravityMode(m_body, flag); }
    virtual void SetObjectContactCallbackData(void* callback) = 0;
    virtual void SetObjectContactCallback(ObjectContactCallbackFun* callback) = 0;
    virtual void SetWheelContactCallback(ObjectContactCallbackFun* callback) = 0;
    virtual ObjectContactCallbackFun* ObjectContactCallBack() { return nullptr; }
    virtual void GetVelocity(Fvector& vvel) = 0;
    virtual void GetSavedVelocity(Fvector& vvel);
    virtual void GetSmothedVelocity(Fvector& vvel) = 0;
    virtual void SetVelocity(Fvector vel) = 0;
    virtual void SetAirControlFactor(float factor) = 0;
    virtual void GetPosition(Fvector& vpos) = 0;
    virtual void GetFootCenter(Fvector& vpos) { vpos.set(*(const Fvector*)dBodyGetPosition(m_body)); }
    virtual void SetMas(dReal mass) = 0;
    virtual void SetCollisionDamageFactor(float f) = 0;
    virtual float Mass() = 0;
    virtual void SetPhysicsRefObject(CPhysicsShellHolder* ref_object) = 0;
    virtual CPhysicsShellHolder* PhysicsRefObject() { return m_phys_ref_object; }

    // AICharacter
    virtual void GetDesiredPosition(Fvector&) {}
    virtual void SetDesiredPosition(const Fvector&) {}
    virtual bool TryPosition(Fvector) { return false; }
    virtual bool TouchBorder() { return false; }
    virtual void getForce(Fvector& force);
    virtual void setForce(const Fvector& force);
    virtual float FootRadius() = 0;
    virtual void get_State(SPHNetState& state);
    virtual void set_State(const SPHNetState& state);
    virtual void cv2obj_Xfrom(const Fquaternion&, const Fvector&, Fmatrix&) {}
    virtual void cv2bone_Xfrom(const Fquaternion&, const Fvector&, Fmatrix&) {}
    virtual const Fvector& ControlAccel() const = 0;
    virtual float& FrictionFactor() = 0;
    virtual void CutVelocity(float l_limit, float a_limit);
    virtual u16 get_elements_number() { return 1; }
    virtual CPHSynchronize* get_element_sync(u16 element)
    {
        VERIFY(element == 0);
        return static_cast<CPHSynchronize*>(this);
    }
    virtual CElevatorState* ElevatorState() = 0;

    virtual const Fmatrix& XFORM() const override;
    virtual void get_LinearVel(Fvector& velocity) override;
    virtual void get_AngularVel(Fvector& velocity) override;
    virtual u16 numberOfGeoms() override { return 0; }
    virtual IPhysicsGeometry* geometry(u16) const override { return nullptr; }
    virtual const Fvector& mass_Center() override;

public:
    virtual void step(float dt) = 0; //{ step( dt ); }

public:
    CPHCharacter();
    virtual ~CPHCharacter() = 0;
};

inline CPHCharacter::~CPHCharacter() = default;

void virtual_move_collide_callback(bool& do_collide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
