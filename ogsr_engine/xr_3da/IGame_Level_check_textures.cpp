#include "stdafx.h"

#include "IGame_Level.h"

void IGame_Level::LL_CheckTextures()
{
    xr::render_memory_usage usage;
    Device.m_pRender->ResourcesGetMemoryUsage(usage);

    Msg("* t-report - base: {}, {} Kb", usage.c_base, usage.m_base / 1024);
    Msg("* t-report - lmap: {}, {} Kb", usage.c_lmaps, usage.m_lmaps / 1024);
    Msg("* t-report - Lua: {} Kb", usage.lua / 1024);
}
