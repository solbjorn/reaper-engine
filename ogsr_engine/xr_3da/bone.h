#pragma once

// refs
class CBone;

#define BI_NONE (u16(-1))

#define OGF_IKDATA_VERSION 0x0001

#define MAX_BONE_PARAMS 4

class CBoneInstance;
// callback
typedef void BoneCallbackFunction(CBoneInstance* P);
typedef BoneCallbackFunction* BoneCallback;
// typedef void  (* BoneCallback)		(CBoneInstance* P);

//*** Bone Instance *******************************************************************************

class CBoneInstance : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CBoneInstance);

public:
    // data
    Fmatrix mTransform; // final x-form matrix (local to model)
    Fmatrix mRenderTransform; // final x-form matrix (model_base -> bone -> model)
    Fmatrix mRenderTransform_prev; // Prev x-form matrix
    Fmatrix mRenderTransform_temp; // Temp var

private:
    BoneCallback Callback;
    void* Callback_Param;
    BOOL Callback_overwrite; // performance hint - don't calc anims
    u32 Callback_type;

public:
    float param[MAX_BONE_PARAMS]; //

    //
    // methods
    ~CBoneInstance() override = default;

    [[nodiscard]] BoneCallback callback() const { return Callback; }
    [[nodiscard]] void* callback_param() const { return Callback_Param; }
    [[nodiscard]] bool callback_overwrite() const { return Callback_overwrite; } // performance hint - don't calc anims
    [[nodiscard]] u32 callback_type() const { return Callback_type; }

    IC void construct()
    {
        mTransform.identity();
        mRenderTransform.identity();
        mRenderTransform_prev.identity();
        mRenderTransform_temp.identity();

        Callback = nullptr;
        Callback_Param = nullptr;
        Callback_overwrite = FALSE;
        Callback_type = 0;

        std::memset(&param, 0, sizeof(param));
    }

    void set_callback(u32 Type, BoneCallback C, void* Param, BOOL overwrite = FALSE)
    {
        Callback = C;
        Callback_Param = Param;
        Callback_overwrite = overwrite;
        Callback_type = Type;
    }

    void reset_callback()
    {
        Callback = nullptr;
        Callback_Param = nullptr;
        Callback_overwrite = false;
        Callback_type = 0;
    }

    void set_callback_overwrite(BOOL v) { Callback_overwrite = v; }

    void set_param(u32 idx, float data);
    float get_param(u32 idx);

    [[nodiscard]] gsl::index mem_usage() const { return gsl::index{sizeof(*this)}; }
};

struct vertBoned1W // (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
    Fvector P;
    Fvector N;
    Fvector T;
    Fvector B;
    float u, v;
    u32 matrix;

    void get_pos(Fvector& p) const { p.set(P); }
#ifdef DEBUG
    static const u8 bones_count = 1;
    u16 get_bone_id(u8 bone) const
    {
        VERIFY(bone < bones_count);
        return u16(matrix);
    }
#endif
};
static_assert(sizeof(vertBoned1W) == 60);

struct vertBoned2W // (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
    u16 matrix0;
    u16 matrix1;
    Fvector P;
    Fvector N;
    Fvector T;
    Fvector B;
    float w;
    float u, v;

    void get_pos(Fvector& p) const { p.set(P); }
#ifdef DEBUG
    static const u8 bones_count = 2;
    u16 get_bone_id(u8 bone) const
    {
        VERIFY(bone < bones_count);
        return bone == 0 ? matrix0 : matrix1;
    }
#endif
};
static_assert(sizeof(vertBoned2W) == 64);

#pragma pack(push, 2)

struct vertBoned3W // 70 bytes
{
    u16 m[3];
    Fvector P;
    Fvector N;
    Fvector T;
    Fvector B;
    float w[2];
    float u, v;

    void get_pos(Fvector& p) const { p.set(P); }
#ifdef DEBUG
    static const u8 bones_count = 3;
    u16 get_bone_id(u8 bone) const
    {
        VERIFY(bone < bones_count);
        return m[bone];
    }
#endif
};
static_assert(sizeof(vertBoned3W) == 70);

#pragma pack(pop)

struct vertBoned4W // 76 bytes
{
    u16 m[4];
    Fvector P;
    Fvector N;
    Fvector T;
    Fvector B;
    float w[3];
    float u, v;

    void get_pos(Fvector& p) const { p.set(P); }
#ifdef DEBUG
    static const u8 bones_count = 4;
    u16 get_bone_id(u8 bone) const
    {
        VERIFY(bone < bones_count);
        return m[bone];
    }
#endif
};
static_assert(sizeof(vertBoned4W) == 76);

enum EJointType : u32
{
    jtRigid,
    jtCloth,
    jtJoint,
    jtWheel,
    jtNone,
    jtSlider,
};

struct SJointLimit
{
    Fvector2 limit;
    float spring_factor;
    float damping_factor;

    SJointLimit() { Reset(); }
    void Reset()
    {
        limit.set(0.f, 0.f);
        spring_factor = 1.f;
        damping_factor = 1.f;
    }
};
static_assert(sizeof(SJointLimit) == 16);

