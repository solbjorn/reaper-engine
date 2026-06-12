#pragma once

#include "PHDefs.h"
#include "PhysicsCommon.h"
#include "alife_space.h"
#include "script_export_space.h"

#include "../xr_3da/IPhysicsDefinitions.h"

class CPhysicsJoint;
class CPhysicsElement;
class CPhysicsShell;
class CPHFracture;
class CPHJointDestroyInfo;
class CODEGeom;
class CPHSynchronize;
class CPhysicsShellHolder;
class CGameObject;
class NET_Packet;
struct SBoneShape;
class CPHShellSplitterHolder;
class IKinematics;
typedef u32 CLClassBits;
typedef u32 CLBits;
typedef u32 CGID;

struct physicsBone
{
    CPhysicsJoint* joint{};
    CPhysicsElement* element{};

    physicsBone() = default;
};

DEFINE_MAP(u16, physicsBone, BONE_P_MAP, BONE_P_PAIR_IT);
typedef const BONE_P_MAP ::iterator BONE_P_PAIR_CIT;

// ABSTRACT:
class XR_NOVTABLE CPhysicsBase : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPhysicsBase);

public:
    Fmatrix mXFORM; // In parent space

    virtual void Activate(const Fmatrix& m0, f32 dt01, const Fmatrix& m2, bool disable = false) = 0;
    virtual void Activate(const Fmatrix& transform, const Fvector3& lin_vel, const Fvector3& ang_vel, bool disable = false) = 0;
    virtual void Activate(bool disable = false, bool not_set_bone_callbacks = false) = 0;
    virtual void Activate(const Fmatrix& form, bool disable = false) = 0;
    virtual void InterpolateGlobalTransform(Fmatrix* m) = 0;
    virtual void GetGlobalTransformDynamic(Fmatrix* m) = 0;
    virtual void InterpolateGlobalPosition(Fvector* v) = 0;
    virtual void GetGlobalPositionDynamic(Fvector* v) = 0;
    [[nodiscard]] virtual bool isBreakable() = 0;
    [[nodiscard]] virtual bool isEnabled() = 0;
    [[nodiscard]] virtual bool isActive() = 0;
    [[nodiscard]] virtual bool isFullActive() = 0;
    virtual void Deactivate() = 0;
    virtual void Enable() = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void setMass(f32 M) = 0;
    virtual void setDensity(f32 M) = 0;
    [[nodiscard]] virtual f32 getMass() = 0;
    [[nodiscard]] virtual f32 getDensity() = 0;
    [[nodiscard]] virtual f32 getVolume() = 0;
    virtual void get_Extensions(const Fvector& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void applyForce(const Fvector3& dir, f32 val) = 0;
    virtual void applyForce(f32 x, f32 y, f32 z) = 0;
    virtual void applyImpulse(const Fvector& dir, f32 val) = 0;
    virtual void setTorque(const Fvector3& torque) = 0;
    virtual void setForce(const Fvector3& force) = 0;
    virtual void applyGravityAccel(const Fvector3& accel) = 0;
    virtual void SetAirResistance(f32 linear = default_k_l, f32 angular = default_k_w) = 0;
    virtual void GetAirResistance(f32& linear, f32& angular) = 0;
    virtual void set_DynamicLimits(f32 l_limit = default_l_limit, f32 w_limit = default_w_limit) = 0;
    virtual void set_DynamicScales(f32 l_scale = default_l_scale, f32 w_scale = default_w_scale) = 0;
    virtual void set_ContactCallback(ContactCallbackFun* callback) = 0;
    virtual void set_ObjectContactCallback(ObjectContactCallbackFun* callback) = 0;
    virtual void add_ObjectContactCallback(ObjectContactCallbackFun* callback) = 0;
    virtual void remove_ObjectContactCallback(ObjectContactCallbackFun* callback) = 0;
    virtual void set_CallbackData(void* cd) = 0;
    [[nodiscard]] virtual void* get_CallbackData() = 0;
    virtual void set_PhysicsRefObject(CPhysicsShellHolder* ref_object) = 0;
    virtual void set_LinearVel(const Fvector3& velocity) = 0;
    virtual void set_AngularVel(const Fvector3& velocity) = 0;
    virtual void TransformPosition(const Fmatrix& form) = 0;
    virtual void set_ApplyByGravity(bool flag) = 0;
    [[nodiscard]] virtual bool get_ApplyByGravity() = 0;
    virtual const Fmatrix& XFORM() const { return mXFORM; }

    virtual void SetMaterial(u16 m) = 0;
    virtual void SetMaterial(gsl::czstring m) = 0;
    virtual void set_DisableParams(const SAllDDOParams& params) = 0;
    virtual void SetTransform(const Fmatrix& m0) = 0;

    virtual void SetAnimated(bool v) = 0;

    ~CPhysicsBase() override = 0;
};

