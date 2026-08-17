#include "stdafx.h"

#ifdef DEBUG
#include "dxObjectSpaceRender.h"

dxObjectSpaceRender::dxObjectSpaceRender() { m_shDebug.create("debug\\wireframe", "$null"); }
dxObjectSpaceRender::~dxObjectSpaceRender() { m_shDebug.destroy(); }

void dxObjectSpaceRender::Copy(IObjectSpaceRender& _in) { *this = *(dxObjectSpaceRender*)&_in; }
void dxObjectSpaceRender::dbgAddSphere(const Fsphere& sphere, u32 colour) { dbg_S.emplace_back(sphere, colour); }

void dxObjectSpaceRender::dbgRender()
{
    XR_ASSERT(bDebug);

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    cmd_list.set_Shader(m_shDebug);

    for (auto& obb : q_debug.boxes)
    {
        Fmatrix X;
        obb.xform_get(X);

        cmd_list.dbg_DrawOBB(X, obb.m_halfsize, D3DCOLOR_XRGB(255, 0, 0));

        Fmatrix S;
        S.scale(obb.m_halfsize);
        Fmatrix R;
        R.mul(X, S);

        cmd_list.dbg_DrawEllipse(R, D3DCOLOR_XRGB(0, 0, 255));
    }

    q_debug.boxes.clear();

    for (auto& P : dbg_S)
    {
        Fsphere& S = P.first;
        Fmatrix M;
        M.scale(S.R, S.R, S.R);
        M.translate_over(S.P);

        cmd_list.dbg_DrawEllipse(M, P.second);
    }

    dbg_S.clear();
}

void dxObjectSpaceRender::SetShader() { RImplementation.get_imm_context().cmd_list.set_Shader(m_shDebug); }
#endif // DEBUG
