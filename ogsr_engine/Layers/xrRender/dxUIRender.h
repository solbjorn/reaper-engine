#ifndef dxUIRender_included
#define dxUIRender_included

#include "../../Include/xrRender/UIRender.h"

class dxUIRender final : public IUIRender
{
    RTTI_DECLARE_TYPEINFO(dxUIRender, IUIRender);

public:
    dxUIRender() = default;
    ~dxUIRender() override = default;

    void CreateUIGeom() override;
    void DestroyUIGeom() override;

    void SetShader(IUIShader& shader) override;
    void SetScissor(Irect* rect = nullptr) override;
    void GetActiveTextureResolution(Fvector2& res) override;

    void PushPoint(f32 x, f32 y, f32 z, u32 C, f32 u, f32 v) override;

    void StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) override;
    void FlushPrimitive() override;

    [[nodiscard]] gsl::czstring UpdateShaderName(gsl::czstring tex_name, gsl::czstring sh_name) override;

    void CacheSetXformWorld(const Fmatrix& M) override;
    void CacheSetCullMode(CullMode) override;

private:
    ref_geom hGeom_TL;
    ref_geom hGeom_LIT;

    ePrimitiveType PrimitiveType{ptNone};
    ePointType m_PointType{pttNone};

    //	Vertex buffer attributes
    std::size_t m_iMaxVerts;

    std::span<FVF::TL> tl_verts;
    std::size_t tl_written;

    std::span<FVF::LIT> lit_verts;
    std::size_t lit_written;
};

extern dxUIRender UIRenderImpl;

#endif //	dxUIRender_included