inline CPhysicsBase::~CPhysicsBase() = default;

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class XR_NOVTABLE CPhysicsElement : public CPhysicsBase, public IPhysicsElement
{
    RTTI_DECLARE_TYPEINFO(CPhysicsElement, CPhysicsBase, IPhysicsElement);

public:
    u16 m_SelfID;

    [[nodiscard]] virtual CPhysicsShell* PhysicsShell() = 0;
    void set_ContactCallback(ContactCallbackFun* callback) override = 0;
    [[nodiscard]] virtual CPhysicsShellHolder* PhysicsRefObject() = 0;
    virtual void add_Sphere(const Fsphere& V) = 0;
    virtual void add_Box(const Fobb& V) = 0;
    virtual void add_Cylinder(const Fcylinder& V) = 0;
    virtual void add_Shape(const SBoneShape& shape) = 0;
    virtual void add_Shape(const SBoneShape& shape, const Fmatrix& offset) = 0;
    [[nodiscard]] virtual CODEGeom* last_geom() = 0;
    [[nodiscard]] virtual bool has_geoms() = 0;
    virtual void add_Mass(const SBoneShape& shape, const Fmatrix& offset, const Fvector& mass_center, f32 mass, CPHFracture* fracture = nullptr) = 0;
    virtual void set_ParentElement(CPhysicsElement* p) = 0;
    [[nodiscard]] virtual CPhysicsElement* get_ParentElement() = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void set_BoxMass(const Fobb& box, f32 mass) = 0;
    virtual void setInertia(const dMass& M) = 0;
    virtual void addInertia(const dMass& M) = 0;
    virtual void setMassMC(f32 M, const Fvector& mass_center) = 0;
    virtual void applyImpulseVsMC(const Fvector3& pos, const Fvector3& dir, f32 val) = 0;
    virtual void applyImpulseVsGF(const Fvector3& pos, const Fvector3& dir, f32 val) = 0;
    virtual void applyImpulseTrace(const Fvector& pos, const Fvector& dir, f32 val, const u16 id) = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void setDensityMC(f32 M, const Fvector& mass_center) = 0;
    [[nodiscard]] virtual u16 setGeomFracturable(CPHFracture& fracture) = 0;
    [[nodiscard]] virtual CPHFracture& Fracture(u16 num) = 0;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] virtual dBodyID get_body() = 0;
    [[nodiscard]] virtual const Fvector& local_mass_Center() = 0;
    [[nodiscard]] virtual f32 getRadius() = 0;
    [[nodiscard]] virtual dMass* getMassTensor() = 0;
    virtual void get_MaxAreaDir(Fvector& dir) = 0;
    [[nodiscard]] virtual ObjectContactCallbackFun* get_ObjectContactCallback() = 0;
    virtual void Fix() = 0;
    virtual void ReleaseFixed() = 0;
    [[nodiscard]] virtual bool isFixed() = 0;

    [[nodiscard]] const Fmatrix& XFORM() const override { return CPhysicsBase::XFORM(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~CPhysicsElement() override = 0;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CPhysicsElement);

inline CPhysicsElement::~CPhysicsElement() = default;

add_to_type_list(CPhysicsElement);
#undef script_type_list
#define script_type_list save_type_list(CPhysicsElement)

// ABSTRACT:
//  Joint between two elements

class XR_NOVTABLE CPhysicsJoint : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPhysicsJoint);

