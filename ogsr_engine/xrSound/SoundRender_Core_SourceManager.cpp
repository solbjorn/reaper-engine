#include "stdafx.h"

#include "SoundRender_Core.h"
#include "SoundRender_Source.h"

CSoundRender_Source* CSoundRender_Core::i_create_source(LPCSTR name)
{
    // Search
    string256 id;
    xr_strcpy(id, name);
    strlwr(id);
    if (strext(id))
        *strext(id) = 0;

    s_sources_lock.lock();

    const auto it = s_sources.find(id);
    if (it != s_sources.end())
    {
        s_sources_lock.unlock();
        return it->second;
    }

    // Load a _new one
    CSoundRender_Source* S = xr_new<CSoundRender_Source>();
    S->load(id);

    s_sources_lock.lock();
    s_sources.emplace(id, S);
    s_sources_lock.unlock();

    return S;
}

void CSoundRender_Core::i_destroy_source(CSoundRender_Source* S)
{
    // No actual destroy at all
}
