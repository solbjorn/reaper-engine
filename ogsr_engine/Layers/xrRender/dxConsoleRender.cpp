#include "stdafx.h"
#include "dxConsoleRender.h"

dxConsoleRender::dxConsoleRender()
{
    m_Shader.create("hud\\crosshair");
    m_Geom.create(FVF::F_TL, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
}

void dxConsoleRender::Copy(IConsoleRender& _in) { *this = *(dxConsoleRender*)&_in; }

void dxConsoleRender::OnRender(bool bGame)
{
    VERIFY(HW.pDevice);

    D3DRECT R{0L, 0L, (LONG)Device.dwWidth, (LONG)Device.dwHeight};

    if (bGame)
        R.y2 /= 2;

    u32 vOffset = 0;
    //	TODO: DX10: Implement console background clearing for DX10
    FVF::TL* verts = (FVF::TL*)RImplementation.Vertex.Lock(4, m_Geom->vb_stride, vOffset);
    verts->set(static_cast<int>(R.x1), static_cast<int>(R.y2), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts++;
    verts->set(static_cast<int>(R.x1), static_cast<int>(R.y1), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts++;
    verts->set(static_cast<int>(R.x2), static_cast<int>(R.y2), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts++;
    verts->set(static_cast<int>(R.x2), static_cast<int>(R.y1), D3DCOLOR_XRGB(32, 32, 32), 0, 0);
    verts++;
    RImplementation.Vertex.Unlock(4, m_Geom->vb_stride);

    RCache.set_Element(m_Shader->E[0]);
    RCache.set_Geometry(m_Geom);

    RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, 4, 0, 2);
}
