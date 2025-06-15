#ifndef SoundRender_EmitterH
#define SoundRender_EmitterH

#include "soundrender.h"
#include "SoundRender_Core.h"
#include "soundrender_environment.h"

class xr_task_group;
struct OggVorbis_File;

class CSoundRender_Emitter : public CSound_emitter
{
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
    ref_sound_data_ptr owner_data;

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
    BOOL bStopping;
    float fStoppingSpeed_k{1.f};
    BOOL bRewind;
    float fTimeStarted; // time of "Start"
    float fTimeToStop; // time to "Stop"
    float fTimeToPropagade;
    float fTimeToRewind; // --#SM+#--

    u32 marker;
    void i_stop();

    [[nodiscard]] u32 get_cursor(bool b_absolute) const;
    void set_cursor(u32 p);
    void move_cursor(int offset);

private:
    OggVorbis_File* ovf{};

    xr_task_group* tg{};
    xr_vector<u8> temp_buf[sdef_target_count_prefill];

    size_t current_block{};
    int filled_blocks{};

    void fill_block(void* ptr, u32 size);
    void fill_data(void* dest, u32 offset, u32 size);

    void fill_all_blocks();
    void dispatch_prefill();

    void wait_prefill() const;

    bool canceling{};

public:
    void updateOccVolume(float dt);

    float smooth_hf_volume;

    void Event_Propagade();
    void Event_ReleaseOwner();

    bool isPlaying() const { return m_current_state != stStopped; }
    virtual bool is_2D() const { return b2D; }
    virtual void switch_to_2D();
    virtual void switch_to_3D();

    virtual void set_position(const Fvector& pos);

    virtual void set_frequency(float scale)
    {
        VERIFY(_valid(scale));
        p_source.freq = scale;
    }

    virtual void set_range(float min, float max)
    {
        VERIFY(_valid(min) && _valid(max));
        p_source.min_distance = min;
        p_source.max_distance = max;
    }

    virtual void set_volume(float vol)
    {
        if (!_valid(vol))
            vol = 0.0f;
        p_source.volume = vol;
    }

    virtual void set_gain(float low_gain, float high_gain)
    {
        p_source.low_gain = low_gain;
        p_source.high_gain = high_gain;
    }

    virtual void set_priority(float p) { priority_scale = p; }
    virtual void set_time(float t); //--#SM+#--
    virtual const CSound_params* get_params() { return &p_source; }

    std::pair<u8*, size_t> obtain_block();

    float priority() const;
    float att() const;
    void start(ref_sound* _owner, BOOL _loop, float delay);
    void cancel(); // manager forces out of rendering
    void update(float time, float dt);
    void render();
    BOOL update_culling(float dt);
    void rewind();
    void stop(BOOL bDeffered, float speed_k = 1.f);
    void pause(BOOL bVal, int id);

    virtual u32 play_time();

    CSoundRender_Emitter();
    ~CSoundRender_Emitter();

private:
    void stop_target();

    float applyOccVolume() const;
    float applyOccHfVolume() const;
    void initStartingVolumes();
};

#endif
