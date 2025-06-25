#pragma once

#include <al.h>
#include <efx-presets.h>

#include "SoundRender.h"
#include "SoundRender_Environment.h"

#include <efx.h>

namespace efx_api
{
inline LPALGENFILTERS alGenFilters{};
inline LPALDELETEFILTERS alDeleteFilters{};
inline LPALISFILTER alIsFilter{};

inline LPALFILTERF alFilterf{};
inline LPALFILTERI alFilteri{};
} // namespace efx_api

class CSoundRender_Core : public CSound_manager_interface
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_Core, CSound_manager_interface);

protected:
    virtual void _create_data(ref_sound_data& S, LPCSTR fName, esound_type sound_type, u32 game_type);
    virtual void _destroy_data(ref_sound_data& S);

public:
    struct SListener
    {
        Fvector position;
        Fvector orientation[3];

        [[nodiscard]]
        SListener ToRHS() const
        {
            return {
                {position.x, position.y, -position.z},
                {
                    {orientation[0].x, orientation[0].y, -orientation[0].z},
                    {orientation[1].x, orientation[1].y, -orientation[1].z},
                    {orientation[2].x, orientation[2].y, -orientation[2].z},
                },
            };
        }
    };

protected:
    SListener Listener{};

    bool bListenerMoved{};
    bool e_currentPaused{false};

private:
    volatile bool bLocked{};

protected:
    CSoundRender_Environment e_current;
    CSoundRender_Environment* e_target{};

public:
    struct event
    {
        ref_sound_data_ptr sound_data;
        float range;
        float time_to_stop;
    };
    xr_vector<event> s_events;

public:
    BOOL bPresent{};
    BOOL bEAX{}; // Boolean variable to indicate presence of EAX Extension
    BOOL bDeferredEAX{};
    bool bEFX{}; // boolean variable to indicate presence of EFX Extension
    bool supports_float_pcm{};
    BOOL bReady{};

    CTimer Timer{};
    float fTimer_Value{};
    float fTimer_Delta{};
    sound_event* Handler{};

    struct Occ
    {
        Fvector occ[3]{};
        float occ_value{};
        u32 lastFrame{};
        bool valid{};
    };

protected:
    // Collider
    CDB::MODEL* geom_SOM{};
    CDB::MODEL* geom_OCC{};
    CDB::MODEL* geom_ENV{};

    // Containers
    string_unordered_map<xr_string, CSoundRender_Source*> s_sources;
    xr_vector<CSoundRender_Emitter*> s_emitters;
    xr_vector<CSoundRender_Target*> s_targets;

    SoundEnvironment_LIB* s_environment{};

    u32 s_emitters_u{}; // emitter update marker
    int m_iPauseCounter{};

    virtual void i_eax_set(const GUID* guid, u32 prop, void* val, u32 sz) = 0;
    virtual void i_eax_get(const GUID* guid, u32 prop, void* val, u32 sz) = 0;

    std::mutex m_bLocked;
    std::recursive_mutex s_sources_lock;

public:
    CSoundRender_Core();
    virtual ~CSoundRender_Core();

    // General
    virtual void _initialize(int stage) = 0;
    virtual void _clear() = 0;
    virtual void _restart();

    // Sound interface
    void verify_refsound(ref_sound& S);
    virtual void create(ref_sound& S, LPCSTR fName, esound_type sound_type, u32 game_type);
    virtual void attach_tail(ref_sound& S, LPCSTR fName);

    virtual void clone(ref_sound& S, const ref_sound& from, esound_type sound_type, u32 game_type);
    virtual void destroy(ref_sound& S);
    virtual void stop_emitters();
    virtual int pause_emitters(bool val);

    virtual void play(ref_sound& S, CObject* O, u32 flags = 0, float delay = 0.f);
    virtual void play_at_pos(ref_sound& S, CObject* O, const Fvector& pos, u32 flags = 0, float delay = 0.f);
    virtual void play_no_feedback(ref_sound& S, CObject* O, u32 flags = 0, float delay = 0.f, Fvector* pos = 0, float* vol = 0, float* freq = 0, Fvector2* range = 0);
    virtual void set_master_volume(float f) = 0;
    virtual void set_master_gain(float low_pass, float high_pass);
    virtual void set_geometry_env(IReader* I);
    virtual void set_geometry_som(IReader* I);
    virtual void set_geometry_occ(CDB::MODEL* M);
    virtual void set_handler(sound_event* E);

    void update(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R) override;
    void render() override;
    virtual void statistic(CSound_stats* dest, CSound_stats_ext* ext);

    // listener
    const auto& listener_params() const { return Listener; }
    const Fvector& listener_position() override { return Listener.position; }
    virtual void update_listener(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R, float dt) = 0;

    virtual float get_time() const { return Timer.GetElapsed_sec(); }

    // eax listener
    void i_eax_listener_set(CSound_environment* E);
    void i_eax_commit_setting();

    // efx listener
    void i_efx_listener_set(CSound_environment* E);
    bool i_efx_commit_setting();
    void i_efx_disable();

    virtual CSound_environment* DbgCurrentEnv() override { return e_target; }
    virtual void DbgCurrentEnvPaused(bool v) override { e_currentPaused = v; }
    virtual void DbgCurrentEnvSave() override { env_save_all(); }

public:
    CSoundRender_Source* i_create_source(LPCSTR name);
    void i_destroy_source(CSoundRender_Source* S);
    CSoundRender_Emitter* i_play(ref_sound* S, BOOL _loop, float delay);
    void i_start(CSoundRender_Emitter* E) const;
    bool i_allow_play(const CSoundRender_Emitter* E) const;

    virtual void object_relcase(CObject* obj);

    virtual float get_occlusion_to(const Fvector& hear_pt, const Fvector& snd_pt, float dispersion = 0.2f);
    float get_occlusion(const Fvector& snd_pt, Occ* occ);
    float calc_occlusion(const Fvector& hear_pt, const Fvector& snd_pt, Occ* occ);
    CSoundRender_Environment* get_environment_def();
    CSoundRender_Environment* get_environment(const Fvector& P);

    void env_load();
    void env_unload();
    void env_save_all() const;
    void env_apply();

    float master_low_pass{1.f};
    float master_high_pass{1.f};

protected: // EFX
    EFXEAXREVERBPROPERTIES efx_reverb{};
    ALuint effect{};
    ALuint slot{};

    bool EFXTestSupport();
    void InitAlEFXAPI();

    void release_efx_objects() const;

private:
    float occRayTestMtl(const Fvector& pos, const Fvector& dir, float range, Occ* occ);
    float occRayTestSom(const Fvector& pos, const Fvector& dir, float range) const;
};

extern CSoundRender_Core* SoundRender;
