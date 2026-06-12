#pragma once

///////////////////////////Implemetation//for//CPhysicsElement//////////////////

#include "Geometry.h"
#include "phdefs.h"
#include "PhysicsCommon.h"
#include "PHSynchronize.h"
#include "PHDisabling.h"
#include "PHGeometryOwner.h"
#include "PHInterpolation.h"

class CPHElement;
class CPHShell;
class CPHFracture;
struct SPHImpact;
class CPHFracturesHolder;

class CPHElement : public CPhysicsElement, public CPHSynchronize, public CPHDisablingFull, public CPHGeometryOwner
{
    RTTI_DECLARE_TYPEINFO(CPHElement, CPhysicsElement, CPHSynchronize, CPHDisablingFull, CPHGeometryOwner);

public:
    friend class CPHFracturesHolder;

    dMass m_mass; // e ??				//bl
    dBodyID m_body{}; // e					//st
    dReal m_l_scale; // ->to shell ??	//bl
    dReal m_w_scale; // ->to shell ??	//bl
    CPHElement* m_parent_element{}; // bool !			//bl
    CPHShell* m_shell{}; // e					//bl
    CPHInterpolation m_body_interpolation; // e					//bl
    CPHFracturesHolder* m_fratures_holder{}; // e					//bl

    dReal m_w_limit; //->to shell ??		//bl
    dReal m_l_limit; //->to shell ??		//bl
    dReal k_w; //->to shell ??		//st
    dReal k_l; //->to shell ??		//st

    Flags8 m_flags; //
    enum
    {
        flActive = 1 << 0,
        flActivating = 1 << 1,
        flUpdate = 1 << 2,
        flWasEnabledBeforeFreeze = 1 << 3,
        flEnabledOnStep = 1 << 4,
        flFixed = 1 << 5,
        flAnimated = 1 << 6
    };

    ////////////////////////////////////////////Interpolation/////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    void FillInterpolation() // interpolation called anywhere visual influent
    {
        m_body_interpolation.ResetPositions();
        m_body_interpolation.ResetRotations();
        m_flags.set(flUpdate, TRUE);
    }

    IC void UpdateInterpolation() // interpolation called from ph update visual influent
    {
        /// VERIFY(dBodyStateValide(m_body));
        m_body_interpolation.UpdatePositions();
        m_body_interpolation.UpdateRotations();
        // bUpdate=true;
        m_flags.set(flUpdate, TRUE);
    }

public:
    ////////////////////////////////////////////////Geometry/////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void add_Sphere(const Fsphere& V) override; // aux
    void add_Box(const Fobb& V) override; // aux
    void add_Cylinder(const Fcylinder& V) override; // aux
    void add_Shape(const SBoneShape& shape) override; // aux
    void add_Shape(const SBoneShape& shape, const Fmatrix& offset) override; // aux
    [[nodiscard]] CODEGeom* last_geom() override { return CPHGeometryOwner::last_geom(); } // aux
    [[nodiscard]] bool has_geoms() override { return CPHGeometryOwner::has_geoms(); }
    void set_ContactCallback(ContactCallbackFun* callback) override; // aux (may not be)
    void set_ObjectContactCallback(ObjectContactCallbackFun* callback) override; // called anywhere ph state influent
    void add_ObjectContactCallback(ObjectContactCallbackFun* callback) override; // called anywhere ph state influent
    void remove_ObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void set_CallbackData(void* cd) override;
    [[nodiscard]] void* get_CallbackData() override;
    [[nodiscard]] ObjectContactCallbackFun* get_ObjectContactCallback() override;
    void set_PhysicsRefObject(CPhysicsShellHolder* ref_object) override; // aux
    [[nodiscard]] CPhysicsShellHolder* PhysicsRefObject() override { return m_phys_ref_object; } // aux
    void SetMaterial(u16 m) override; // aux
    void SetMaterial(gsl::czstring m) override { CPHGeometryOwner::SetMaterial(m); } // aux
    [[nodiscard]] u16 numberOfGeoms() override; // aux
    [[nodiscard]] const Fvector& local_mass_Center() override { return CPHGeometryOwner::local_mass_Center(); } // aux
    [[nodiscard]] f32 getVolume() override { return CPHGeometryOwner::get_volume(); } // aux
    void get_Extensions(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) override; // aux
    void get_MaxAreaDir(Fvector& dir) override { CPHGeometryOwner::get_MaxAreaDir(dir); }
    [[nodiscard]] f32 getRadius() override;

