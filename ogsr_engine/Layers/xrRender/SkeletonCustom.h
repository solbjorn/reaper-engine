#pragma once

#include "fhierrarhyvisual.h"
#include "../../xr_3da/bone.h"
#include "../../Include/xrRender/Kinematics.h"
#include "../../xr_3da/vismask.h"

// refs
class CKinematics;
class CInifile;
class CBoneData;
class CSkeletonX;
struct SEnumVerticesCallback;

class CSkeletonWallmark : public intrusive_base // 4+4+4+12+4+16+16 = 60 + 4 = 64
{
    RTTI_DECLARE_TYPEINFO(CSkeletonWallmark, intrusive_base);

public:
    CKinematics* m_Parent; // 4
    const Fmatrix* m_XForm; // 4
    ref_shader m_Shader; // 4
    Fvector3 m_ContactPoint; // 12		model space
    float m_fTimeStart; // 4

public:
#ifdef DEBUG
    u32 used_in_render;
#endif

    Fsphere m_LocalBounds{}; // 16		model space

    struct WMFace
    {
        Fvector3 vert[3];
        Fvector2 uv[3];
        u16 bone_id[3][4];
        float weight[3][3];
    };
    DEFINE_VECTOR(WMFace, WMFacesVec, WMFacesVecIt);

    WMFacesVec m_Faces; // 16
    Fsphere m_Bounds{}; // 16		world space

    CSkeletonWallmark(CKinematics* p, const Fmatrix* m, ref_shader s, const Fvector& cp, float ts) : m_Parent{p}, m_XForm{m}, m_Shader{s}, m_ContactPoint{cp}, m_fTimeStart{ts}
    {
#ifdef DEBUG
        used_in_render = u32(-1);
#endif
    }

    ~CSkeletonWallmark()
#ifdef DEBUG
        ;
#else
    {
    }
#endif

    IC CKinematics* Parent() { return m_Parent; }
    IC u32 VCount() { return m_Faces.size() * 3; }
    IC bool Similar(ref_shader& sh, const Fvector& cp, float eps) { return (m_Shader == sh) && m_ContactPoint.similar(cp, eps); }
    IC float TimeStart() { return m_fTimeStart; }
    IC const Fmatrix* XFORM() { return m_XForm; }
    IC const Fvector3& ContactPoint() { return m_ContactPoint; }
    IC ref_shader Shader() { return m_Shader; }
};
DEFINE_VECTOR(intrusive_ptr<CSkeletonWallmark>, SkeletonWMVec, SkeletonWMVecIt);

// sanity check
#ifdef DEBUG
struct dbg_marker
{
    BOOL* lock;
    dbg_marker(BOOL* b)
    {
        lock = b;
        VERIFY(*lock == FALSE);
        *lock = TRUE;
    }
    ~dbg_marker() { *lock = FALSE; }
};

#define _DBG_SINGLE_USE_MARKER dbg_marker _dbg_marker(&dbg_single_use_marker)
#else
#define _DBG_SINGLE_USE_MARKER
#endif

class CKinematics : public FHierrarhyVisual, public IKinematics
{
    RTTI_DECLARE_TYPEINFO(CKinematics, FHierrarhyVisual, IKinematics);

public:
    typedef FHierrarhyVisual inherited;
    friend class CBoneData;
    friend class CSkeletonX;

protected: //--#SM+#--
    DEFINE_VECTOR(KinematicsABT::additional_bone_transform, BONE_TRANSFORM_VECTOR, BONE_TRANSFORM_VECTOR_IT);
    BONE_TRANSFORM_VECTOR m_bones_offsets;

public:
#ifdef DEBUG
    BOOL dbg_single_use_marker;
#endif

    /************************* Add by Zander *******************************/
    void SetRFlag(const u32 id, const bool v) override { children.at(id)->setRZFlag(v); }
    bool GetRFlag(const u32 id) const override { return children.at(id)->getRZFlag(); }
    u32 RChildCount() const override { return children.size(); }

