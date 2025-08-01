#ifndef Kinematics_included
#define Kinematics_included
#pragma once

#include "RenderVisual.h"
#include "../../Layers/xrRender/KinematicsAddBoneTransform.h"

typedef void (*UpdateCallback)(IKinematics* P);

class CBoneData;
class IBoneData;
class IKinematicsAnimated;
class IRenderVisual;
class CBoneInstance;
struct VisMask;
struct SEnumVerticesCallback;

// 10 fps
#define UCalc_Interval (u32(100))

class IKinematics : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IKinematics);

public:
    struct pick_result
    {
        Fvector normal;
        float dist;
        Fvector tri[3];
    };

public:
    /************************* Add by Zander *******************************/
    // Устанавливает видимость меша с указанным индексом
    virtual void SetRFlag(const u32 id, const bool v) = 0;
    // Возвращает состояние видимости для меша с указанным индексом.
    virtual bool GetRFlag(const u32 id) const = 0;
    // Возвращает количество мешей в модели.
    virtual u32 RChildCount() const = 0;

    virtual Fvector3 RC_VisBox(const u32 id) = 0;
    virtual Fvector3 RC_VisCenter(const u32 id) = 0;
    virtual Fvector3 RC_VisBorderMin(const u32 id) = 0;
    virtual Fvector3 RC_VisBorderMax(const u32 id) = 0;
    virtual void RC_Dump() = 0; // Дамп иерархии модели, всех костей и всех мешей в лог.
    /************************* End Add *************************************/

    virtual void Bone_Calculate(CBoneData* bd, Fmatrix* parent) = 0;
    virtual void Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks) = 0;

    virtual bool PickBone(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id) = 0;
    virtual void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) = 0;

    // Low level interface
    virtual u16 LL_BoneID(const char* B) const = 0;
    virtual u16 LL_BoneID(const shared_str& B) const = 0;
    virtual const char* LL_BoneName_dbg(const u16 ID) const = 0;

    virtual CInifile* _BCL LL_UserData() = 0;

    virtual ICF CBoneInstance& _BCL LL_GetBoneInstance(u16 bone_id) = 0;

    virtual CBoneData& _BCL LL_GetData(u16 bone_id) = 0;

    virtual const IBoneData& _BCL GetBoneData(u16 bone_id) const = 0;

    virtual u16 _BCL LL_BoneCount() const = 0;
    virtual u16 LL_VisibleBoneCount() = 0;

    virtual ICF Fmatrix& _BCL LL_GetTransform(u16 bone_id) = 0;
    virtual ICF const Fmatrix& _BCL LL_GetTransform(u16 bone_id) const = 0;

    virtual ICF Fmatrix& LL_GetTransform_R(u16 bone_id) = 0;
    virtual Fobb& LL_GetBox(u16 bone_id) = 0;
    virtual const Fbox& _BCL GetBox() const = 0;
    virtual void LL_GetBindTransform(xr_vector<Fmatrix>& matrices) = 0;
    virtual int LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups) = 0;

    virtual u16 _BCL LL_GetBoneRoot() = 0;
    virtual void LL_SetBoneRoot(u16 bone_id) = 0;

    virtual BOOL _BCL LL_GetBoneVisible(u16 bone_id) = 0;
    virtual void LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive) = 0;

    virtual VisMask _BCL LL_GetBonesVisible() = 0;
    virtual void LL_SetBonesVisible(VisMask mask) = 0;

    virtual void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset) = 0; //--#SM+#--
    virtual void LL_ClearAdditionalTransform(u16 bone_id) = 0; //--#SM+#--

    // Main functionality
    virtual void CalculateBones(BOOL bForceExact = FALSE) = 0; // Recalculate skeleton
    virtual void CalculateBones_Invalidate() = 0;
    virtual void Callback(UpdateCallback C, void* Param) = 0;

    //	Callback: data manipulation
    virtual void SetUpdateCallback(UpdateCallback pCallback) = 0;
    virtual void SetUpdateCallbackParam(void* pCallbackParam) = 0;

    virtual UpdateCallback GetUpdateCallback() = 0;
    virtual void* GetUpdateCallbackParam() = 0;
    // UpdateCallback						Update_Callback;
    // void*								Update_Callback_Param;
    virtual IRenderVisual* dcast_RenderVisual() = 0;
    virtual IKinematicsAnimated* dcast_PKinematicsAnimated() = 0;

    // debug
#ifdef DEBUG
    virtual void DebugRender(Fmatrix& XFORM) = 0;
    virtual shared_str getDebugName() = 0;
#endif
};

IC IKinematics* PKinematics(IRenderVisual* V) { return V ? V->dcast_PKinematics() : 0; }

#endif //	Kinematics_included
