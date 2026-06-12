#ifndef UIRender_included
#define UIRender_included

class IUIShader;

class XR_NOVTABLE IUIRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IUIRender);

public:
    enum ePrimitiveType
    {
        ptNone = -1,
        ptTriList,
        ptTriStrip,
        ptLineStrip,
        ptLineList
    };

    enum ePointType
    {
        pttNone = -1,
        pttTL,
        pttLIT
    };

    enum CullMode
    {
        cmNONE = 0,
        cmCW,
        cmCCW,
    };

public:
    ~IUIRender() override = 0;

    virtual void CreateUIGeom() = 0;
    virtual void DestroyUIGeom() = 0;

    virtual void SetShader(IUIShader& shader) = 0;
    virtual void SetAlphaRef(s32 aref) = 0;

    virtual void SetScissor(Irect* rect = nullptr) = 0;
    virtual void GetActiveTextureResolution(Fvector2& res) = 0;

    virtual void PushPoint(f32 x, f32 y, f32 z, u32 C, f32 u, f32 v) = 0;

    virtual void StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) = 0;
    virtual void FlushPrimitive() = 0;

    [[nodiscard]] virtual gsl::czstring UpdateShaderName(gsl::czstring tex_name, gsl::czstring sh_name) = 0;

    virtual void CacheSetXformWorld(const Fmatrix& M) = 0;
    virtual void CacheSetCullMode(CullMode) = 0;
};

inline IUIRender::~IUIRender() = default;

#endif //	UIRender_included
