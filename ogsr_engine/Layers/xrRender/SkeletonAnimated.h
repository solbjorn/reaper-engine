//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include "skeletoncustom.h"
#include "animation.h"
#include "../../xr_3da/SkeletonMotions.h"

#include "../../Include/xrRender/KinematicsAnimated.h"

//*** Bone Instance *******************************************************************************

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

    [[nodiscard]] constexpr gsl::index mem_usage() const
    {
        gsl::index sz{sizeof(*this)};

        for (auto& it : Blend)
            sz += it->mem_usage();

        return sz;
    }
};

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

    // Calculation
private:
    void LL_BuldBoneMatrixDequatize(const CBoneData* bd, u8 channel_mask, SKeyTable& keys) override;
    void LL_BoneMatrixBuild(CBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys) override;
    void BuildBoneMatrix(const CBoneData* bd, CBoneInstance& bi, const Fmatrix* parent, u8 mask_channel = (1 << 0)) override;

public:
    void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset) override; //--#SM+#--
    void LL_ClearAdditionalTransform(u16 bone_id = BI_NONE) override; //--#SM+#--

    void OnCalculateBones() override;

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
    void IBoneInstances_Create() override;
    void IBoneInstances_Destroy() override;

    void IBlend_Startup();
    void ChannelFactorsStartup();
    CBlend* IBlend_Create();

private:
    void IBlendSetup(CBlend& B, u16 part, u8 channel, MotionID motion_ID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop,
                     PlayCallback Callback, LPVOID CallbackParam);
    void IFXBlendSetup(CBlend& B, MotionID motion_ID, float blendAccrue, float blendFalloff, float Power, float Speed, u16 bone);

public:
    [[nodiscard]] std::pair<gsl::czstring, gsl::czstring> LL_MotionDefName_dbg(MotionID ID) override;
    void LL_DumpBlends_dbg() override;

    [[nodiscard]] u32 LL_PartBlendsCount(u32 bone_part_id) override;
    [[nodiscard]] CBlend* LL_PartBlend(u32 bone_part_id, u32 n) override;
    void LL_IterateBlends(IterateBlendsCallback& callback) override;

    void SetUpdateTracksCalback(IUpdateTracksCallback* callback) override;
    [[nodiscard]] IUpdateTracksCallback* GetUpdateTracksCalback() override { return m_update_tracks_callback; }

    [[nodiscard]] u16 LL_MotionsSlotCount() override { return (u16)m_Motions.size(); }
    [[nodiscard]] const shared_motions& LL_MotionsSlot(u16 idx) override { return m_Motions[idx].motions; }

    [[nodiscard]] CMotionDef* LL_GetMotionDef(MotionID id) override { return m_Motions[id.slot].motions.motion_def(id.idx); }
    [[nodiscard]] CMotion* LL_GetRootMotion(MotionID id) override { return &m_Motions[id.slot].bone_motions[iRoot]->at(id.idx); }
    [[nodiscard]] CMotion* LL_GetMotion(MotionID id, u16 bone_id) override { return &m_Motions[id.slot].bone_motions[bone_id]->at(id.idx); }

    [[nodiscard]] IBlendDestroyCallback* GetBlendDestroyCallback() override;
    void SetBlendDestroyCallback(IBlendDestroyCallback* cb) override;
    // Low level interface
    [[nodiscard]] MotionID LL_MotionID(gsl::czstring B) override;
    [[nodiscard]] u16 LL_PartID(gsl::czstring B) override;

    CBlend* LL_PlayFX(u16 bone, MotionID motion, float blendAccrue, float blendFalloff, float Speed, float Power);
    [[nodiscard]] CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixing, f32 blendAccrue, f32 blendFalloff, f32 Speed, BOOL noloop,
                                       PlayCallback Callback, void* CallbackParam, u8 channel = 0) override;
    [[nodiscard]] CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixIn, PlayCallback Callback, void* CallbackParam, u8 channel = 0) override;
    void LL_FadeCycle(u16 partition, float falloff, u8 mask_channel = (1 << 0));
    void LL_CloseCycle(u16 partition, u8 mask_channel = (1 << 0)) override;
    void LL_SetChannelFactor(u16 channel, f32 factor) override;

    CBlendInstance& LL_GetBlendInstance(u16 bone_id)
    {
        ASSERT_FMT(bone_id < LL_BoneCount(), "!![%s] visual_name: [%s], invalid bone_id: [%u]", std::source_location::current().function_name(),
                   dbg_name.c_str(), bone_id);
        return blend_instances[bone_id];
    }

    // Main functionality
    void UpdateTracks() override; // Update motions
    void LL_UpdateTracks(f32 dt, bool b_force, bool leave_blends) override; // Update motions
    void LL_UpdateFxTracks(float dt);
    void DestroyCycle(CBlend& B);

    // cycles
    [[nodiscard]] MotionID ID_Cycle(const shared_str& N) override;
    [[nodiscard]] MotionID ID_Cycle_Safe(const shared_str& N) override;
    [[nodiscard]] CBlend* PlayCycle(const shared_str& N, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr,
                                    u8 channel = 0) override;
    [[nodiscard]] CBlend* PlayCycle(MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr, u8 channel = 0) override;
    [[nodiscard]] CBlend* PlayCycle(u16 partition, MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr,
                                    u8 channel = 0) override;
    // fx'es
    [[nodiscard]] MotionID ID_FX(std::string_view N) override;
    [[nodiscard]] MotionID ID_FX_Safe(std::string_view N) override;
    [[nodiscard]] CBlend* PlayFX(std::string_view N, f32 power_scale) override;
    [[nodiscard]] CBlend* PlayFX(MotionID M, f32 power_scale) override;

    [[nodiscard]] const CPartition& partitions() const override { return *m_Partition; }

    // General "Visual" stuff
    void Copy(dxRender_Visual* pFrom) override;
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Release() override;
    void Spawn() override;
    [[nodiscard]] IKinematicsAnimated* dcast_PKinematicsAnimated() override { return this; }
    [[nodiscard]] IRenderVisual* dcast_RenderVisual() override { return this; }
    [[nodiscard]] IKinematics* dcast_PKinematics() override { return this; }

    CKinematicsAnimated();
    ~CKinematicsAnimated() override;

    [[nodiscard]] gsl::index mem_usage(bool bInstance) const override
    {
        return CKinematics::mem_usage(bInstance) + gsl::index{sizeof(*this)} + (bInstance && blend_instances ? blend_instances->mem_usage() : 0);
    }

    IC const BlendSVec& blend_cycle(const u32& bone_part_id) const
    {
        VERIFY(bone_part_id < MAX_PARTS);
        return (blend_cycles[bone_part_id]);
    }

    [[nodiscard]] f32 get_animation_length(MotionID motion_ID) override;
};

inline CKinematicsAnimated* PKinematicsAnimated(IRenderVisual* V) { return V ? smart_cast<CKinematicsAnimated*>(V->dcast_PKinematicsAnimated()) : nullptr; }

//---------------------------------------------------------------------------
#endif