public:
    bool bActive;
    enum eVs
    {
        // coordinate system
        vs_first, // in first local
        vs_second, // in second local
        vs_global // in global
    };
    enum enumType
    {
        // joint type
        ball, // ball-socket
        hinge, // standart hinge 1 - axis
        hinge2, // for car wheels 2-axes
        full_control, // 3 - axes control (eiler - angles)
        slider
    };
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enumType eType; // type of the joint

public:
    ~CPhysicsJoint() override = 0;

    [[nodiscard]] virtual u16 BoneID() = 0;
    virtual void SetBoneID(u16 bone_id) = 0;
    [[nodiscard]] virtual CPhysicsElement* PFirst_element() = 0;
    [[nodiscard]] virtual CPhysicsElement* PSecond_element() = 0;
    [[nodiscard]] virtual u16 GetAxesNumber() = 0;
    virtual void Activate() = 0;
    virtual void Create() = 0;
    virtual void RunSimulation() = 0;
    virtual void Deactivate() = 0;
    virtual void SetBackRef(CPhysicsJoint** j) = 0;
    virtual void SetAnchor(const Fvector& position) = 0;
    virtual void SetAxisSDfactors(f32 spring_factor, f32 damping_factor, s32 axis_num) = 0;
    virtual void SetJointSDfactors(f32 spring_factor, f32 damping_factor) = 0;
    virtual void SetAnchorVsFirstElement(const Fvector& position) = 0;
    virtual void SetAnchorVsSecondElement(const Fvector& position) = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetAxisDir(const Fvector& orientation, const s32 axis_num) = 0;
    virtual void SetAxisDirVsFirstElement(const Fvector& orientation, const s32 axis_num) = 0;
    virtual void SetAxisDirVsSecondElement(const Fvector& orientation, const s32 axis_num) = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetAnchor(const f32 x, const f32 y, const f32 z) = 0;
    virtual void SetAnchorVsFirstElement(const f32 x, const f32 y, const f32 z) = 0;
    virtual void SetAnchorVsSecondElement(const f32 x, const f32 y, const f32 z) = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetAxisDir(const f32 x, const f32 y, const f32 z, const s32 axis_num) = 0;
    virtual void SetAxisDirVsFirstElement(const f32 x, const f32 y, const f32 z, const s32 axis_num) = 0;
    virtual void SetAxisDirVsSecondElement(const f32 x, const f32 y, const f32 z, const s32 axis_num) = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetLimits(const f32 low, const f32 high, const s32 axis_num) = 0;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetBreakable(f32 force, f32 torque) = 0;
    [[nodiscard]] virtual CPHJointDestroyInfo* JointDestroyInfo() = 0;
    [[nodiscard]] virtual bool isBreakable() = 0;
    virtual void SetForceAndVelocity(const f32 force, const f32 velocity = 0.0f, const s32 axis_num = -1) = 0;
    virtual void GetMaxForceAndVelocity(f32& force, f32& velocity, s32 axis_num) = 0;
    [[nodiscard]] virtual f32 GetAxisAngle(s32 axis_num) = 0;
    [[nodiscard]] virtual dJointID GetDJoint() = 0;
    virtual void GetAxisSDfactors(f32& spring_factor, f32& damping_factor, s32 axis_num) = 0;
    virtual void GetJointSDfactors(f32& spring_factor, f32& damping_factor) = 0;
    virtual void GetLimits(f32& lo_limit, f32& hi_limit, s32 axis_num) = 0;
    virtual void GetAxisDir(s32 num, Fvector& axis, eVs& vs) = 0;
    virtual void GetAxisDirDynamic(s32 num, Fvector& axis) = 0;
    virtual void GetAnchorDynamic(Fvector& anchor) = 0;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

