#include "stdafx.h"

#include "dxConsoleRender.h"

dxConsoleRender::dxConsoleRender()
{
    m_Shader.create("hud\\crosshair");

    m_Geom.create(FVF::F_TL, SGeometry::default_vb(), RImplementation.QuadIB);
    XR_ASSERT(m_Geom.stride() == sizeof(FVF::TL));
}

void dxConsoleRender::Copy(IConsoleRender& _in)
{
    auto& in{*smart_cast<const dxConsoleRender*>(&_in)};

    m_Shader = in.m_Shader;
    m_Geom = in.m_Geom;
}

void dxConsoleRender::OnRender(bool bGame)
{
    XR_ASSERT(HW.pDevice != nullptr);

    D3DRECT R{0L, 0L, (LONG)Device.dwWidth, (LONG)Device.dwHeight};
    if (bGame)
        R.y2 /= 2;

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    //	TODO: DX10: Implement console background clearing for DX10
    verts[0].set(gsl::narrow_cast<s32>(R.x1), gsl::narrow_cast<s32>(R.y2), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts[1].set(gsl::narrow_cast<s32>(R.x1), gsl::narrow_cast<s32>(R.y1), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts[2].set(gsl::narrow_cast<s32>(R.x2), gsl::narrow_cast<s32>(R.y2), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts[3].set(gsl::narrow_cast<s32>(R.x2), gsl::narrow_cast<s32>(R.y1), D3DCOLOR_XRGB(32, 32, 32), 0, 0);

    const auto vOffset = cmd_list.Vertex.Unlock<FVF::TL>(4);

    cmd_list.set_Element(m_Shader->E[0]);
    cmd_list.set_Geometry(m_Geom);

    cmd_list.Render(D3DPT_TRIANGLELIST, vOffset, 0, 4, 0, 2);
}
