#include "stdafx.h"

#include "../xrCore/xr_task.h"
#include "soundrender_emitter.h"
#include "soundrender_core.h"
#include "SoundRender_Source.h"

extern u32 psSoundModel;
extern float psSoundVEffects;

void CSoundRender_Emitter::set_position(const Fvector& pos)
{
    if (source()->channels_num() == 1 && _valid(pos))
        p_source.update_position(pos);
    else
        p_source.update_position({});

    bMoved = TRUE;
}

// Перемотка звука на заданную секунду [rewind snd to target time] --#SM+#--
void CSoundRender_Emitter::set_time(float t)
{
    R_ASSERT2(get_length_sec() >= t, "set_time: time is bigger than length of sound");

    clamp(t, 0.0f, get_length_sec());
    fTimeToRewind = t;
}

CSoundRender_Emitter::CSoundRender_Emitter()
{
#ifdef DEBUG
    static u32 incrementalID = 0;
    dbg_ID = ++incrementalID;
#endif

    tg = &xr_task_group_get();
    target = NULL;
    owner_data = NULL;
    smooth_volume = 1.f;
    occluder_volume = 1.f;
    fade_volume = 1.f;
    occluder[0].set(0, 0, 0);
    occluder[1].set(0, 0, 0);
    occluder[2].set(0, 0, 0);
    m_current_state = stStopped;
    set_cursor(0);
    bMoved = TRUE;
    b2D = FALSE;
    bStopping = FALSE;
    bRewind = FALSE;
    iPaused = 0;
    fTimeStarted = 0.0f;
    fTimeToStop = 0.0f;
    fTimeToPropagade = 0.0f;
    fTimeToRewind = 0.0f; //--#SM+#--
    marker = 0xabababab;
    starting_delay = 0.f;
    priority_scale = 1.f;
    m_cur_handle_cursor = 0;
}

CSoundRender_Emitter::~CSoundRender_Emitter()
{
    // try to release dependencies, events, for example
    Event_ReleaseOwner();
    tg->wait_put();
}

//////////////////////////////////////////////////////////////////////
void CSoundRender_Emitter::Event_ReleaseOwner()
{
    if (!owner_data)
        return;

    auto& events = SoundRender->s_events;

    for (u32 it = 0; it < events.size(); it++)
    {
        if (owner_data == events[it].first)
        {
            events.erase(events.begin() + it);
            it--;
        }
    }
}

void CSoundRender_Emitter::Event_Propagade()
{
    fTimeToPropagade += ::Random.randF(s_f_def_event_pulse - 0.030f, s_f_def_event_pulse + 0.030f);
    if (!owner_data)
        return;
    if (!owner_data->g_type)
        return;
    if (!owner_data->g_object)
        return;
    if (!SoundRender->Handler)
        return;

    VERIFY(_valid(p_source.volume));
    // Calculate range
    const float clip = p_source.max_ai_distance * p_source.volume;
    const float range = std::min(p_source.max_ai_distance, clip);
    if (range < 0.1f)
        return;

    // Inform objects
    SoundRender->s_events.emplace_back(owner_data, range);
}

void CSoundRender_Emitter::switch_to_2D()
{
    b2D = true;
    set_priority(100.f);
}

void CSoundRender_Emitter::switch_to_3D() { b2D = false; }

u32 CSoundRender_Emitter::play_time()
{
    if (m_current_state == stPlaying || m_current_state == stPlayingLooped || m_current_state == stSimulating || m_current_state == stSimulatingLooped)
        return iFloor((SoundRender->fTimer_Value - fTimeStarted) * 1000.0f);

    return 0;
}

void CSoundRender_Emitter::set_cursor(u32 p)
{
    m_stream_cursor = p;

    if (owner_data._get() && owner_data->fn_attached[0].size())
    {
        u32 bt = ((CSoundRender_Source*)owner_data->handle)->dwBytesTotal;
        if (m_stream_cursor >= m_cur_handle_cursor + bt)
        {
            SoundRender->i_destroy_source((CSoundRender_Source*)owner_data->handle);
            owner_data->handle = SoundRender->i_create_source(owner_data->fn_attached[0].c_str());
            owner_data->fn_attached[0] = owner_data->fn_attached[1];
            owner_data->fn_attached[1] = "";
            m_cur_handle_cursor = get_cursor(true);
        }
    }
}

