#ifndef KinematicsAnimated_included
#define KinematicsAnimated_included

#include "../../xr_3da/SkeletonMotions.h"
#include "animation_blend.h"

#include "../../layers/xrrender/KinematicAnimatedDefs.h"
#include "../../Layers/xrRender/KinematicsAddBoneTransform.h"

class IKinematics;
class CBlend;
class CKinematicsAnimated;
class CBoneInstanceAnimated;
struct CKey;
class CInifile;
class IKinematicsAnimated;
class IRenderVisual;

struct IterateBlendsCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IterateBlendsCallback);

public:
    ~IterateBlendsCallback() override = 0;

    virtual void operator()(CBlend& B) = 0;
};

inline IterateBlendsCallback::~IterateBlendsCallback() = default;

struct IUpdateTracksCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUpdateTracksCallback);

public:
    ~IUpdateTracksCallback() override = 0;

    virtual bool operator()(float dt, IKinematicsAnimated& k) = 0;
};

inline IUpdateTracksCallback::~IUpdateTracksCallback() = default;

struct SKeyTable
{
    CKey keys[MAX_CHANNELS][MAX_BLENDED]; // all keys
    CBlend* blends[MAX_CHANNELS][MAX_BLENDED]; // blend pointers
    int chanel_blend_conts[MAX_CHANNELS]; // channel counts

    SKeyTable() { std::fill_n(chanel_blend_conts, MAX_CHANNELS, 0); }
};

class XR_NOVTABLE IKinematicsAnimated : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IKinematicsAnimated);

public:
    ~IKinematicsAnimated() override = 0;

    // Calculation
    virtual void OnCalculateBones() = 0;

    [[nodiscard]] virtual std::pair<gsl::czstring, gsl::czstring> LL_MotionDefName_dbg(MotionID ID) = 0;
    virtual void LL_DumpBlends_dbg() = 0;

    [[nodiscard]] virtual u32 LL_PartBlendsCount(u32 bone_part_id) = 0;
    [[nodiscard]] virtual CBlend* LL_PartBlend(u32 bone_part_id, u32 n) = 0;
    virtual void LL_IterateBlends(IterateBlendsCallback& callback) = 0;

    [[nodiscard]] virtual u16 LL_MotionsSlotCount() = 0;
    [[nodiscard]] virtual const shared_motions& LL_MotionsSlot(u16 idx) = 0;

    [[nodiscard]] virtual CMotionDef* LL_GetMotionDef(MotionID id) = 0;
    [[nodiscard]] virtual CMotion* LL_GetRootMotion(MotionID id) = 0;
    [[nodiscard]] virtual CMotion* LL_GetMotion(MotionID id, u16 bone_id) = 0;
    // interface for procedural animations :)
    virtual void LL_BuldBoneMatrixDequatize(const CBoneData* bd, u8 channel_mask, SKeyTable& keys) = 0;
    virtual void LL_BoneMatrixBuild(CBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys) = 0;

    [[nodiscard]] virtual IBlendDestroyCallback* GetBlendDestroyCallback() = 0;
    virtual void SetBlendDestroyCallback(IBlendDestroyCallback* cb) = 0;
    virtual void SetUpdateTracksCalback(IUpdateTracksCallback* callback) = 0;
    [[nodiscard]] virtual IUpdateTracksCallback* GetUpdateTracksCalback() = 0;

    // Low level interface
    [[nodiscard]] virtual MotionID LL_MotionID(gsl::czstring B) = 0;
    [[nodiscard]] virtual u16 LL_PartID(gsl::czstring B) = 0;

    [[nodiscard]] virtual CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixing, f32 blendAccrue, f32 blendFalloff, f32 Speed, BOOL noloop,
                                               PlayCallback Callback, void* CallbackParam, u8 channel = 0) = 0;
    [[nodiscard]] virtual CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixIn, PlayCallback Callback, void* CallbackParam, u8 channel = 0) = 0;
    virtual void LL_CloseCycle(u16 partition, u8 mask_channel = (1 << 0)) = 0;
    virtual void LL_SetChannelFactor(u16 channel, f32 factor) = 0;

    // Main functionality
    virtual void UpdateTracks() = 0; // Update motions
    virtual void LL_UpdateTracks(f32 dt, bool b_force, bool leave_blends) = 0; // Update motions

    // cycles
    [[nodiscard]] virtual MotionID ID_Cycle(const shared_str& N) = 0;
    [[nodiscard]] virtual MotionID ID_Cycle_Safe(const shared_str& N) = 0;
    [[nodiscard]] virtual CBlend* PlayCycle(const shared_str& N, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr,
                                            u8 channel = 0) = 0;
    [[nodiscard]] virtual CBlend* PlayCycle(MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr, u8 channel = 0) = 0;
    [[nodiscard]] virtual CBlend* PlayCycle(u16 partition, MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, void* CallbackParam = nullptr,
                                            u8 channel = 0) = 0;
    // fx'es
    [[nodiscard]] virtual MotionID ID_FX(std::string_view N) = 0;
    [[nodiscard]] virtual MotionID ID_FX_Safe(std::string_view N) = 0;
    [[nodiscard]] virtual CBlend* PlayFX(std::string_view N, f32 power_scale) = 0;
    [[nodiscard]] virtual CBlend* PlayFX(MotionID M, f32 power_scale) = 0;

    [[nodiscard]] virtual const CPartition& partitions() const = 0;

    [[nodiscard]] virtual IRenderVisual* dcast_RenderVisual() = 0;
    [[nodiscard]] virtual IKinematics* dcast_PKinematics() = 0;

    [[nodiscard]] virtual f32 get_animation_length(MotionID motion_ID) = 0;
};

inline IKinematicsAnimated::~IKinematicsAnimated() = default;

#endif //	KinematicsAnimated_included