inline CPhysicsJoint::~CPhysicsJoint() = default;

add_to_type_list(CPhysicsJoint);
#undef script_type_list
#define script_type_list save_type_list(CPhysicsJoint)

// ABSTRACT:
class CPHIsland;

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
class CPhysicsShellAnimator;
#endif

class XR_NOVTABLE CPhysicsShell : public CPhysicsBase, public IPhysicsShell
{
    RTTI_DECLARE_TYPEINFO(CPhysicsShell, CPhysicsBase, IPhysicsShell);

protected:
    IKinematics* m_pKinematics;

public:
#ifdef DEBUG
    CPhysicsShellHolder* dbg_obj;
#endif
public:
    IC IKinematics* PKinematics() { return m_pKinematics; }

    [[nodiscard]] const Fmatrix& XFORM() const override { return CPhysicsBase::XFORM(); }
    [[nodiscard]] IPhysicsElement& IElement(u16 index) override { return *get_ElementByStoreOrder(index); }

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    [[nodiscard]] virtual CPhysicsShellAnimator* PPhysicsShellAnimator() = 0;
#endif
    void set_Kinematics(IKinematics* p) { m_pKinematics = p; }
    virtual void set_JointResistance(f32 force) = 0;
    virtual void add_Element(CPhysicsElement* E) = 0;
    virtual void add_Joint(CPhysicsJoint* E) = 0;
    [[nodiscard]] virtual CPHIsland* PIsland() = 0;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////
    [[nodiscard]] virtual const CGID& GetCLGroup() const = 0;
    virtual void RegisterToCLGroup(CGID g) = 0;
    [[nodiscard]] virtual bool IsGroupObject() = 0;
    virtual void SetIgnoreStatic() = 0;
    virtual void SetIgnoreDynamic() = 0;
    virtual void SetStatic() = 0;
    virtual void SetDynamic() = 0;

    virtual void SetRagDoll() = 0;
    virtual void SetIgnoreRagDoll() = 0;

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    virtual void CreateShellAnimator() = 0;
    virtual void SetIgnoreAnimated() = 0;
    [[nodiscard]] virtual bool Animated() = 0;
#endif

    virtual void SetSmall() = 0;
    virtual void SetIgnoreSmall() = 0;
    [[nodiscard]] virtual bool isFractured() = 0;
    [[nodiscard]] virtual CPHShellSplitterHolder* SplitterHolder() = 0;
    virtual void SplitProcess(PHSHELL_PAIR_VECTOR& out_shels) = 0;
    virtual void BlockBreaking() = 0;
    virtual void UnblockBreaking() = 0;
    [[nodiscard]] virtual bool IsBreakingBlocked() = 0;
    virtual void applyImpulseTrace(const Fvector3& pos, const Fvector3& dir, f32 val) = 0;
    virtual void applyImpulseTrace(const Fvector3& pos, const Fvector3& dir, f32 val, const u16 id) = 0;
    virtual void applyHit(const Fvector3& pos, const Fvector3& dir, f32 val, const u16 id, ALife::EHitType hit_type) = 0;
    [[nodiscard]] virtual BoneCallbackFun* GetBonesCallback() = 0;
    [[nodiscard]] virtual BoneCallbackFun* GetStaticObjectBonesCallback() = 0;
    virtual void Update() = 0;

    virtual void get_LinearVel(Fvector& velocity) = 0;
    virtual void get_AngularVel(Fvector& velocity) = 0;

