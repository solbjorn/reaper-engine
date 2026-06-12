#pragma once

class CPHShell;
class CPHShellSplitterHolder;

#include "PHJoint.h"
#include "PHElement.h"
#include "PHDefs.h"
#include "PHShellSplitter.h"
#include "phmovestorage.h"

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
class CPhysicsShellAnimator;
#endif

class CPHShell : public CPhysicsShell, public CPHObject
{
    RTTI_DECLARE_TYPEINFO(CPHShell, CPhysicsShell, CPHObject);

public:
    friend class CPHShellSplitterHolder;

    enum
    {
        flActive = 1 << 0,
        flActivating = 1 << 1,
        flRemoveCharacterCollisionAfterDisable = 1 << 2,
    };

    s16 m_active_count{};
    Flags8 m_flags;
    ELEMENT_STORAGE elements;
    JOINT_STORAGE joints;
    CPHShellSplitterHolder* m_spliter_holder{};
    CPHMoveStorage m_traced_geoms;

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    CPhysicsShellAnimator* m_pPhysicsShellAnimatorC{};
#endif

protected:
    dSpaceID m_space{};

public:
    Fmatrix m_object_in_root;

    CPHShell();
    ~CPHShell() override;

    void applyImpulseTrace(const Fvector3& pos, const Fvector3& dir, f32 val, const u16 id) override;
    void applyHit(const Fvector3& pos, const Fvector3& dir, f32 val, const u16 id, ALife::EHitType hit_type) override;

    static void BonesCallback(CBoneInstance* B);
    static void StataticRootBonesCallBack(CBoneInstance* B);
    [[nodiscard]] BoneCallbackFun* GetBonesCallback() override { return BonesCallback; }
    [[nodiscard]] BoneCallbackFun* GetStaticObjectBonesCallback() override { return StataticRootBonesCallBack; }
    void add_Element(CPhysicsElement* E) override;
    void ToAnimBonesPositions() override;
    [[nodiscard]] bool AnimToVelocityState(f32 dt, f32 l_limit, f32 a_limit) override;
    void SetBonesCallbacksOverwrite(bool v) override;
    void SetPhObjectInElements();
    void EnableObject(CPHObject*) override;
    virtual void DisableObject();

    void SetAirResistance(f32 linear = default_k_l, f32 angular = default_k_w) override
    {
        xr_vector<CPHElement*>::iterator i;
        for (i = elements.begin(); elements.end() != i; ++i)
            (*i)->SetAirResistance(linear, angular);
    }

    void GetAirResistance(f32& linear, f32& angular) override { (*elements.begin())->GetAirResistance(linear, angular); }
    void add_Joint(CPhysicsJoint* J) override;

    [[nodiscard]] CPHIsland* PIsland() override { return &Island(); }
    void applyImpulseTrace(const Fvector3& pos, const Fvector3& dir, f32 val) override;

    void Update() override;

    void Activate(const Fmatrix& m0, f32, const Fmatrix& m2, bool disable = false) override;
    void Activate(const Fmatrix& transform, const Fvector3& lin_vel, const Fvector3& ang_vel, bool disable = false) override;

    void Activate(bool disable = false, bool not_set_bone_callbacks = false) override;
    void Activate(const Fmatrix&, bool = false) override {}

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    [[nodiscard]] CPhysicsShellAnimator* PPhysicsShellAnimator() override { return m_pPhysicsShellAnimatorC; }
#endif

private:
    void activate(bool disable);

public:
    void Build(bool disable = false) override;
    void RunSimulation(bool place_current_forms = true) override;
    void PresetActive();
    void AfterSetActive();
    void PureActivate();
    void Deactivate() override;
    [[nodiscard]] const CGID& GetCLGroup() const override;
    void RegisterToCLGroup(CGID g) override;
    [[nodiscard]] bool IsGroupObject() override;
    void SetIgnoreStatic() override;
    void SetIgnoreDynamic() override;
    void SetStatic() override;
    void SetDynamic() override;

    void SetRagDoll() override;
    void SetIgnoreRagDoll() override;

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    void CreateShellAnimator() override;
    void SetIgnoreAnimated() override;
    [[nodiscard]] bool Animated() override;
#endif

    void SetSmall() override;
    void SetIgnoreSmall() override;
    void setMass(f32 M) override;