struct SBoneShape
{
    enum EShapeType : u16
    {
        stNone,
        stBox,
        stSphere,
        stCylinder,
    };

    enum EShapeFlags
    {
        sfNoPickable = (1 << 0), // use only in RayPick
        sfRemoveAfterBreak = (1 << 1),
        sfNoPhysics = (1 << 2),
        sfNoFogCollider = (1 << 3),
    };

    u16 type; // 2
    Flags16 flags; // 2
    Fobb box; // 15*4
    Fsphere sphere; // 4*4
    Fcylinder cylinder; // 8*4

    constexpr SBoneShape() { Reset(); }

    constexpr void Reset()
    {
        flags.zero();
        type = stNone;
        box.invalidate();
        sphere.P.set(0.f, 0.f, 0.f);
        sphere.R = 0.f;
        cylinder.invalidate();
    }

    [[nodiscard]] constexpr bool Valid() const
    {
        switch (type)
        {
        case stBox: return !fis_zero(box.m_halfsize.x) && !fis_zero(box.m_halfsize.y) && !fis_zero(box.m_halfsize.z);
        case stSphere: return !fis_zero(sphere.R);
        case stCylinder: return !fis_zero(cylinder.m_height) && !fis_zero(cylinder.m_radius) && !fis_zero(cylinder.m_direction.square_magnitude());
        }

        return true;
    }
};
static_assert(sizeof(SBoneShape) == 112);

struct SJointIKData
{
    // IK
    EJointType type;
    u32 pad;
    SJointLimit limits[3]; // by [axis XYZ on joint] and[Z-wheel,X-steer on wheel]
    float spring_factor;
    float damping_factor;
    enum
    {
        flBreakable = (1 << 0),
    };
    Flags32 ik_flags;
    float break_force; // [0..+INF]
    float break_torque; // [0..+INF]

    float friction;

    constexpr SJointIKData() { Reset(); }

    constexpr void Reset()
    {
        limits[0].Reset();
        limits[1].Reset();
        limits[2].Reset();
        type = jtRigid;
        spring_factor = 1.f;
        damping_factor = 1.f;
        ik_flags.zero();
        break_force = 0.f;
        break_torque = 0.f;
    }

    void clamp_by_limits(Fvector& dest_xyz);

    void Export(IWriter& F)
    {
        F.w_u32(type);
        for (int k = 0; k < 3; k++)
        {
            // Kostya Slipchenko say:
            // направление вращения в ОДЕ отличается от направления вращение в X-Ray
            // поэтому меняем знак у лимитов
            // F.w_float	(_min(-limits[k].limit.x,-limits[k].limit.y)); // min (swap special for ODE)
            // F.w_float	(_max(-limits[k].limit.x,-limits[k].limit.y)); // max (swap special for ODE)

            VERIFY(fsimilar(_min(-limits[k].limit.x, -limits[k].limit.y), -limits[k].limit.y));
            VERIFY(fsimilar(_max(-limits[k].limit.x, -limits[k].limit.y), -limits[k].limit.x));

            F.w_float(-limits[k].limit.y); // min (swap special for ODE)
            F.w_float(-limits[k].limit.x); // max (swap special for ODE)

            F.w_float(limits[k].spring_factor);
            F.w_float(limits[k].damping_factor);
        }
        F.w_float(spring_factor);
        F.w_float(damping_factor);

        F.w_u32(ik_flags.get());
        F.w_float(break_force);
        F.w_float(break_torque);

        F.w_float(friction);
    }

    bool Import(IReader& F, u16 vers)
    {
        type = (EJointType)F.r_u32();
        F.r(limits, sizeof(SJointLimit) * 3);
        spring_factor = F.r_float();
        damping_factor = F.r_float();
        ik_flags.flags = F.r_u32();
        break_force = F.r_float();
        break_torque = F.r_float();
        if (vers > 0)
        {
            friction = F.r_float();
        }
        return true;
    }
};
static_assert(sizeof(SJointIKData) == 80);

class XR_NOVTABLE IBoneData : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IBoneData);

public:
    ~IBoneData() override = 0;
};

inline IBoneData::~IBoneData() = default;

// static const Fobb	dummy ;//= Fobb().identity();
//  refs
class CBone;
DEFINE_VECTOR(CBone*, BoneVec, BoneIt);

class CBone : public CBoneInstance, public IBoneData
{
    RTTI_DECLARE_TYPEINFO(CBone, CBoneInstance, IBoneData);

public:
    shared_str name;
    shared_str parent_name;
    shared_str wmap;
    Fvector rest_offset;
    Fvector rest_rotate; // XYZ format (Game format)
    float rest_length;

    Fvector mot_offset;
    Fvector mot_rotate; // XYZ format (Game format)
    float mot_length;

    Fmatrix mot_transform;

    Fmatrix local_rest_transform;
    Fmatrix rest_transform;
    Fmatrix rest_i_transform;

    // Fmatrix			    last_transform;

