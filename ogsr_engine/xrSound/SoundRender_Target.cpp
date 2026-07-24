#include "stdafx.h"

#include "SoundRender_Target.h"

#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Target::start(CSoundRender_Emitter* E)
{
    XR_ASSERT(E != nullptr);

    // *** Initial buffer startup ***
    // 1. Fill parameters
    // 4. Load 2 blocks of data (as much as possible)
    // 5. Deferred-play-signal (emitter-exist, rendering-false)
    m_pEmitter = E;
    rendering = false;
}

tmc::task<void> CSoundRender_Target::render()
{
    XR_ASSERT(!rendering);

    rendering = true;
    co_return;
}

void CSoundRender_Target::stop()
{
    rendering = false;
    m_pEmitter = nullptr;
}

tmc::task<void> CSoundRender_Target::rewind()
{
    XR_ASSERT(rendering);
    co_return;
}

tmc::task<void> CSoundRender_Target::update()
{
    XR_ASSERT(m_pEmitter != nullptr);
    co_return;
}