    void setMass1(f32 M) override;
    void setEquelInertiaForEls(const dMass& M) override;
    void addEquelInertiaToEls(const dMass& M) override;
    [[nodiscard]] f32 getMass() override;
    void setDensity(f32 M) override;
    [[nodiscard]] f32 getDensity() override;
    [[nodiscard]] f32 getVolume() override;
    void get_Extensions(const Fvector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) override;
    void applyForce(const Fvector3& dir, f32 val) override;
    void applyForce(f32 x, f32 y, f32 z) override;
    void applyImpulse(const Fvector& dir, f32 val) override;
    void applyGravityAccel(const Fvector3& accel) override;
    void setTorque(const Fvector3& torque) override;
    void setForce(const Fvector3& force) override;

    void set_JointResistance(f32 force) override
    {
        JOINT_I i;
        for (i = joints.begin(); joints.end() != i; ++i)
        {
            (*i)->SetForce(force);
            (*i)->SetVelocity();
        }
    }

    void set_DynamicLimits(f32 l_limit = default_l_limit, f32 w_limit = default_w_limit) override;
    void set_DynamicScales(f32 l_scale = default_l_scale, f32 w_scale = default_w_scale) override;
    void set_ContactCallback(ContactCallbackFun* callback) override;
    void set_ObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void add_ObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void remove_ObjectContactCallback(ObjectContactCallbackFun* callback) override;
    void set_CallbackData(void* cd) override;
    [[nodiscard]] void* get_CallbackData() override;
    void set_PhysicsRefObject(CPhysicsShellHolder* ref_object) override;

    // breakbable interface
    [[nodiscard]] bool isBreakable() override;
    [[nodiscard]] bool isFractured() override;
    [[nodiscard]] CPHShellSplitterHolder* SplitterHolder() override { return m_spliter_holder; }
    void SplitProcess(PHSHELL_PAIR_VECTOR& out_shels) override;

    void BlockBreaking() override
    {
        if (m_spliter_holder)
            m_spliter_holder->SetUnbreakable();
    }

    void UnblockBreaking() override
    {
        if (m_spliter_holder)
            m_spliter_holder->SetBreakable();
    }

    [[nodiscard]] bool IsBreakingBlocked() override { return m_spliter_holder && m_spliter_holder->IsUnbreakable(); }
    ///////	////////////////////////////////////////////////////////////////////////////////////////////
    void get_LinearVel(Fvector3& velocity) override;
    void get_AngularVel(Fvector3& velocity) override;
    void set_LinearVel(const Fvector3& velocity) override;
    void set_AngularVel(const Fvector3& velocity) override;
    void TransformPosition(const Fmatrix& form) override;
    void SetGlTransformDynamic(const Fmatrix& form) override;
    void set_ApplyByGravity(bool flag) override;
    [[nodiscard]] bool get_ApplyByGravity() override;
    void SetMaterial(u16 m) override;
    void SetMaterial(gsl::czstring m) override;
    [[nodiscard]] ELEMENT_STORAGE& Elements() override { return elements; }
    [[nodiscard]] CPhysicsElement* get_Element(u16 bone_id) override;
    [[nodiscard]] CPhysicsElement* get_Element(const shared_str& bone_name) override;
    [[nodiscard]] CPhysicsElement* get_Element(gsl::czstring bone_name) override;
    [[nodiscard]] CPhysicsElement* get_ElementByStoreOrder(u16 num) override;
    [[nodiscard]] u16 get_ElementsNumber() override { return (u16)elements.size(); }
    [[nodiscard]] CPHSynchronize* get_ElementSync(u16 element) override;
    [[nodiscard]] u16 get_elements_number() override { return get_ElementsNumber(); }
    [[nodiscard]] CPHSynchronize* get_element_sync(u16 element) override { return get_ElementSync(element); }
    [[nodiscard]] CPhysicsElement* NearestToPoint(const Fvector3& point) override;
    [[nodiscard]] CPhysicsJoint* get_Joint(u16 bone_id) override;
    [[nodiscard]] CPhysicsJoint* get_Joint(const shared_str& bone_name) override;
    [[nodiscard]] CPhysicsJoint* get_Joint(gsl::czstring bone_name) override;
    [[nodiscard]] CPhysicsJoint* get_JointByStoreOrder(u16 num) override;
    [[nodiscard]] u16 get_JointsNumber() override;
    [[nodiscard]] CODEGeom* get_GeomByID(u16 bone_id) override;

    void SetAnimated(bool v) override;