    // Fmatrix				render_transform;
public:
    int SelfID;
    CBone* parent;
    BoneVec children;

public:
    // editor part
    Flags8 flags;
    enum
    {
        flSelected = (1 << 0),
    };
    SJointIKData IK_data;
    shared_str game_mtl;
    // SBoneShape shape;

    float mass;
    Fvector center_of_mass;

public:
    CBone();
    ~CBone() override;

    void SetName(const char* p)
    {
        name._set(p);
        xr_strlwr(name);
    }

    void SetParentName(const char* p)
    {
        parent_name._set(p);
        xr_strlwr(parent_name);
    }

    void SetWMap(const char* p) { wmap._set(p); }

    void SetRestParams(float length, const Fvector& offset, const Fvector& rotate)
    {
        rest_offset.set(offset);
        rest_rotate.set(rotate);
        rest_length = length;
    }

    shared_str Name() { return name; }
    shared_str ParentName() { return parent_name; }
    shared_str WMap() { return wmap; }
    IC CBone* Parent() { return parent; }
    IC BOOL IsRoot() { return !parent; }
    shared_str& NameRef() { return name; }

    //// transformation
    // const Fvector& _Offset() { return mot_offset; }
    // const Fvector& _Rotate() { return mot_rotate; }
    // float _Length() { return mot_length; }
    // IC Fmatrix& _RTransform() { return rest_transform; }
    // IC Fmatrix& _RITransform() { return rest_i_transform; }
    // IC Fmatrix& _LRTransform() { return local_rest_transform; }
    // IC Fmatrix& _MTransform() { return mot_transform; }

    // IC Fmatrix& _LTransform() { return mTransform; } //{return last_transform;}
    // IC const Fmatrix& _LTransform() const { return mTransform; }

    // IC Fmatrix& _RenderTransform() { return mRenderTransform; } //{return render_transform;}
    // IC Fvector& _RestOffset() { return rest_offset; }
    // IC Fvector& _RestRotate() { return rest_rotate; }

    // void _Update(const Fvector& T, const Fvector& R)
    //{
    //     mot_offset.set(T);
    //     mot_rotate.set(R);
    //     mot_length = rest_length;
    // }
    // void Reset()
    //{
    //     mot_offset.set(rest_offset);
    //     mot_rotate.set(rest_rotate);
    //     mot_length = rest_length;
    // }

    // IO
    void Save(IWriter& F);
    void Load_0(IReader& F);
    void Load_1(IReader& F);

    IC float engine_lo_limit(u8 k) const { return -IK_data.limits[k].limit.y; }
    IC float engine_hi_limit(u8 k) const { return -IK_data.limits[k].limit.x; }

    IC float editor_lo_limit(u8 k) const { return IK_data.limits[k].limit.x; }
    IC float editor_hi_limit(u8 k) const { return IK_data.limits[k].limit.y; }

    void SaveData(IWriter& F);
    void LoadData(IReader& F);
    void ResetData();
    void CopyData(CBone* bone);
};

//*** Shared Bone Data ****************************************************************************
class CBoneData;
// t-defs
typedef xr_vector<CBoneData*> vecBones;
typedef vecBones::iterator vecBonesIt;

class CBoneData : public IBoneData
{
    RTTI_DECLARE_TYPEINFO(CBoneData, IBoneData);

protected:
    u16 SelfID;
    u16 ParentID;

public:
    shared_str name;

    Fobb obb;

    Fmatrix bind_transform;
    Fmatrix m2b_transform; // model to bone conversion transform
    SBoneShape shape;
    shared_str game_mtl_name;
    u16 game_mtl_idx;
    SJointIKData IK_data;
    float mass;
    Fvector center_of_mass;

    vecBones children; // bones which are slaves to this

    DEFINE_VECTOR(u16, FacesVec, FacesVecIt);
    DEFINE_VECTOR(FacesVec, ChildFacesVec, ChildFacesVecIt);
    ChildFacesVec child_faces; // shared

    explicit CBoneData(u16 ID) : SelfID{ID} { VERIFY(SelfID != BI_NONE); }
    ~CBoneData() override = default;

#ifdef DEBUG
    typedef svector<int, 128> BoneDebug;
    void DebugQuery(BoneDebug& L);
#endif
    IC void SetParentID(u16 id) { ParentID = id; }

    IC u16 GetSelfID() const { return SelfID; }
    IC u16 GetParentID() const { return ParentID; }

    // assign face
    void AppendFace(u16 child_idx, u16 idx) { child_faces[child_idx].push_back(idx); }
    // Calculation
    void CalculateM2B(const Fmatrix& Parent);

public:
    [[nodiscard]] virtual gsl::index mem_usage() const
    {
        gsl::index sz{gsl::index{sizeof(*this)} + gsl::index{sizeof(vecBones::value_type)} * std::ssize(children)};

        for (auto& face : child_faces)
            sz += std::ssize(face) * gsl::index{sizeof(FacesVec::value_type)} + gsl::index{sizeof(face)};

        return sz;
    }
};

enum EBoneCallbackType : u32
{
    bctDummy = u32(0), // 0 - required!!!
    bctPhysics,
    bctCustom,
};
