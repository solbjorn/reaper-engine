#include "stdafx.h"

#include "dxStatsRender.h"

#include "../../xr_3da/GameFont.h"

void dxStatsRender::Copy(IStatsRender&) {}

void dxStatsRender::OutData1(CGameFont& F)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    F.OutNext("VERT:        {}/{}", cmd_list.stat.verts, cmd_list.stat.calls ? cmd_list.stat.verts / cmd_list.stat.calls : 0);
    F.OutNext("POLY:        {}/{}", cmd_list.stat.polys, cmd_list.stat.calls ? cmd_list.stat.polys / cmd_list.stat.calls : 0);
    F.OutNext("DIP/DP:      {}", cmd_list.stat.calls);
}

void dxStatsRender::OutData2([[maybe_unused]] CGameFont& F)
{
#ifdef DEBUG
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    F.OutNext("SH/T/M/C:    {}/{}/{}/{}", cmd_list.stat.states, cmd_list.stat.textures, cmd_list.stat.matrices, cmd_list.stat.constants);
    F.OutNext("RT/PS/VS:    {}/{}/{}", cmd_list.stat.target_rt, cmd_list.stat.ps, cmd_list.stat.vs);
    F.OutNext("DCL/VB/IB:   {}/{}/{}", cmd_list.stat.decl, cmd_list.stat.vb, cmd_list.stat.ib);
#endif
}

void dxStatsRender::OutData3(CGameFont& F) { F.OutNext("xforms:      {}", RImplementation.get_imm_context().cmd_list.stat.xforms); }

void dxStatsRender::OutData4(CGameFont& F)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    F.OutNext("static:        {:3.1f}/{}", cmd_list.stat.r.s_static.verts / 1024.f, cmd_list.stat.r.s_static.dips);
    F.OutNext("flora:         {:3.1f}/{}", cmd_list.stat.r.s_flora.verts / 1024.f, cmd_list.stat.r.s_flora.dips);
    F.OutNext("  flora_lods:  {:3.1f}/{}", cmd_list.stat.r.s_flora_lods.verts / 1024.f, cmd_list.stat.r.s_flora_lods.dips);
    F.OutNext("dynamic:       {:3.1f}/{}", cmd_list.stat.r.s_dynamic.verts / 1024.f, cmd_list.stat.r.s_dynamic.dips);
    F.OutNext("  dynamic_inst:{:3.1f}/{}", cmd_list.stat.r.s_dynamic_inst.verts / 1024.f, cmd_list.stat.r.s_dynamic_inst.dips);
    F.OutNext("  dynamic_1B:  {:3.1f}/{}", cmd_list.stat.r.s_dynamic_1B.verts / 1024.f, cmd_list.stat.r.s_dynamic_1B.dips);
    F.OutNext("  dynamic_2B:  {:3.1f}/{}", cmd_list.stat.r.s_dynamic_2B.verts / 1024.f, cmd_list.stat.r.s_dynamic_2B.dips);
    F.OutNext("  dynamic_3B:  {:3.1f}/{}", cmd_list.stat.r.s_dynamic_3B.verts / 1024.f, cmd_list.stat.r.s_dynamic_3B.dips);
    F.OutNext("  dynamic_4B:  {:3.1f}/{}", cmd_list.stat.r.s_dynamic_4B.verts / 1024.f, cmd_list.stat.r.s_dynamic_4B.dips);
    F.OutNext("details:       {:3.1f}/{}", cmd_list.stat.r.s_details.verts / 1024.f, cmd_list.stat.r.s_details.dips);
}

void dxStatsRender::GuardVerts(CGameFont& F)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (cmd_list.stat.verts > 500000)
        F.OutNext("Verts     > 500k: {}", cmd_list.stat.verts);
}

void dxStatsRender::GuardDrawCalls(CGameFont& F)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (cmd_list.stat.calls > 1000)
        F.OutNext("DIP/DP    > 1k:   {}", cmd_list.stat.calls);
}

void dxStatsRender::SetDrawParams(IRenderDeviceRender* pRender)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Shader(smart_cast<dxRenderDeviceRender*>(pRender)->m_SelectionShader);

    constexpr Fvector4 tfactor{1, 1, 1, 1};
    cmd_list.set_c("tfactor", tfactor);
}
