#include "stdafx.h"

#include "ResourceManager.h"
#include "../../xr_3da/Render.h"

void CResourceManager::reset_begin()
{
    // destroy everything, renderer may use
    RImplementation.reset_begin();

    // destroy state-blocks
    for (SState* sstate : v_states)
        _RELEASE(sstate->state);

    // destroy RTs
    for (auto& rt_pair : m_rtargets)
        rt_pair.second->reset_begin();

    // destroy DStreams
    RImplementation.old_QuadIB = RImplementation.QuadIB;
    HW.stats_manager.decrement_stats_ib(RImplementation.QuadIB);
    _RELEASE(RImplementation.QuadIB);

    RImplementation.Index.reset_begin();
    RImplementation.Vertex.reset_begin();
}

void CResourceManager::reset_end()
{
    // create RDStreams
    RImplementation.Vertex.reset_end();
    RImplementation.Index.reset_end();
    Evict();
    RImplementation.CreateQuadIB();

    // remark geom's which point to dynamic VB/IB
    {
        for (u32 _it = 0; _it < v_geoms.size(); _it++)
        {
            SGeometry* _G = v_geoms[_it];
            if (_G->vb == RImplementation.Vertex.old_pVB)
                _G->vb = RImplementation.Vertex.Buffer();

            // Here we may recover the buffer using one of
            // RCache's index buffers.
            // Do not remove else.
            if (_G->ib == RImplementation.Index.old_pIB)
            {
                _G->ib = RImplementation.Index.Buffer();
            }
            else if (_G->ib == RImplementation.old_QuadIB)
            {
                _G->ib = RImplementation.QuadIB;
            }
        }
    }

    // create RTs in the same order as them was first created
    xr_vector<CRT*> sorted_rts;
    sorted_rts.reserve(m_rtargets.size());

    for (auto& rt_pair : m_rtargets)
        sorted_rts.push_back(rt_pair.second);

    std::ranges::sort(sorted_rts, [](const CRT* A, const CRT* B) { return A->_order < B->_order; });

    for (CRT* rt : sorted_rts)
        rt->reset_end();

    // create state-blocks
    for (SState* sstate : v_states)
        sstate->state = ID3DState::Create(sstate->state_code);

    // create everything, renderer may use
    RImplementation.reset_end();
    Dump(true);
}

template <class C>
void mdump(C c)
{
    if (0 == c.size())
        return;
    for (auto I = c.begin(); I != c.end(); I++)
        Msg("*        : %3d: %s", I->second->dwReference, I->second->cName.c_str());
}

CResourceManager::~CResourceManager() { Dump(false); }

void CResourceManager::Dump(bool bBrief)
{
    Msg("* RM_Dump: textures  : %d", m_textures.size());
    if (!bBrief)
        mdump(m_textures);
    Msg("* RM_Dump: rtargets  : %d", m_rtargets.size());
    if (!bBrief)
        mdump(m_rtargets);
    //	DX10 cut 	Msg		("* RM_Dump: rtargetsc : %d",		m_rtargets_c.size());	if(!bBrief) mdump(m_rtargets_c);
    Msg("* RM_Dump: vs        : %d", m_vs.size());
    if (!bBrief)
        mdump(m_vs);
    Msg("* RM_Dump: ps        : %d", m_ps.size());
    if (!bBrief)
        mdump(m_ps);
    Msg("* RM_Dump: gs        : %d", m_gs.size());
    if (!bBrief)
        mdump(m_gs);
    Msg("* RM_Dump: cs        : %d", m_cs.size());
    if (!bBrief)
        mdump(m_cs);
    Msg("* RM_Dump: hs        : %d", m_hs.size());
    if (!bBrief)
        mdump(m_hs);
    Msg("* RM_Dump: ds        : %d", m_ds.size());
    if (!bBrief)
        mdump(m_ds);
    Msg("* RM_Dump: dcl       : %d", v_declarations.size());
    Msg("* RM_Dump: states    : %d", v_states.size());
    Msg("* RM_Dump: tex_list  : %d", lst_textures.size());
    Msg("* RM_Dump: v_passes  : %d", v_passes.size());
    Msg("* RM_Dump: v_elements: %d", v_elements.size());
    Msg("* RM_Dump: v_shaders : %d", v_shaders.size());
}
