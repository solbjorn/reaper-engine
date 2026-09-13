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
}

void CResourceManager::reset_end()
{
    // create RDStreams
    RImplementation.CreateQuadIB();

    for (auto geom : v_geoms)
    {
        if (geom->ib == RImplementation.old_QuadIB)
            geom->ib = RImplementation.QuadIB;
    }

    // create RTs in the same order as them was first created
    xr_vector<CRT*> sorted_rts;
    sorted_rts.reserve(m_rtargets.size());

    for (auto& rt_pair : m_rtargets)
        sorted_rts.push_back(rt_pair.second);

    std::ranges::sort(sorted_rts, {}, &CRT::_order);

    for (CRT* rt : sorted_rts)
        rt->reset_end();

    // create state-blocks
    for (SState* sstate : v_states)
        sstate->state = ID3DState::Create(sstate->state_code);

    // create everything, renderer may use
    RImplementation.reset_end();
    Dump(true);
}

namespace
{
template <typename C>
void mdump(C c)
{
    if (c.empty())
        return;

    for (auto& I : c)
        XR_LOG_INFO("       : {:3}: {}", I.second->ref_count, I.second->cName);
}
} // namespace

CResourceManager::~CResourceManager() { Dump(false); }

void CResourceManager::Dump(bool bBrief)
{
    XR_LOG_INFO("RM_Dump: textures  : {}", m_textures.size());
    if (!bBrief)
        mdump(m_textures);

    XR_LOG_INFO("RM_Dump: rtargets  : {}", m_rtargets.size());
    if (!bBrief)
        mdump(m_rtargets);

    XR_LOG_INFO("RM_Dump: vs        : {}", m_vs.size());
    if (!bBrief)
        mdump(m_vs);

    XR_LOG_INFO("RM_Dump: ps        : {}", m_ps.size());
    if (!bBrief)
        mdump(m_ps);

    XR_LOG_INFO("RM_Dump: gs        : {}", m_gs.size());
    if (!bBrief)
        mdump(m_gs);

    XR_LOG_INFO("RM_Dump: cs        : {}", m_cs.size());
    if (!bBrief)
        mdump(m_cs);

    XR_LOG_INFO("RM_Dump: hs        : {}", m_hs.size());
    if (!bBrief)
        mdump(m_hs);

    XR_LOG_INFO("RM_Dump: ds        : {}", m_ds.size());
    if (!bBrief)
        mdump(m_ds);

    XR_LOG_INFO("RM_Dump: dcl       : {}", v_declarations.size());
    XR_LOG_INFO("RM_Dump: states    : {}", v_states.size());
    XR_LOG_INFO("RM_Dump: tex_list  : {}", lst_textures.size());
    XR_LOG_INFO("RM_Dump: v_passes  : {}", v_passes.size());
    XR_LOG_INFO("RM_Dump: v_elements: {}", v_elements.size());
    XR_LOG_INFO("RM_Dump: v_shaders : {}", v_shaders.size());
}