    virtual void setMass1(f32 M) = 0;
    virtual void SmoothElementsInertia(f32 k) = 0;
    virtual void setEquelInertiaForEls(const dMass& M) = 0;
    virtual void addEquelInertiaToEls(const dMass& M) = 0;
    [[nodiscard]] virtual ELEMENT_STORAGE& Elements() = 0;
    [[nodiscard]] virtual CPhysicsElement* get_Element(u16 bone_id) = 0;
    [[nodiscard]] virtual CPhysicsElement* get_Element(const shared_str& bone_name) = 0;
    [[nodiscard]] virtual CPhysicsElement* get_Element(gsl::czstring bone_name) = 0;
    [[nodiscard]] virtual CPhysicsElement* get_ElementByStoreOrder(u16 num) = 0;
    [[nodiscard]] virtual CPHSynchronize* get_ElementSync(u16 element) = 0;
    [[nodiscard]] virtual CPhysicsJoint* get_Joint(u16 bone_id) = 0;
    [[nodiscard]] virtual CPhysicsJoint* get_Joint(const shared_str& bone_name) = 0;
    [[nodiscard]] virtual CPhysicsJoint* get_Joint(gsl::czstring bone_name) = 0;
    [[nodiscard]] virtual CPhysicsJoint* get_JointByStoreOrder(u16 num) = 0;
    [[nodiscard]] virtual u16 get_JointsNumber() = 0;
    [[nodiscard]] virtual CODEGeom* get_GeomByID(u16 bone_id) = 0;
    virtual void Freeze() = 0;
    virtual void UnFreeze() = 0;
    virtual void NetInterpolationModeON() = 0;
    virtual void NetInterpolationModeOFF() = 0;
    virtual void Disable() = 0;
    virtual void DisableCollision() = 0;
    virtual void EnableCollision() = 0;
    virtual void SetRemoveCharacterCollLADisable() = 0;
    virtual void DisableCharacterCollision() = 0;
    virtual void PureStep(f32 step = fixed_step) = 0;
    virtual void SetGlTransformDynamic(const Fmatrix& form) = 0;
    virtual void CollideAll() = 0;
    [[nodiscard]] virtual CPhysicsElement* NearestToPoint(const Fvector3& point) = 0;
    virtual void build_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map = nullptr) = 0;
    virtual void preBuild_FromKinematics(IKinematics* K, BONE_P_MAP* p_geting_map = nullptr) = 0;
    virtual void Build(bool disable = false) = 0;
    virtual void SetMaxAABBRadius(f32) {}
    virtual void AddTracedGeom(u16 element = 0, u16 geom = 0) = 0;
    virtual void SetAllGeomTraced() = 0;
    virtual void RunSimulation(bool place_current_forms = true) = 0;
    virtual void UpdateRoot() = 0;
    virtual void ZeroCallbacks() = 0;
    virtual void ResetCallbacks(u16 id, VisMask& mask) = 0;
    virtual void SetCallbacks(BoneCallbackFun* callback) = 0;
    virtual void EnabledCallbacks(BOOL val) = 0;
    virtual void ToAnimBonesPositions() = 0;
    [[nodiscard]] virtual bool AnimToVelocityState(f32 dt, f32 l_limit, f32 a_limit) = 0;
    virtual void SetBonesCallbacksOverwrite(bool v) = 0;
    [[nodiscard]] virtual Fmatrix& ObjectInRoot() = 0;
    virtual void ObjectToRootForm(const Fmatrix& form) = 0;
    virtual void SetPrefereExactIntegration() = 0;

    ~CPhysicsShell() override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CPhysicsShell);

add_to_type_list(CPhysicsShell);
#undef script_type_list
#define script_type_list save_type_list(CPhysicsShell)

void get_box(CPhysicsShell* shell, const Fmatrix& form, Fvector& sz, Fvector& c);

// Implementation creator
CPhysicsJoint* P_create_Joint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second);
CPhysicsElement* P_create_Element();
CPhysicsShell* P_create_Shell();
CPhysicsShell* P_create_splited_Shell();
CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, LPCSTR fixed_bones);
CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, U16Vec& fixed_bones);
CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* bone_map, LPCSTR fixed_bones);
CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* bone_map = nullptr);
CPhysicsShell* P_build_SimpleShell(CGameObject* obj, float mass, bool not_active_state);

void ApplySpawnIniToPhysicShell(CInifile* ini, CPhysicsShell* physics_shell, bool fixed);
void fix_bones(LPCSTR fixed_bones, CPhysicsShell* shell);
