#include "stdafx.h"

#include "SoundRender_Target.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Target::start(CSoundRender_Emitter* E)
{
    R_ASSERT(E);

    // *** Initial buffer startup ***
    // 1. Fill parameters
    // 4. Load 2 blocks of data (as much as possible)
    // 5. Deferred-play-signal (emitter-exist, rendering-false)
    m_pEmitter = E;
    priority = E->priority();
    rendering = false;
}

void CSoundRender_Target::render()
{
    VERIFY(!rendering);
    rendering = true;
}

void CSoundRender_Target::stop()
{
    m_pEmitter = nullptr;
    rendering = false;
    priority = -1;
}

void CSoundRender_Target::rewind() { R_ASSERT(rendering); }
void CSoundRender_Target::update() { R_ASSERT(m_pEmitter); }
