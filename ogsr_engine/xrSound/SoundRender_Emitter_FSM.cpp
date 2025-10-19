#include "stdafx.h"

#include "SoundRender_Emitter.h"
#include "SoundRender_Core.h"
#include "SoundRender_Source.h"
#include "SoundRender_Target.h"

namespace
{
constexpr float psSoundCull{0.01f};
constexpr float TIME_TO_STOP_INFINITE{std::numeric_limits<float>::max()};

inline u32 calc_cursor(const float& fTimeStarted, float& fTime, const float& fTimeTotal, const float& fFreq, const WAVEFORMATEX& wfx)
{
    if (fTime < fTimeStarted)
        fTime = fTimeStarted; // Андрюха посоветовал, ассерт что ниже вылетел из за паузы как то хитро
    R_ASSERT((fTime - fTimeStarted) >= 0.0f);
    while ((fTime - fTimeStarted) > fTimeTotal / fFreq) // looped
    {
        fTime -= fTimeTotal / fFreq;
    }
    const u32 curr_sample_num = iFloor((fTime - fTimeStarted) * fFreq * wfx.nSamplesPerSec);
    return curr_sample_num * (wfx.wBitsPerSample / 8) * wfx.nChannels;
}
} // namespace

void CSoundRender_Emitter::update(float fTime, float dt)
{
    VERIFY2(!!(owner_data) || (!(owner_data) && (m_current_state == stStopped)), "owner");
    VERIFY2(owner_data ? *(int*)(&owner_data->feedback) : 1, "owner");

    if (bRewind)
    {
        wait_prefill();

        const float time = SoundRender->Timer.GetElapsed_sec();
        const float diff = time - fTimeStarted;
        fTimeStarted += diff;
        fTimeToStop += diff;
        fTimeToPropagade = time;

        set_cursor(0);
        if (target)
        {
            fill_all_blocks();
            target->rewind();
            dispatch_prefill();
        }
        bRewind = FALSE;
    }

    switch (m_current_state)
    {
    case stStopped: break;
    case stStartingDelayed:
        if (iPaused)
            break;
        starting_delay -= dt;
        if (starting_delay <= 0)
            m_current_state = stStarting;
        break;
    case stStarting:
        if (iPaused)
            break;
        fTimeStarted = fTime;
        fTimeToStop = fTime + (get_length_sec() / p_source.freq); //--#SM+#--
        fTimeToPropagade = fTime;
        fade_volume = 1.f;
        initStartingVolumes();

        if (update_culling(dt))
        {
            m_current_state = stPlaying;
            set_cursor(0);
            SoundRender->i_start(this);
            dispatch_prefill();
        }
        else
            m_current_state = stSimulating;
        break;
    case stStartingLoopedDelayed:
        if (iPaused)
            break;
        starting_delay -= dt;
        if (starting_delay <= 0)
            m_current_state = stStartingLooped;
        break;
    case stStartingLooped:
        if (iPaused)
            break;
        fTimeStarted = fTime;
        fTimeToStop = TIME_TO_STOP_INFINITE;
        fTimeToPropagade = fTime;
        fade_volume = 1.f;
        initStartingVolumes();

        if (update_culling(dt))
        {
            m_current_state = stPlayingLooped;
            set_cursor(0);
            SoundRender->i_start(this);
            dispatch_prefill();
        }
        else
            m_current_state = stSimulatingLooped;
        break;
    case stPlaying:
        if (iPaused)
        {
            stop_target();
            m_current_state = stSimulating;

            fTimeStarted += dt;
            fTimeToStop += dt;
            fTimeToPropagade += dt;
            break;
        }
        if (fTime >= fTimeToStop)
        {
            // STOP
            stop_target();
            m_current_state = stStopped;
        }
        else
        {
            if (!update_culling(dt))
            {
                // switch to: SIMULATE
                stop_target();
                m_current_state = stSimulating;
            }
            else
            {
                if (bMoved)
                    p_source.update_velocity(dt);
            }
        }
        break;
    case stSimulating:
        if (iPaused)
        {
            fTimeStarted += dt;
            fTimeToStop += dt;
            fTimeToPropagade += dt;
            break;
        }
        if (fTime >= fTimeToStop)
        {
            // STOP
            m_current_state = stStopped;
        }
        else
        {
            const u32 ptr = calc_cursor(fTimeStarted, fTime, get_length_sec(), p_source.freq, source()->m_wformat); //--#SM+#--
            set_cursor(ptr);

            if (update_culling(dt))
            {
                // switch to: PLAY
                m_current_state = stPlaying;
                SoundRender->i_start(this);
                dispatch_prefill();
            }
        }
        break;
    case stPlayingLooped:
        if (iPaused)
        {
            stop_target();
            m_current_state = stSimulatingLooped;

            fTimeStarted += dt;
            fTimeToPropagade += dt;
            break;
        }
        if (!update_culling(dt))
        {
            // switch to: SIMULATE
            stop_target();
            m_current_state = stSimulatingLooped; // switch state
        }
        else
        {
            if (bMoved)
                p_source.update_velocity(dt);
        }
        break;
    case stSimulatingLooped:
        if (iPaused)
        {
            fTimeStarted += dt;
            fTimeToPropagade += dt;
            break;
        }
        if (update_culling(dt))
        {
            // switch to: PLAY
            m_current_state = stPlayingLooped; // switch state
            const u32 ptr = calc_cursor(fTimeStarted, fTime, get_length_sec(), p_source.freq, source()->m_wformat); //--#SM+#--
            set_cursor(ptr);

            SoundRender->i_start(this);
            dispatch_prefill();
        }
        break;
    }

    //--#SM+# Begin--
    // hard rewind
    switch (m_current_state)
    {
    case stStarting:
    case stStartingLooped:
    case stPlaying:
    case stSimulating:
    case stPlayingLooped:
    case stSimulatingLooped:
        if (fTimeToRewind > 0.0f)
        {
            const float fLength = get_length_sec();
            const bool bLooped = fsimilar(fTimeToStop, TIME_TO_STOP_INFINITE);

            R_ASSERT2(fLength >= fTimeToRewind, "set_time: target time is bigger than length of sound");

            const float fRemainingTime = (fLength - fTimeToRewind) / p_source.freq;
            const float fPastTime = fTimeToRewind / p_source.freq;

            fTimeStarted = fTime - fPastTime;
            fTimeToPropagade = fTimeStarted; //--> For AI events

            if (fTimeStarted < 0.0f)
            {
                R_ASSERT2(fTimeStarted >= 0.0f, "Possible error in sound rewind logic! See log.");

                fTimeStarted = fTime;
                fTimeToPropagade = fTimeStarted;
            }

            if (!bLooped)
            {
                //--> Пересчитываем время, когда звук должен остановиться [recalculate stop time]
                fTimeToStop = fTime + fRemainingTime;
            }

            const u32 ptr = calc_cursor(fTimeStarted, fTime, fLength, p_source.freq, source()->m_wformat);
            set_cursor(ptr);

            fTimeToRewind = 0.0f;
        }

        break;
    default: break;
    }
    //--#SM+# End--

    // if deffered stop active and volume==0 -> physically stop sound
    if (bStopping && fis_zero(fade_volume))
        i_stop();

    VERIFY2(!!(owner_data) || (!(owner_data) && (m_current_state == stStopped)), "owner");
    VERIFY2(owner_data ? *(int*)(owner_data->feedback) : 1, "owner");

    bMoved = FALSE;

    // footer
    if (m_current_state != stStopped)
    {
        if (fTime >= fTimeToPropagade)
            Event_Propagade();
    }
    else if (owner_data)
    {
        VERIFY(this == owner_data->feedback);
        owner_data->feedback = nullptr;
        owner_data = nullptr;
    }
}

