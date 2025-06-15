#include "stdafx.h"

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"
#include "SoundRender_Target.h"

void CSoundRender_Emitter::start(ref_sound* _owner, BOOL _loop, float delay)
{
    starting_delay = delay;

    VERIFY(_owner);
    owner_data = _owner->_p;
    VERIFY(owner_data);

    p_source.position.set(0, 0, 0);
    p_source.min_distance = source()->m_fMinDist; // DS3D_DEFAULTMINDISTANCE;
    p_source.max_distance = source()->m_fMaxDist; // 300.f;
    p_source.base_volume = source()->m_fBaseVolume; // 1.f
    p_source.volume = 1.f;
    p_source.freq = 1.f;
    p_source.max_ai_distance = source()->m_fMaxAIDist; // 300.f;

    if (fis_zero(delay, EPS_L))
    {
        m_current_state = _loop ? stStartingLooped : stStarting;
    }
    else
    {
        m_current_state = _loop ? stStartingLoopedDelayed : stStartingDelayed;
        fTimeToPropagade = SoundRender->Timer.GetElapsed_sec();
    }
    bStopping = FALSE;
    bRewind = FALSE;

    // Calc storage
    for (auto& buf : temp_buf)
        buf.resize(source()->bytesPerBuffer);

    ovf = source()->open();
}

void CSoundRender_Emitter::i_stop()
{
    bRewind = FALSE;
    if (target)
        stop_target();

    wait_prefill();

    if (owner_data)
    {
        source()->close(ovf);
        Event_ReleaseOwner();
        VERIFY(this == owner_data->feedback);
        owner_data->feedback = NULL;
        owner_data = NULL;
    }
    m_current_state = stStopped;
    fStoppingSpeed_k = 1.f;
}

void CSoundRender_Emitter::stop(BOOL bDeffered, float speed_k)
{
    if (bDeffered)
    {
        bStopping = TRUE;
        fStoppingSpeed_k = speed_k;
    }
    else
        i_stop();
}

void CSoundRender_Emitter::rewind()
{
    bStopping = FALSE;
    bRewind = TRUE;
}

void CSoundRender_Emitter::pause(BOOL bVal, int id)
{
    if (bVal)
    {
        if (0 == iPaused)
            iPaused = id;
    }
    else
    {
        if (id == iPaused)
            iPaused = 0;
    }
}

void CSoundRender_Emitter::cancel()
{
    canceling = true;

    switch (m_current_state)
    {
    case stPlaying:
        stop_target();
        m_current_state = stSimulating;
        break;
    case stPlayingLooped:
        stop_target();
        m_current_state = stSimulatingLooped;
        break;
    default: FATAL("Non playing ref_sound forced out of render queue"); break;
    }

    canceling = false;
}

void CSoundRender_Emitter::stop_target()
{
    R_ASSERT(target);
    wait_prefill();

    target->stop();
    target = nullptr;

    source()->close(ovf);
    ovf = nullptr;
}
