//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include "skeletoncustom.h"
#include "animation.h"
#include "../../xr_3da/SkeletonMotions.h"

#include "../../Include/xrRender/KinematicsAnimated.h"

//*** Bone Instance *******************************************************************************
#pragma pack(push, 8)
class CBlendInstance // Bone Instance Blend List (per-bone data)
{
public:
    typedef svector<CBlend*, MAX_BLENDED> BlendSVec;
    typedef BlendSVec::iterator BlendSVecIt;
    typedef BlendSVec::const_iterator BlendSVecCIt;

private:
    BlendSVec Blend;

public:
    // methods
    IC BlendSVec& blend_vector() { return Blend; }
    void construct();
    void blend_add(CBlend* H);
    void blend_remove(CBlend* H);

    u32 mem_usage()
    {
        u32 sz = sizeof(*this);
        for (auto& it : Blend)
            sz += it->mem_usage();
        return sz;
    }
};
#pragma pack(pop)

// typedef void	( * MotionMarkCallback)		(CBlend*		P);

//*** The visual itself ***************************************************************************
class CKinematicsAnimated : public CKinematics, public IKinematicsAnimated
{
    RTTI_DECLARE_TYPEINFO(CKinematicsAnimated, CKinematics, IKinematicsAnimated);

public:
    typedef CKinematics inherited;
    friend class CBoneData;
    friend class CMotionDef;
    friend class CSkeletonX;

private:
    // Motion control
    void Bone_Motion_Start(CBoneData* bd, CBlend* handle); // with recursion
    void Bone_Motion_Stop(CBoneData* bd, CBlend* handle); // with recursion

    void Bone_Motion_Start_IM(CBoneData* bd, CBlend* handle);
    void Bone_Motion_Stop_IM(CBoneData* bd, CBlend* handle);

public:
    // Calculation
private:
    void LL_BuldBoneMatrixDequatize(const CBoneData* bd, u8 channel_mask, SKeyTable& keys);
    void LL_BoneMatrixBuild(CBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys);
    virtual void BuildBoneMatrix(const CBoneData* bd, CBoneInstance& bi, const Fmatrix* parent, u8 mask_channel = (1 << 0));

public:
    virtual void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset); //--#SM+#--
    virtual void LL_ClearAdditionalTransform(u16 bone_id = BI_NONE); //--#SM+#--

    virtual void OnCalculateBones();

private:
    u32 Update_LastTime{};

    CBlendInstance* blend_instances{};

    struct SMotionsSlot
    {
        shared_motions motions;
        BoneMotionsVec bone_motions;
    };
    using MotionsSlotVec = xr_vector<SMotionsSlot>;
    MotionsSlotVec m_Motions;

    CPartition* m_Partition{};

    IBlendDestroyCallback* m_blend_destroy_callback{};
    IUpdateTracksCallback* m_update_tracks_callback{};
    // Blending
    svector<CBlend, MAX_BLENDED_POOL> blend_pool;
    BlendSVec blend_cycles[MAX_PARTS];
    BlendSVec blend_fx;
    animation::channels channels;

protected:
    // internal functions
    virtual void IBoneInstances_Create();
    virtual void IBoneInstances_Destroy();

    void IBlend_Startup();
    void ChannelFactorsStartup();
    CBlend* IBlend_Create();

private:
    void IBlendSetup(CBlend& B, u16 part, u8 channel, MotionID motion_ID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback,
                     LPVOID CallbackParam);
    void IFXBlendSetup(CBlend& B, MotionID motion_ID, float blendAccrue, float blendFalloff, float Power, float Speed, u16 bone);
    //.	bool						LoadMotions				(LPCSTR N, IReader *data);
