#ifndef SoundRender_EmitterH
#define SoundRender_EmitterH

#include "soundrender.h"
#include "SoundRender_Core.h"
#include "soundrender_environment.h"

namespace sf
{
typedef struct sf_private_tag SNDFILE;
}

class CSoundRender_Emitter : public CSound_emitter
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_Emitter, CSound_emitter);

public:
    enum State : u32
    {
        stStopped = 0,

        stStartingDelayed,
        stStartingLoopedDelayed,

        stStarting,
        stStartingLooped,

        stPlaying,
        stPlayingLooped,

        stSimulating,
        stSimulatingLooped,
    };

private:
    CSoundRender_Core::Occ occluder;
    u32 updateOccVolumeFrame{};

public:
#ifdef DEBUG
    u32 dbg_ID;
#endif

    CSoundRender_Target* target{};
    ref_sound_data_ptr owner_data{};

    [[nodiscard]] CSoundRender_Source* source() const { return (CSoundRender_Source*)owner_data->handle; }
    [[nodiscard]] u32 get_bytes_total() const;
    [[nodiscard]] float get_length_sec() const;

    float starting_delay{};
    float priority_scale;
    float smooth_volume;
    float occluder_volume; // USER
    float fade_volume;

    State m_current_state;
    u32 m_stream_cursor;
    u32 m_cur_handle_cursor;
    CSound_params p_source{};

    int iPaused;
    BOOL bMoved;
    BOOL b2D;
    BOOL bRewind;
    BOOL bStopping;
    float fStoppingSpeed_k{1.f};
    float fTimeStarted; // time of "Start"
    float fTimeToStop; // time to "Stop"
    float fTimeToPropagade;
    float fTimeToRewind; // --#SM+#--

    u32 marker;
    tmc::task<void> i_stop();

    [[nodiscard]] u32 get_cursor(bool b_absolute) const;
    void set_cursor(u32 p);
    void move_cursor(int offset);

private:
    sf::SNDFILE* snd{};

    tmc::manual_reset_event event{true};
    xr_vector<u8> temp_buf[sdef_target_count_prefill];

    gsl::index current_block{};
    gsl::index filled_blocks{};

    void fill_block(void* ptr, u32 size);
    void fill_data(void* dest, u32 offset, u32 size);
    void fill_all_blocks();

    tmc::task<void> dispatch_prefill();
    tmc::task<void> prefill_async(f32 fDeltaTime);
    tmc::task<void> wait_prefill() { co_await event; }

    bool canceling{};

public:
    void updateOccVolume(float dt);

    float smooth_hf_volume;

    void Event_Propagade();
    void Event_ReleaseOwner();

    bool isPlaying() const { return m_current_state != stStopped; }
    [[nodiscard]] bool is_2D() const override { return b2D; }
    void switch_to_2D() override;
    void switch_to_3D() override;

    void set_position(const Fvector3& pos) override;

    void set_frequency(f32 scale) override
    {
        VERIFY(_valid(scale));
        p_source.freq = scale;
    }

    void set_range(f32 min, f32 max) override
    {
        VERIFY(_valid(min) && _valid(max));

        p_source.min_distance = min;
        p_source.max_distance = max;
    }

    void set_volume(f32 vol) override
    {
        if (!_valid(vol))
            vol = 0.0f;

        p_source.volume = vol;
    }

    void set_gain(f32 low_gain, f32 high_gain) override
    {
        p_source.low_gain = low_gain;
        p_source.high_gain = high_gain;
    }

    void set_priority(f32 p) override { priority_scale = p; }
    void set_time(f32 t) override; //--#SM+#--
    [[nodiscard]] const CSound_params* get_params() override { return &p_source; }

    tmc::task<std::span<u8>> obtain_block();

    float priority() const;
    float att() const;
    void start(ref_sound* _owner, BOOL _loop, float delay);
    tmc::task<void> cancel(); // manager forces out of rendering
    tmc::task<void> update(f32 time, f32 dt);
    tmc::task<void> render();
    BOOL update_culling(float dt);
    void rewind();
    tmc::task<void> stop(bool bDeffered, f32 speed_k = 1.0f) override;
    void pause(BOOL bVal, int id);

    [[nodiscard]] u32 play_time() override;

    CSoundRender_Emitter();
    ~CSoundRender_Emitter() override;

private:
    tmc::task<void> stop_target();

    float applyOccVolume() const;
    float applyOccHfVolume() const;
    void initStartingVolumes();
};

#endif