    ////////////////////////////////////////////////////Mass/////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void calculate_it_data(const Fvector& mc, float mass); // aux
    void calculate_it_data_use_density(const Fvector& mc, float density); // aux
    void calc_it_fract_data_use_density(const Fvector& mc, float density); // sets element mass and fractures parts mass	//aux
    dMass recursive_mass_summ(u16 start_geom, FRACTURE_I cur_fracture); // aux

public: //
    const Fvector& mass_Center() override; // aux
    void setDensity(f32 M) override; // aux
    [[nodiscard]] f32 getDensity() override { return m_mass.mass / m_volume; } // aux
    void setMassMC(f32 M, const Fvector& mass_center) override; // aux
    void setDensityMC(f32 M, const Fvector& mass_center) override; // aux
    void setInertia(const dMass& M) override; // aux
    void addInertia(const dMass& M) override;
    void add_Mass(const SBoneShape& shape, const Fmatrix& offset, const Fvector& mass_center, f32 mass, CPHFracture* fracture = nullptr) override; // aux
    void set_BoxMass(const Fobb& box, f32 mass) override; // aux
    void setMass(f32 M) override; // aux
    [[nodiscard]] f32 getMass() override { return m_mass.mass; } // aux
    [[nodiscard]] dMass* getMassTensor() override; // aux
    void ReAdjustMassPositions(const Fmatrix& shift_pivot, float density); // aux
    void ResetMass(float density); // aux
    void CutVelocity(float l_limit, float a_limit);
    ///////////////////////////////////////////////////PushOut///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////Disable/////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void Disable() override; //
    void ReEnable() override; //
    void Enable() override; // aux
    [[nodiscard]] bool isEnabled() override { return isActive() && dBodyIsEnabled(m_body); }
    [[nodiscard]] bool isFullActive() override { return isActive() && !m_flags.test(flActivating); }
    [[nodiscard]] bool isActive() override { return !!m_flags.test(flActive); }
    virtual void Freeze(); //
    virtual void UnFreeze(); //
    virtual bool EnabledStateOnStep() { return dBodyIsEnabled(m_body) || m_flags.test(flEnabledOnStep); } //
    ////////////////////////////////////////////////Updates///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool AnimToVel(float dt, float l_limit, float a_limit);
    void BoneGlPos(Fmatrix& m, CBoneInstance* B);
    void ToBonePos(CBoneInstance* B);

    void SetBoneCallbackOverwrite(bool v);
    void BonesCallBack(CBoneInstance* B); // called from updateCL visual influent
    void StataticRootBonesCallBack(CBoneInstance* B);
    void PhDataUpdate(dReal step); // ph update
    void PhTune(dReal step); // ph update
    virtual void Update(); // called update CL visual influence
    //////////////////////////////////////////////////Dynamics////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SetAirResistance(f32 linear = default_k_l, f32 angular = default_k_w) override // aux (may not be)
    {
        k_w = angular; //
        k_l = linear; //
    }

    void GetAirResistance(f32& linear, f32& angular) override
    {
        linear = k_l; //
        angular = k_w; //
    }

