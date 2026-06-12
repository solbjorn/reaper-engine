/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifndef PH_JOINT
#define PH_JOINT

#include "PhysicsShell.h"

class CPHJointDestroyInfo;

class CPHJoint : public CPhysicsJoint
{
    RTTI_DECLARE_TYPEINFO(CPHJoint, CPhysicsJoint);

public:
    ///////////////////////////////////////////////////////
    u16 m_bone_id{std::numeric_limits<u16>::max()};
    CPHElement* pFirst_element;
    CPHElement* pSecond_element;
    CODEGeom* pFirstGeom{};
    /////////////////////////////////////////////////////////
    CPHShell* pShell{};
    dJointID m_joint{};
    dJointID m_joint1{};
    CPhysicsJoint** m_back_ref{};
    CPHJointDestroyInfo* m_destroy_info{};
    float m_erp; // joint erp
    float m_cfm; // joint cfm
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct SPHAxis
    {
        float high; // high limit
        float low; // law limit
        float zero; // zero angle position
        float erp; // limit erp
        float cfm; // limit cfm
        eVs vs; // coordinate system
        float force; // max force
        float velocity; // velocity to achieve
        Fvector direction; // axis direction
        IC void set_limits(float h, float l)
        {
            high = h;
            low = l;
        }
        IC void set_direction(const Fvector& v) { direction.set(v); }
        IC void set_direction(const float x, const float y, const float z) { direction.set(x, y, z); }
        IC void set_param(const float e, const float c)
        {
            erp = e;
            cfm = c;
        }
        void set_sd_factors(float sf, float df, enumType jt);
        SPHAxis();
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    xr_vector<SPHAxis> axes;
    Fvector anchor;
    eVs vs_anchor;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateBall();
    void CreateHinge();
    void CreateHinge2();
    void CreateFullControl();
    void CreateSlider();
    void LimitAxisNum(int& axis_num);
    void SetForceActive(const int axis_num);
    void SetVelocityActive(const int axis_num);
    void SetLimitsActive(int axis_num);
    void CalcAxis(int ax_num, Fvector& axis, float& lo, float& hi, const Fmatrix& first_matrix, const Fmatrix& second_matrix);
    void CalcAxis(int ax_num, Fvector& axis, float& lo, float& hi, const Fmatrix& first_matrix, const Fmatrix& second_matrix, const Fmatrix&);
    [[nodiscard]] u16 GetAxesNumber() override;
    void SetAxisSDfactors(f32 spring_factor, f32 damping_factor, s32 axis_num) override;
    void SetJointSDfactors(f32 spring_factor, f32 damping_factor) override;
    virtual void SetJointSDfactorsActive();
    virtual void SetLimitsSDfactorsActive();
    virtual void SetAxisSDfactorsActive(int axis_num);
    virtual void SetAxis(const SPHAxis& axis, const int axis_num);
    void SetAnchor(const Fvector& position) override { SetAnchor(position.x, position.y, position.z); }
    void SetAnchorVsFirstElement(const Fvector& position) override { SetAnchorVsFirstElement(position.x, position.y, position.z); }
    void SetAnchorVsSecondElement(const Fvector& position) override { SetAnchorVsSecondElement(position.x, position.y, position.z); }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAxisDir(const Fvector& orientation, const s32 axis_num) override { SetAxisDir(orientation.x, orientation.y, orientation.z, axis_num); }
    void SetAxisDirVsFirstElement(const Fvector& orientation, const s32 axis_num) override
    {
        SetAxisDirVsFirstElement(orientation.x, orientation.y, orientation.z, axis_num);
    }
    void SetAxisDirVsSecondElement(const Fvector& orientation, const s32 axis_num) override
    {
        SetAxisDirVsSecondElement(orientation.x, orientation.y, orientation.z, axis_num);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetLimits(const f32 low, const f32 high, const s32 axis_num) override;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAnchor(const f32 x, const f32 y, const f32 z) override;
    void SetAnchorVsFirstElement(const f32 x, const f32 y, const f32 z) override;
    void SetAnchorVsSecondElement(const f32 x, const f32 y, const f32 z) override;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAxisDir(const f32 x, const f32 y, const f32 z, const s32 axis_num) override;
    void SetAxisDirVsFirstElement(const f32 x, const f32 y, const f32 z, const s32 axis_num) override;
    void SetAxisDirVsSecondElement(const f32 x, const f32 y, const f32 z, const s32 axis_num) override;

public:
    [[nodiscard]] CPhysicsElement* PFirst_element() override;
    [[nodiscard]] CPhysicsElement* PSecond_element() override;
    [[nodiscard]] u16 BoneID() override { return m_bone_id; }
    void SetBoneID(u16 bone_id) override { m_bone_id = bone_id; }
    IC CPHElement* PFirstElement() { return pFirst_element; }
    IC CPHElement* PSecondElement() { return pSecond_element; }
    void Activate() override;
    void Create() override;
    void RunSimulation() override;
    void SetBackRef(CPhysicsJoint** j) override;
    void SetForceAndVelocity(const f32 force, const f32 velocity = 0.0f, const s32 axis_num = -1) override;
    virtual void SetForce(const float force, const int axis_num = -1);
    virtual void SetVelocity(const float velocity = 0.f, const int axis_num = -1);
    void SetBreakable(f32 force, f32 torque) override;
    [[nodiscard]] bool isBreakable() override { return !!m_destroy_info; }
    [[nodiscard]] dJointID GetDJoint() override { return m_joint; }
    virtual dJointID GetDJoint1() { return m_joint1; }
    void GetLimits(f32& lo_limit, f32& hi_limit, s32 axis_num) override;
    void GetAxisDir(s32 num, Fvector& axis, eVs& vs) override;
    void GetAxisDirDynamic(s32 num, Fvector& axis) override;
    void GetAnchorDynamic(Fvector& anchor) override;
    void GetAxisSDfactors(f32& spring_factor, f32& damping_factor, s32 axis_num) override;
    void GetJointSDfactors(f32& spring_factor, f32& damping_factor) override;
    void GetMaxForceAndVelocity(f32& force, f32& velocity, s32 axis_num) override;
    [[nodiscard]] f32 GetAxisAngle(s32 axis_num) override;
    void Deactivate() override;
    void ReattachFirstElement(CPHElement* new_element);
    CODEGeom*& RootGeom() { return pFirstGeom; }
    [[nodiscard]] CPHJointDestroyInfo* JointDestroyInfo() override { return m_destroy_info; }

    explicit CPHJoint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second);
    ~CPHJoint() override;

    void SetShell(CPHShell* p);
    void ClearDestroyInfo();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
IC void own_axis(const Fmatrix& m, Fvector& axis)
{
    if (m._11 == 1.f)
    {
        axis.set(1.f, 0.f, 0.f);
        return;
    }
    float k = m._13 * m._21 - m._11 * m._23 + m._23;

    if (k == 0.f)
    {
        if (m._13 == 0.f)
        {
            axis.set(0.f, 0.f, 1.f);
            return;
        }
        float k1 = m._13 / (1.f - m._11);
        axis.z = _sqrt(1.f / (1.f + k1 * k1));
        axis.x = axis.z * k1;
        axis.y = 0.f;
        return;
    }

    float k_zy = -(m._12 * m._21 - m._11 * m._22 + m._11 + m._22 - 1.f) / k;
    float k_xy = (m._12 + m._13 * k_zy) / (1.f - m._11);
    axis.y = _sqrt(1.f / (k_zy * k_zy + k_xy * k_xy + 1.f));
    axis.x = axis.y * k_xy;
    axis.z = axis.y * k_zy;
    return;
}

IC void own_axis_angle(const Fmatrix& m, Fvector& axis, float& angle)
{
    own_axis(m, axis);
    Fvector ort1, ort2;
    if (!(axis.z == 0.f && axis.y == 0.f))
    {
        ort1.set(0.f, -axis.z, axis.y);
        ort2.crossproduct(axis, ort1);
    }
    else
    {
        ort1.set(0.f, 1.f, 0.f);
        ort2.crossproduct(axis, ort1);
    }
    ort1.normalize();
    ort2.normalize();

    Fvector ort1_t;
    m.transform_dir(ort1_t, ort1);

    float cosinus = ort1.dotproduct(ort1_t);
    float sinus = ort2.dotproduct(ort1_t);
    angle = acosf(cosinus);
    if (sinus < 0.f)
        angle = -angle;
}

IC void axis_angleB(const Fmatrix& m, const Fvector& axis, float& angle)
{
    Fvector ort1, ort2;
    if (!(fis_zero(axis.z) && fis_zero(axis.y)))
    {
        ort1.set(0.f, -axis.z, axis.y);
        ort2.crossproduct(axis, ort1);
    }
    else
    {
        ort1.set(0.f, 1.f, 0.f);
        ort2.crossproduct(axis, ort1);
    }
    ort1.normalize();
    ort2.normalize();
    Fvector ort1_t;
    m.transform_dir(ort1_t, ort1);
    Fvector ort_r;
    float pr1, pr2;
    pr1 = ort1.dotproduct(ort1_t);
    pr2 = ort2.dotproduct(ort1_t);
    if (pr1 == 0.f && pr2 == 0.f)
    {
        angle = 0.f;
        return;
    }
    ort_r.set(pr1 * ort1.x + pr2 * ort2.x, pr1 * ort1.y + pr2 * ort2.y, pr1 * ort1.z + pr2 * ort2.z);

    ort_r.normalize();
    float cosinus = ort1.dotproduct(ort_r);
    float sinus = ort2.dotproduct(ort_r);
    angle = acosf(cosinus);
    if (sinus < 0.f)
        angle = -angle;
}

IC void axis_angleA(const Fmatrix& m, const Fvector& axis, float& angle)
{
    Fvector ort1, ort2, axis_t;
    m.transform_dir(axis_t, axis);
    if (!(fis_zero(axis_t.z) && fis_zero(axis_t.y)))
    {
        ort1.set(0.f, -axis_t.z, axis_t.y);
        ort2.crossproduct(axis_t, ort1);
    }
    else
    {
        ort1.set(0.f, 1.f, 0.f);
        ort2.crossproduct(axis_t, ort1);
    }
    ort1.normalize();
    ort2.normalize();
    Fvector ort1_t;
    m.transform_dir(ort1_t, ort1);
    Fvector ort_r;
    float pr1, pr2;
    pr1 = ort1.dotproduct(ort1_t);
    pr2 = ort2.dotproduct(ort1_t);
    if (pr1 == 0.f && pr2 == 0.f)
    {
        angle = 0.f;
        return;
    }
    ort_r.set(pr1 * ort1.x + pr2 * ort2.x, pr1 * ort1.y + pr2 * ort2.y, pr1 * ort1.z + pr2 * ort2.z);

    ort_r.normalize();
    float cosinus = ort1.dotproduct(ort_r);
    float sinus = ort2.dotproduct(ort_r);
    angle = acosf(cosinus);
    if (sinus < 0.f)
        angle = -angle;
    // if(angle>M_PI) angle=angle-2.f*M_PI;
    // if(angle<-M_PI) angle=angle+2.f*M_PI;
}

#endif
