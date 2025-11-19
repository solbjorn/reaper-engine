#ifndef DebugRender_included
#define DebugRender_included

// #ifdef DEBUG

#include "DebugShader.h"

class XR_NOVTABLE IDebugRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IDebugRender);

public:
    enum CullMode
    {
        cmNONE = 0,
        cmCW,
        cmCCW,
    };

    enum dbgShaderHandle
    {
        dbgShaderWindow = 0,
        dbgShaderCount
    };

public:
    ~IDebugRender() override = 0;

    virtual void Render() = 0;

    virtual void add_lines(Fvector const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color, bool hud_mode = false) = 0;

    virtual void ZEnable(bool bEnable) = 0;
    virtual void OnFrameEnd() = 0;
    virtual void SetShader(const debug_shader& shader) = 0;
    // virtual void CacheSetXformWorld(const Fmatrix& M) = 0;
    // virtual void CacheSetCullMode(CullMode) = 0;
    virtual void SetAmbient(u32 colour) = 0;

    // Shaders
    virtual void SetDebugShader(dbgShaderHandle shdHandle) = 0;
    virtual void DestroyDebugShader(dbgShaderHandle shdHandle) = 0;

    virtual void dbg_DrawTRI(const Fmatrix& T, const Fvector& p1, const Fvector& p2, const Fvector& p3, u32 C) = 0;
};

inline IDebugRender::~IDebugRender() = default;

// #endif // DEBUG

#endif //	DebugRender_included
