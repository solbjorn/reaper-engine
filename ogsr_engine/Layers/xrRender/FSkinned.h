// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FSkinnedH
#define FSkinnedH

#include "FVisual.h"
#include "FProgressive.h"
#include "SkeletonX.h"

struct SEnumVerticesCallback;

class XR_NOVTABLE CSkeletonX_ext : public CSkeletonX // shared code for SkeletonX derivates
{
    RTTI_DECLARE_TYPEINFO(CSkeletonX_ext, CSkeletonX);

public:
    ~CSkeletonX_ext() override = 0;

protected:
    void _Load_hw(Fvisual& V, const void* data) override;
    void _CollectBoneFaces(Fvisual* V, u32 iBase, u32 iCount) override;
    void _EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id, u32 iBase) const;
    void _FillVerticesHW1W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, Fvisual* V, u16* indices,
                           CBoneData::FacesVec& faces) override;
    void _FillVerticesHW2W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, Fvisual* V, u16* indices,
                           CBoneData::FacesVec& faces) override;
    void _FillVerticesHW3W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, Fvisual* V, u16* indices,
                           CBoneData::FacesVec& faces) override;
    void _FillVerticesHW4W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, Fvisual* V, u16* indices,
                           CBoneData::FacesVec& faces) override;
    void _FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, u16 bone_id, u32 iBase) override;

    [[nodiscard]] BOOL _PickBoneHW1W(IKinematics::pick_result& r, f32 range, const Fvector3& S, const Fvector3& D, Fvisual* V, u16* indices,
                                     CBoneData::FacesVec& faces) override;
    [[nodiscard]] BOOL _PickBoneHW2W(IKinematics::pick_result& r, f32 range, const Fvector3& S, const Fvector3& D, Fvisual* V, u16* indices,
                                     CBoneData::FacesVec& faces) override;
    [[nodiscard]] BOOL _PickBoneHW3W(IKinematics::pick_result& r, f32 range, const Fvector3& S, const Fvector3& D, Fvisual* V, u16* indices,
                                     CBoneData::FacesVec& faces) override;
    [[nodiscard]] BOOL _PickBoneHW4W(IKinematics::pick_result& r, f32 range, const Fvector3& S, const Fvector3& D, Fvisual* V, u16* indices,
                                     CBoneData::FacesVec& faces) override;

    [[nodiscard]] BOOL _PickBone(IKinematics::pick_result& r, f32 range, const Fvector3& S, const Fvector3& D, u16 bone_id, u32 iBase) override;
};

inline CSkeletonX_ext::~CSkeletonX_ext() = default;

class CSkeletonX_ST : public Fvisual, public CSkeletonX_ext
{
    RTTI_DECLARE_TYPEINFO(CSkeletonX_ST, Fvisual, CSkeletonX_ext);

private:
    typedef Fvisual inherited1;
    typedef CSkeletonX_ext inherited2;

public:
    CSkeletonX_ST() = default;
    ~CSkeletonX_ST() override = default;

    void Render(CBackend& cmd_list, f32 LOD, bool) override;
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
    void AfterLoad(CKinematics* parent, u16 child_idx) override;
    void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) override;
    [[nodiscard]] BOOL PickBone(IKinematics::pick_result& r, f32 dist, const Fvector3& start, const Fvector3& dir, u16 bone_id) override;
    void FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, u16 bone_id) override;
};

class CSkeletonX_PM : public FProgressive, public CSkeletonX_ext
{
    RTTI_DECLARE_TYPEINFO(CSkeletonX_PM, FProgressive, CSkeletonX_ext);

private:
    typedef FProgressive inherited1;
    typedef CSkeletonX_ext inherited2;

public:
    CSkeletonX_PM() = default;
    ~CSkeletonX_PM() override = default;

    void Render(CBackend& cmd_list, f32 LOD, bool) override;
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
    void AfterLoad(CKinematics* parent, u16 child_idx) override;
    void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) override;
    [[nodiscard]] BOOL PickBone(IKinematics::pick_result& r, f32 dist, const Fvector3& start, const Fvector3& dir, u16 bone_id) override;
    void FillVertices(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector3& normal, f32 size, u16 bone_id) override;
};

#endif // FSkinnedH