void CSoundRender_Emitter::render()
{
    target->fill_parameters(SoundRender);

    if (target->get_Rendering())
        target->update();
    else
        target->render();

    dispatch_prefill();
}

static IC void volume_lerp(float& c, float t, float s, float dt)
{
    const float diff = t - c;
    const float diff_a = _abs(diff);
    if (diff_a < EPS_S)
        return;
    float mot = s * dt;
    if (mot > diff_a)
        mot = diff_a;
    c += (diff / diff_a) * mot;
}

BOOL CSoundRender_Emitter::update_culling(float dt)
{
    float fade_speed = psSoundFadeSpeed;

    if (bStopping)
        fade_speed *= fStoppingSpeed_k;

    if (b2D)
    {
        occluder_volume = 1.f;

        fade_volume += dt * fade_speed * (bStopping ? -1.f : 1.f);
    }
    else
    {
        const float curr_vol = att() * applyOccVolume();
        const float fade_dir = bStopping || (curr_vol) < psSoundCull ? -1.f : 1.f;

        // Calc attenuated volume
        fade_volume += dt * fade_speed * fade_dir;

        // Если звук уже звучит и мы от него отошли дальше его максимальной
        // дистанции, не будем обрывать его резко. Пусть громкость плавно
        // снизится до минимального и потом уже выключается. А вот если звук еще
        // даже и не начинал звучать, то можно сразу отсекать его по расстоянию.
        // Ничего внезапно не замолчит, т.к. еще ничего и не было слышно. Звучит
        // звук или не звучит - будем определять по наличию target, т.е. тому,
        // что обеспечивает физическое звучание.
        if (!target && fade_dir < 0.f)
        {
            smooth_volume = 0;
            return FALSE;
        }
    }

    clamp(fade_volume, 0.f, 1.f);

    // Update smoothing
    smooth_volume = .9f * smooth_volume + .1f * applyOccVolume() * fade_volume;
    smooth_hf_volume = .9f * smooth_hf_volume + .1f * applyOccHfVolume() * fade_volume;

    if (smooth_volume < psSoundCull)
        return FALSE; // allow volume to go up

    // Here we has enought "PRIORITY" to be soundable
    // If we are playing already, return OK
    // --- else check availability of resources
    if (target)
        return TRUE;

    return SoundRender->i_allow_play(this);
}