    void Enable() override;
    void Disable() override;
    void DisableCollision() override;
    void EnableCollision() override;
    void DisableCharacterCollision() override;
    void SetRemoveCharacterCollLADisable() override { m_flags.set(flRemoveCharacterCollisionAfterDisable, TRUE); }
    [[nodiscard]] bool isEnabled() override { return CPHObject::is_active(); }
    [[nodiscard]] bool isActive() override { return !!m_flags.test(flActive); }
    [[nodiscard]] bool isFullActive() override { return isActive() && !m_flags.test(flActivating); }
    void SetNotActivating() { m_flags.set(flActivating, FALSE); }
    // CPHObject
    void vis_update_activate() override;
    void vis_update_deactivate() override;
    void PureStep(f32 step) override;
    void CollideAll() override;
    void PhDataUpdate(dReal step) override;
    void PhTune(dReal step) override;
    void InitContact(dContact*, bool&, u16, u16) override {}
    void FreezeContent() override;
    void UnFreezeContent() override;
    void Freeze() override;
    void UnFreeze() override;
    void NetInterpolationModeON() override { CPHObject::NetInterpolationON(); }
    void NetInterpolationModeOFF() override { CPHObject::NetInterpolationOFF(); }
    virtual void StepFrameUpdate(dReal) {}
    [[nodiscard]] CPHMoveStorage* MoveStorage() override { return &m_traced_geoms; }
    void build_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map = nullptr) override;
    void preBuild_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map) override;
    void ZeroCallbacks() override;
    void ResetCallbacks(u16 id, VisMask& mask) override;
    void PlaceBindToElForms();
    void SetCallbacks(BoneCallbackFun* callback) override;
    void EnabledCallbacks(BOOL val) override;
    void set_DisableParams(const SAllDDOParams& params) override;
    void UpdateRoot() override;
    void SmoothElementsInertia(f32 k) override;
    void InterpolateGlobalTransform(Fmatrix* m) override;
    void InterpolateGlobalPosition(Fvector* v) override;
    void GetGlobalTransformDynamic(Fmatrix* m) override;
    void GetGlobalPositionDynamic(Fvector* v) override;
    [[nodiscard]] Fmatrix& ObjectInRoot() override { return m_object_in_root; }
    void ObjectToRootForm(const Fmatrix& form) override;
    [[nodiscard]] virtual dSpaceID dSpace() { return m_space; }
    void SetTransform(const Fmatrix& m0) override;
    void AddTracedGeom(u16 element = 0, u16 geom = 0) override;
    void SetAllGeomTraced() override;
    void SetPrefereExactIntegration() override;
    void CutVelocity(f32 l_limit, f32 a_limit) override;
    ///////////	//////////////////////////////////////////////////////////////////////////////////////////
    void CreateSpace();
    void PassEndElements(u16 from, u16 to, CPHShell* dest);
    void PassEndJoints(u16 from, u16 to, CPHShell* dest);
    void DeleteElement(u16 element);
    void DeleteJoint(u16 joint);
    u16 BoneIdToRootGeom(u16 id);
    /////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    void get_spatial_params() override;
    [[nodiscard]] dGeomID dSpacedGeom() override { return (dGeomID)m_space; }

    void ClearRecentlyDeactivated() override;
    void ClearCashedTries();

private:
    // breakable
    void setEndElementSplitter();
    void setElementSplitter(u16 element_number, u16 splitter_position);
    void setEndJointSplitter();
    void AddSplitter(CPHShellSplitter::EType type, u16 element, u16 joint);
    void AddSplitter(CPHShellSplitter::EType type, u16 element, u16 joint, u16 position);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void AddElementRecursive(CPhysicsElement* root_e, u16 id, Fmatrix global_parent, u16 element_number, bool* vis_check);
    void PlaceBindToElFormsRecursive(Fmatrix parent, u16 id, u16 element, VisMask& mask);
    void BonesBindCalculateRecursive(Fmatrix parent, u16 id);
    void ZeroCallbacksRecursive(u16 id);
    void SetCallbacksRecursive(u16 id, u16 element);
    void ResetCallbacksRecursive(u16 id, u16 element, VisMask& mask);
    void SetJointRootGeom(CPhysicsElement* root_e, CPhysicsJoint* J);
    void ReanableObject();
    void ExplosionHit(const Fvector& pos, const Fvector& dir, float val);
    void ClearBreakInfo();
};