public:
    std::pair<LPCSTR, LPCSTR> LL_MotionDefName_dbg(MotionID ID);
    void LL_DumpBlends_dbg();

    u32 LL_PartBlendsCount(u32 bone_part_id);
    CBlend* LL_PartBlend(u32 bone_part_id, u32 n);
    void LL_IterateBlends(IterateBlendsCallback& callback);

    void SetUpdateTracksCalback(IUpdateTracksCallback* callback);
    IUpdateTracksCallback* GetUpdateTracksCalback() { return m_update_tracks_callback; }

    u16 LL_MotionsSlotCount() { return (u16)m_Motions.size(); }
    const shared_motions& LL_MotionsSlot(u16 idx) { return m_Motions[idx].motions; }

    IC CMotionDef* LL_GetMotionDef(MotionID id) { return m_Motions[id.slot].motions.motion_def(id.idx); }
    IC CMotion* LL_GetRootMotion(MotionID id) { return &m_Motions[id.slot].bone_motions[iRoot]->at(id.idx); }
    IC CMotion* LL_GetMotion(MotionID id, u16 bone_id) { return &m_Motions[id.slot].bone_motions[bone_id]->at(id.idx); }

    virtual IBlendDestroyCallback* GetBlendDestroyCallback();
    virtual void SetBlendDestroyCallback(IBlendDestroyCallback* cb);
    // Low level interface
    MotionID LL_MotionID(LPCSTR B);
    u16 LL_PartID(LPCSTR B);

    CBlend* LL_PlayFX(u16 bone, MotionID motion, float blendAccrue, float blendFalloff, float Speed, float Power);
    CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam,
                         u8 channel = 0);
    CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0);
    void LL_FadeCycle(u16 partition, float falloff, u8 mask_channel = (1 << 0));
    void LL_CloseCycle(u16 partition, u8 mask_channel = (1 << 0));
    void LL_SetChannelFactor(u16 channel, float factor);
    CBlendInstance& LL_GetBlendInstance(u16 bone_id)
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", __FUNCTION__, dbg_name.c_str(), bone_id);
        return blend_instances[bone_id];
    }

    // Main functionality
    void UpdateTracks(); // Update motions
    void LL_UpdateTracks(float dt, bool b_force, bool leave_blends); // Update motions
    void LL_UpdateFxTracks(float dt);
    void DestroyCycle(CBlend& B);

    // cycles
    MotionID ID_Cycle(const shared_str& N) override;
    MotionID ID_Cycle_Safe(const shared_str& N) override;
    CBlend* PlayCycle(const shared_str& N, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0) override;
    CBlend* PlayCycle(MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0);
    CBlend* PlayCycle(u16 partition, MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0);
    // fx'es
    MotionID ID_FX(LPCSTR N);
    MotionID ID_FX_Safe(LPCSTR N);
    CBlend* PlayFX(LPCSTR N, float power_scale);
    CBlend* PlayFX(MotionID M, float power_scale);

    const CPartition& partitions() const { return *m_Partition; };

    // General "Visual" stuff
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Release();
    virtual void Spawn();
    virtual IKinematicsAnimated* dcast_PKinematicsAnimated() { return this; }
    virtual IRenderVisual* _BCL dcast_RenderVisual() { return this; }
    virtual IKinematics* _BCL dcast_PKinematics() { return this; }

    virtual ~CKinematicsAnimated();
    CKinematicsAnimated();

    virtual u32 mem_usage(bool bInstance)
    {
        u32 sz = CKinematics::mem_usage(bInstance) + sizeof(*this) + (bInstance && blend_instances ? blend_instances->mem_usage() : 0);
        return sz;
    }

    IC const BlendSVec& blend_cycle(const u32& bone_part_id) const
    {
        VERIFY(bone_part_id < MAX_PARTS);
        return (blend_cycles[bone_part_id]);
    }

    virtual float get_animation_length(MotionID motion_ID);
};
// IC CKinematicsAnimated* PKinematicsAnimated(IRenderVisual* V) { return V?V->dcast_PKinematicsAnimated():0; }
IC CKinematicsAnimated* PKinematicsAnimated(IRenderVisual* V) { return V ? (CKinematicsAnimated*)V->dcast_PKinematicsAnimated() : 0; }
//---------------------------------------------------------------------------
#endif
