#pragma once

#include "../../Include/xrRender/DebugRender.h"

class dxDebugRender : public IDebugRender
{
    RTTI_DECLARE_TYPEINFO(dxDebugRender, IDebugRender);

public:
    dxDebugRender() = default;
    ~dxDebugRender() override = default;

    void Render() override;
    void add_lines(Fvector3 const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color,
                   bool hud_mode = false) override;

    void ZEnable(bool bEnable) override;
    void OnFrameEnd() override;
    void SetShader(const debug_shader& shader) override;
    void SetAmbient(u32 colour) override;

    // Shaders
    void SetDebugShader(dbgShaderHandle shdHandle) override;
    void DestroyDebugShader(dbgShaderHandle shdHandle) override;

    void dbg_DrawTRI(const Fmatrix& T, const Fvector& p1, const Fvector& p2, const Fvector& p3, u32 C) override;

private:
    xr_unordered_map<u32, xr_vector<FVF::L>> m_line_vertices;
    xr_unordered_map<u32, xr_vector<u16>> m_line_indices;

    xr_unordered_map<u32, xr_vector<FVF::L>> m_line_vertices_hud;
    xr_unordered_map<u32, xr_vector<u16>> m_line_indices_hud;

    ref_shader m_dbgShaders[dbgShaderCount];
};

extern dxDebugRender DebugRenderImpl;

#ifdef DEBUG
extern dxDebugRender* rdebug_render;
#endif