float CSoundRender_Emitter::priority() const { return b2D ? 1000.f : smooth_volume * att() * priority_scale; }

/*
The AL_INVERSE_DISTANCE_CLAMPED model works according to the following formula:
distance = max(distance,AL_REFERENCE_DISTANCE);
distance = min(distance,AL_MAX_DISTANCE);
gain = AL_REFERENCE_DISTANCE / (AL_REFERENCE_DISTANCE +
 AL_ROLLOFF_FACTOR *
 (distance – AL_REFERENCE_DISTANCE));
*/
float CSoundRender_Emitter::att() const
{
    float dist = SoundRender->listener_position().distance_to(p_source.position);
    if (dist < p_source.min_distance)
        dist = p_source.min_distance;
    else if (dist > p_source.max_distance)
        return 0.f; // dist = p_source.max_distance;
    float att = p_source.min_distance / (p_source.min_distance + psSoundRolloff * (dist - p_source.min_distance));
    clamp(att, 0.f, 1.f);

    return att;
}

float CSoundRender_Emitter::applyOccVolume() const
{
    float vol = p_source.base_volume * p_source.volume * (owner_data->s_type == st_Effect ? psSoundVEffects * psSoundVFactor : psSoundVMusic);

    if (!b2D)
    {
        if (psSoundOcclusionScale < 1.f && occluder.valid)
            vol *= psSoundOcclusionScale;
        if (psSoundOcclusionMtl > 0.f && occluder_volume < 1.f)
            vol = vol * (1.f - psSoundOcclusionMtl) + vol * psSoundOcclusionMtl * occluder_volume;
    }

    return vol;
}

float CSoundRender_Emitter::applyOccHfVolume() const
{
    if (psSoundOcclusionHf > 0.f && occluder_volume < 1.f)
        return 1.f - psSoundOcclusionHf + occluder_volume * psSoundOcclusionHf;
    return 1.f;
}

void CSoundRender_Emitter::initStartingVolumes()
{
    fade_volume = 1.f;
    occluder_volume = (b2D || att() < psSoundCull) ? 1.f : SoundRender->get_occlusion(p_source.position, &occluder);
    smooth_volume = applyOccVolume();
    smooth_hf_volume = applyOccHfVolume();
}

#include "../COMMON_AI/ai_sounds.h"

void CSoundRender_Emitter::updateOccVolume(float dt)
{
    if (updateOccVolumeFrame == marker)
        return;

    updateOccVolumeFrame = marker;

    if (target)
    {
        // Звук воспроизводится или пока ещё воспроизводится
        const float occ = owner_data->g_type == SOUND_TYPE_WORLD_AMBIENT ? 1.0f : SoundRender->get_occlusion(p_source.position, &occluder);
        volume_lerp(occluder_volume, occ, 1.f, dt);
    }
    else
    {
        // Или звук только что запустили, или его (пока ещё?) не слышно. Нужно
        // проверить, если его не слышно потому, что он слишком далеко, тогда не
        // будем зря дёргать `get_occlusion`. `occluder_volume` нужно установить
        // сразу, а не через `volume_lerp`, что бы, если звук зазвучит на
        // следующем кадре, он сразу с правильной громкостью начал
        // воспроизводится. По этой же причине сразу установим `SmoothHfVolume`,
        // что бы высокие частоты правильную громкость имели.
        occluder_volume = (att() < psSoundCull) ? 1.f : SoundRender->get_occlusion(p_source.position, &occluder);
        smooth_hf_volume = applyOccHfVolume();
    }
    clamp(occluder_volume, 0.f, 1.f);
}