    virtual void applyImpact(const SPHImpact& impact); //
    void applyImpulseTrace(const Fvector& pos, const Fvector& dir, f32 val, const u16 id) override; // called anywhere ph state influent
    void set_DisableParams(const SAllDDOParams& params) override; //
    void set_DynamicLimits(f32 l_limit = default_l_limit, f32 w_limit = default_w_limit) override; // aux (may not be)
    void set_DynamicScales(f32 l_scale = default_l_scale, f32 w_scale = default_w_scale) override; // aux (may not be)
    void Fix() override;
    void ReleaseFixed() override;
    [[nodiscard]] bool isFixed() override { return !!(m_flags.test(flFixed)); }
    void applyForce(const Fvector3& dir, f32 val) override; // aux
    void applyForce(f32 x, f32 y, f32 z) override; // called anywhere ph state influent
    void applyImpulse(const Fvector3& dir, f32 val) override; // aux
    void applyImpulseVsMC(const Fvector3& pos, const Fvector3& dir, f32 val) override; //
    void applyImpulseVsGF(const Fvector3& pos, const Fvector3& dir, f32 val) override; //
    void applyGravityAccel(const Fvector3& accel) override;
    virtual void getForce(Fvector& force);
    virtual void getTorque(Fvector& torque);
    void get_LinearVel(Fvector& velocity) override; // aux
    void get_AngularVel(Fvector& velocity) override; // aux
    void set_LinearVel(const Fvector3& velocity) override; // called anywhere ph state influent
    void set_AngularVel(const Fvector3& velocity) override; // called anywhere ph state influent
    void setForce(const Fvector3& force) override; //
    void setTorque(const Fvector3& torque) override; //
    void set_ApplyByGravity(bool flag) override; //
    [[nodiscard]] bool get_ApplyByGravity() override; //
    ///////////////////////////////////////////////////Net////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void get_State(SPHNetState& state) override; //
    void set_State(const SPHNetState& state) override; //
    ///////////////////////////////////////////////////Position///////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetTransform(const Fmatrix& m0) override; //
    void TransformPosition(const Fmatrix& form) override;
    virtual void getQuaternion(Fquaternion& quaternion); //
    virtual void setQuaternion(const Fquaternion& quaternion); //
    virtual void SetGlobalPositionDynamic(const Fvector& position); //
    void GetGlobalPositionDynamic(Fvector* v) override; //
    void cv2obj_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) override; //
    void cv2bone_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) override; //
    void InterpolateGlobalTransform(Fmatrix* m) override; // called UpdateCL vis influent
    void InterpolateGlobalPosition(Fvector* v) override; // aux
    void GetGlobalTransformDynamic(Fmatrix* m) override; // aux
    IC void InverceLocalForm(Fmatrix&);
    IC void MulB43InverceLocalForm(Fmatrix&);

    ////////////////////////////////////////////////////Structure/////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] CPhysicsShell* PhysicsShell() override; // aux
    CPHShell* PHShell();
    void set_ParentElement(CPhysicsElement* p) override { m_parent_element = (CPHElement*)p; } // aux
    [[nodiscard]] CPhysicsElement* get_ParentElement() override { return m_parent_element; }
    void SetShell(CPHShell* p); // aux
    [[nodiscard]] dBodyID get_body() override { return m_body; } // aux
    virtual dBodyID get_bodyConst() const { return m_body; } // aux
    //////////////////////////////////////////////////////Breakable//////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    IC CPHFracturesHolder* FracturesHolder() { return m_fratures_holder; } // aux
    IC const CPHFracturesHolder* constFracturesHolder() const { return m_fratures_holder; } // aux
    void DeleteFracturesHolder(); //
    [[nodiscard]] bool isBreakable() override; // aux
    [[nodiscard]] u16 setGeomFracturable(CPHFracture& fracture) override; // aux
    [[nodiscard]] CPHFracture& Fracture(u16 num) override; // aux
    void SplitProcess(ELEMENT_PAIR_VECTOR& new_elements); // aux
    void PassEndGeoms(u16 from, u16 to, CPHElement* dest); // aux
    ////////////////////////////////////////////////////Build/Activate////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Activate(const Fmatrix& m0, f32 dt01, const Fmatrix& m2, bool disable = false) override; // some isues not to be aux
    void Activate(const Fmatrix& transform, const Fvector3& lin_vel, const Fvector3& ang_vel, bool disable = false) override; // some isues not to be aux
    void Activate(bool disable = false, bool not_set_bone_callbacks = false) override; // some isues not to be aux
    void Activate(const Fmatrix& start_from, bool disable = false) override; // some isues not to be aux
    void Deactivate() override; // aux //aux
    void CreateSimulBase(); // create body & cpace																//aux
    void ReInitDynamics(const Fmatrix& shift_pivot, float density); // set body & geom positions
    void PresetActive(); //
    void build(); // aux
    void build(bool disable); // aux
    void destroy(); // called anywhere ph state influent
    void Start(); // aux
    void RunSimulation(); // called anywhere ph state influent
    void RunSimulation(const Fmatrix& start_from); //
    void ClearDestroyInfo();
    void GetAnimBonePos(Fmatrix& bp);

    CPHElement(); // aux
    ~CPHElement() override; // aux

    [[nodiscard]] IPhysicsGeometry* geometry(u16 i) const override { return CPHGeometryOwner::Geom(i); }
    void SetAnimated(bool v) override { m_flags.set(flAnimated, BOOL(v)); }
};

IC CPHElement* cast_PHElement(CPhysicsElement* e) { return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e)); }
IC CPHElement* cast_PHElement(void* e) { return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e)); }
IC CPhysicsElement* cast_PhysicsElement(CPHElement* e) { return static_cast<CPhysicsElement*>(static_cast<CPHElement*>(e)); }