    Fvector3 RC_VisBox(const u32 id) override;
    Fvector3 RC_VisCenter(const u32 id) override;
    Fvector3 RC_VisBorderMin(const u32 id) override;
    Fvector3 RC_VisBorderMax(const u32 id) override;
    void RC_Dump() override;
    /************************* End Add *************************************/

    void Bone_Calculate(CBoneData* bd, const Fmatrix* parent) override;
    void CLBone(const CBoneData* bd, CBoneInstance& bi, const Fmatrix* parent, u8 mask_channel = (1 << 0));

    void BoneChain_Calculate(const CBoneData* bd, CBoneInstance& bi, u8 channel_mask, bool ignore_callbacks);
    void Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks);

    virtual void BuildBoneMatrix(const CBoneData* bd, CBoneInstance& bi, const Fmatrix* parent, u8 mask_channel = (1 << 0));
    virtual void OnCalculateBones() {}

    virtual void CalculateBonesAdditionalTransforms(const CBoneData* bd, CBoneInstance& bi); //--#SM+#--
    virtual void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset); //--#SM+#--
    virtual void LL_ClearAdditionalTransform(u16 bone_id = BI_NONE); //--#SM+#--

    dxRender_Visual* m_lod;

protected:
    SkeletonWMVec wallmarks;
    u32 wm_frame;
    u32 CurrentFrame{};

    xr_vector<dxRender_Visual*> children_invisible;

    // Globals
    CInifile* pUserData;
    CBoneInstance* bone_instances; // bone instances
    vecBones* bones; // all bones	(shared)
    u16 iRoot; // Root bone index

    // Fast search
    string_unordered_map<shared_str, u16> bone_map_N; // bones associations

    BOOL Update_Visibility;
    u32 UCalc_Time;
    s32 UCalc_Visibox;

    VisMask visimask;

    CSkeletonX* LL_GetChild(u32 idx);

    // internal functions
    virtual CBoneData* CreateBoneData(u16 ID) { return xr_new<CBoneData>(ID); }
    virtual void IBoneInstances_Create();
    virtual void IBoneInstances_Destroy();
    void Visibility_Invalidate() { Update_Visibility = TRUE; }
    void Visibility_Update();

    void LL_Validate();