u32 CSoundRender_Emitter::get_cursor(bool b_absolute) const
{
    if (b_absolute)
        return m_stream_cursor;

    VERIFY(m_stream_cursor - m_cur_handle_cursor >= 0);
    return m_stream_cursor - m_cur_handle_cursor;
}

void CSoundRender_Emitter::move_cursor(int offset) { set_cursor(get_cursor(true) + offset); }
void CSoundRender_Emitter::fill_data(void* dest, u32 offset, u32 size) const { source()->decompress(dest, offset, size, ovf); }

void CSoundRender_Emitter::fill_block(void* ptr, u32 size)
{
    u8* dest = (u8*)(ptr);
    const u32 dwBytesTotal = get_bytes_total();

    if ((get_cursor(true) + size) > dwBytesTotal)
    {
        // We are reaching the end of data, what to do?
        switch (m_current_state)
        {
        case stPlaying: {
            // Fill as much data as we can, zeroing remainder
            if (get_cursor(true) >= dwBytesTotal)
            {
                // ??? We requested the block after remainder - just zero
                memset(dest, 0, size);
            }
            else
            {
                // Calculate remainder
                const u32 sz_data = dwBytesTotal - get_cursor(true);
                const u32 sz_zero = (get_cursor(true) + size) - dwBytesTotal;
                VERIFY(size == (sz_data + sz_zero));
                fill_data(dest, get_cursor(false), sz_data);
                memset(dest + sz_data, 0, sz_zero);
            }
            move_cursor(size);
        }
        break;
        case stPlayingLooped: {
            u32 hw_position = 0;
            do
            {
                u32 sz_data = dwBytesTotal - get_cursor(true);
                const u32 sz_write = std::min(size - hw_position, sz_data);
                fill_data(dest + hw_position, get_cursor(true), sz_write);
                hw_position += sz_write;
                move_cursor(sz_write);
                set_cursor(get_cursor(true) % dwBytesTotal);
            } while (0 != (size - hw_position));
        }
        break;
        default: FATAL("SOUND: Invalid emitter state"); break;
        }
    }
    else
    {
        const u32 bt_handle = ((CSoundRender_Source*)owner_data->handle)->dwBytesTotal;
        if (get_cursor(true) + size > m_cur_handle_cursor + bt_handle)
        {
            R_ASSERT(owner_data->fn_attached[0].size());

            u32 rem = 0;
            if ((m_cur_handle_cursor + bt_handle) > get_cursor(true))
            {
                rem = (m_cur_handle_cursor + bt_handle) - get_cursor(true);

#ifdef DEBUG
                Msg("reminder from prev source %d", rem);
#endif // #ifdef DEBUG
                fill_data(dest, get_cursor(false), rem);
                move_cursor(rem);
            }
#ifdef DEBUG
            Msg("recurce from next source %d", size - rem);
#endif // #ifdef DEBUG
            fill_block(dest + rem, size - rem);
        }
        else
        {
            // Everything OK, just stream
            fill_data(dest, get_cursor(false), size);
            move_cursor(size);
        }
    }
}

std::pair<u8*, size_t> CSoundRender_Emitter::obtain_block()
{
    wait_prefill();

    const std::pair result = {temp_buf[current_block].data(), temp_buf[current_block].size()};
    if (++current_block >= sdef_target_count_prefill)
        current_block = 0;

    filled_blocks--;
    return std::move(result);
}

void CSoundRender_Emitter::fill_all_blocks()
{
    current_block = 0;

    for (size_t i = 0; i < sdef_target_count_prefill; i++)
        fill_block(temp_buf[i].data(), temp_buf[i].size());

    filled_blocks = sdef_target_count_prefill;
}

void CSoundRender_Emitter::dispatch_prefill()
{
    wait_prefill();

    if (filled_blocks >= sdef_target_count_prefill)
        return;

    tg->run([this] {
        size_t next_block_to_fill = (current_block + filled_blocks) % sdef_target_count_prefill;

        while (filled_blocks < sdef_target_count_prefill)
        {
            auto& block = temp_buf[next_block_to_fill];
            fill_block(block.data(), block.size());

            next_block_to_fill = (next_block_to_fill + 1) % sdef_target_count_prefill;
            filled_blocks++;
        }
    });
}

void CSoundRender_Emitter::wait_prefill() const { tg->wait(); }

u32 CSoundRender_Emitter::get_bytes_total() const { return owner_data->dwBytesTotal; }
float CSoundRender_Emitter::get_length_sec() const { return owner_data->fTimeTotal; }
