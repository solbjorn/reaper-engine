#ifndef IRenderDetailModelH
#define IRenderDetailModelH

#include "../../Include/xrRender/RenderDetailModel.h"

//////////////////////////////////////////////////////////////////////////
// definition (Detail Model)

class XR_NOVTABLE IRender_DetailModel : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRender_DetailModel);

public:
    struct fvfVertexIn final
    {
        Fvector P;
        float u, v;
    };
    static_assert(sizeof(fvfVertexIn) == 20);

    struct fvfVertexOut final
    {
        Fvector P;
        u32 C;
        float u, v;
    };
    static_assert(sizeof(fvfVertexOut) == 24);

    Fsphere bv_sphere;
    Fbox bv_bb;
    Flags32 m_Flags;
    float m_fMinScale;
    float m_fMaxScale;

    ref_shader shader;
    xr_vector<fvfVertexIn> vertices;
    xr_vector<u16> indices;

    ~IRender_DetailModel() override = 0;

    virtual void transfer(Fmatrix& mXform, std::span<fvfVertexOut> vDest, u32 C, std::span<u16> iDest, u32 iOffset, f32 du = 0.0f, f32 dv = 0.0f) const = 0;
};

inline IRender_DetailModel::~IRender_DetailModel() = default;

#endif