public:
    UpdateCallback Update_Callback;
    void* Update_Callback_Param;

    // wallmarks
    void AddWallmark(const Fmatrix* parent, const Fvector3& start, const Fvector3& dir, ref_shader shader, float size);
    void CalculateWallmarks(bool hud);
    void RenderWallmark(intrusive_ptr<CSkeletonWallmark> wm, FVF::LIT*& verts);
    void ClearWallmarks();

    bool PickBone(const Fmatrix& parent_xform, IKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id);
    virtual void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id);

    CKinematics();
    virtual ~CKinematics();

    // Low level interface
    u16 LL_BoneID(const char* B) const override;
    u16 LL_BoneID(const shared_str& B) const override;
    const char* LL_BoneName_dbg(const u16 ID) const override;

    CInifile* LL_UserData() { return pUserData; }

    ICF CBoneInstance& LL_GetBoneInstance(u16 bone_id)
    {
        // Msg("visual_name: %s, bone_id: %d", dbg_name.c_str(), bone_id);

        R_ASSERT(bone_id < LL_BoneCount(), make_string("visual_name: %s, bone_id: %d", dbg_name.c_str(), bone_id));
        R_ASSERT(bone_instances);
        return bone_instances[bone_id];
    }
    ICF const CBoneInstance& LL_GetBoneInstance(u16 bone_id) const
    {
        // Msg("visual_name: %s, bone_id: %d", dbg_name.c_str(), bone_id);

        R_ASSERT(bone_id < LL_BoneCount(), make_string("visual_name: %s, bone_id: %d", dbg_name.c_str(), bone_id));
        R_ASSERT(bone_instances);
        return bone_instances[bone_id];
    }

    CBoneData& LL_GetData(u16 bone_id)
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", __FUNCTION__, dbg_name.c_str(), bone_id);
        VERIFY(bones);
        CBoneData& bd = *((*bones)[bone_id]);
        return bd;
    }

    virtual const IBoneData& GetBoneData(u16 bone_id) const
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", __FUNCTION__, dbg_name.c_str(), bone_id);
        VERIFY(bones);
        CBoneData& bd = *((*bones)[bone_id]);
        return bd;
    }
    CBoneData* LL_GetBoneData(u16 bone_id)
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", __FUNCTION__, dbg_name.c_str(), bone_id);
        VERIFY(bones);
        u32 sz = sizeof(vecBones);
        u32 sz1 = sizeof(((*bones)[bone_id])->children);
        Msg("sz: %u, sz1: %u", sz, sz1);
        CBoneData* bd = ((*bones)[bone_id]);
        return bd;
    }
    u16 LL_BoneCount() const { return u16(bones->size()); }
    u16 LL_VisibleBoneCount() { return visimask.count(); }
    ICF Fmatrix& LL_GetTransform(u16 bone_id) { return LL_GetBoneInstance(bone_id).mTransform; }
    ICF const Fmatrix& LL_GetTransform(u16 bone_id) const { return LL_GetBoneInstance(bone_id).mTransform; }
    ICF Fmatrix& LL_GetTransform_R(u16 bone_id) { return LL_GetBoneInstance(bone_id).mRenderTransform; } // rendering only
    Fobb& LL_GetBox(u16 bone_id)
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", __FUNCTION__, dbg_name.c_str(), bone_id);
        return (*bones)[bone_id]->obb;
    }
    const Fbox& GetBox() const { return vis.box; }
    void LL_GetBindTransform(xr_vector<Fmatrix>& matrices);
    int LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups);

    u16 LL_GetBoneRoot() { return iRoot; }
    void LL_SetBoneRoot(u16 bone_id)
    {
        VERIFY(bone_id < LL_BoneCount());
        iRoot = bone_id;
    }

    BOOL LL_GetBoneVisible(u16 bone_id)
    {
        VERIFY(bone_id < LL_BoneCount());
        return visimask.is(bone_id);
    }
    void LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive);
    VisMask LL_GetBonesVisible() { return visimask; }
    void LL_SetBonesVisible(VisMask mask);

    // Main functionality
    virtual void CalculateBones(BOOL bForceExact = FALSE); // Recalculate skeleton
    void CalculateBones_Invalidate();
    void Callback(UpdateCallback C, void* Param)
    {
        Update_Callback = C;
        Update_Callback_Param = Param;
    }

    //	Callback: data manipulation
    virtual void SetUpdateCallback(UpdateCallback pCallback) { Update_Callback = pCallback; }
    virtual void SetUpdateCallbackParam(void* pCallbackParam) { Update_Callback_Param = pCallbackParam; }

    virtual UpdateCallback GetUpdateCallback() { return Update_Callback; }
    virtual void* GetUpdateCallbackParam() { return Update_Callback_Param; }

#ifdef DEBUG
    // debug
    void DebugRender(Fmatrix& XFORM);

protected:
    virtual shared_str getDebugName() { return dbg_name; }

public:
#endif

    // General "Visual" stuff
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Spawn();
    virtual void Depart();
    virtual void Release();

    virtual IKinematicsAnimated* dcast_PKinematicsAnimated() { return nullptr; }
    virtual IRenderVisual* dcast_RenderVisual() { return this; }
    virtual IKinematics* dcast_PKinematics() { return this; }
    //	virtual	CKinematics*		dcast_PKinematics	()				{ return this;	}

    [[nodiscard]] virtual gsl::index mem_usage(bool bInstance) const
    {
        auto sz{gsl::index{sizeof(*this)} + (bone_instances ? bone_instances->mem_usage() : 0)};

        if (!bInstance)
        {
            for (auto bone : *bones)
                sz += gsl::index{sizeof(vecBones::value_type)} + bone->mem_usage();
        }

        return sz;
    }

private:
    bool m_is_original_lod;
};

IC CKinematics* PCKinematics(dxRender_Visual* V) { return V ? smart_cast<CKinematics*>(V->dcast_PKinematics()) : nullptr; }
