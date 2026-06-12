#pragma once

#include "SoundRender.h"
#include "SoundRender_Environment.h"

#include <al.h>
#include <efx.h>
#include <efx-presets.h>

namespace efx_api
{
inline LPALGENFILTERS alGenFilters{};
inline LPALDELETEFILTERS alDeleteFilters{};
inline LPALISFILTER alIsFilter{};

inline LPALFILTERF alFilterf{};
inline LPALFILTERI alFilteri{};
} // namespace efx_api

class XR_NOVTABLE CSoundRender_Core : public CSound_manager_interface
{
    RTTI_DECLARE_TYPEINFO(CSoundRender_Core, CSound_manager_interface);

protected:
    void _create_data(ref_sound_data& S, gsl::czstring fName, esound_type sound_type, u32 game_type) override;

public:
    struct SListener
    {
        Fvector position;
        Fvector orientation[3];

        [[nodiscard]]
        SListener ToRHS() const
        {
            return {
                Fvector{position.x, position.y, -position.z},
                {
                    Fvector{orientation[0].x, orientation[0].y, -orientation[0].z},
                    Fvector{orientation[1].x, orientation[1].y, -orientation[1].z},
                    Fvector{orientation[2].x, orientation[2].y, -orientation[2].z},
                },
            };
        }
    };

protected:
    SListener Listener{};

    bool bListenerMoved{};
    bool e_currentPaused{};

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
    ~CSoundRender_Core() override;

    // General
    void _initialize(s32 stage) override = 0;
    void _clear() override = 0;
    void _restart() override;

    // Sound interface
    void verify_refsound(ref_sound& S);
    void create(ref_sound& S, gsl::czstring fName, esound_type sound_type, u32 game_type) override;
    void attach_tail(ref_sound& S, gsl::czstring fName) override;

    void clone(ref_sound& S, const ref_sound& from, esound_type sound_type, u32 game_type) override;
    tmc::task<void> destroy(std::array<std::byte, 16>& arg) override;
    void queue_destroy(ref_sound& S) override;
    tmc::task<void> stop_emitters() override;
    [[nodiscard]] s32 pause_emitters(bool val) override;

    void play(ref_sound& S, CObject* O, u32 flags = 0, f32 delay = 0.0f) override;
    void play_at_pos(ref_sound& S, CObject* O, const Fvector3& pos, u32 flags = 0, f32 delay = 0.0f) override;
    void play_no_feedback(ref_sound& S, CObject* O, u32 flags = 0, f32 delay = 0.0f, Fvector3* pos = nullptr, f32* vol = nullptr, f32* freq = nullptr,
                          Fvector2* range = nullptr) override;
    void queue_stop(ref_sound& S, bool deferred, f32 speed_k = 1.0f) override;

    void set_master_volume(f32 f) override = 0;
    void set_master_gain(f32 low_pass, f32 high_pass) override;

    void set_geometry_env(IReader* I) override;
    void set_geometry_som(IReader* I) override;
    void set_geometry_occ(CDB::MODEL* M) override;
    void set_handler(sound_event* E) override;

    tmc::task<void> update(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R) override;
    tmc::task<void> render() override;
    void statistic(CSound_stats* dest, CSound_stats_ext* ext) override;

    // listener
    const auto& listener_params() const { return Listener; }
    const Fvector& listener_position() override { return Listener.position; }
    virtual void update_listener(const Fvector3& P, const Fvector3& D, const Fvector3& N, const Fvector3& R, f32 dt) = 0;

    [[nodiscard]] f32 get_time() const override { return Timer.GetElapsed_sec(); }

    // eax listener
    void i_eax_listener_set(CSound_environment* E);
    void i_eax_commit_setting();

    // efx listener
    void i_efx_listener_set(CSound_environment* E);
    bool i_efx_commit_setting();
    void i_efx_disable();

    [[nodiscard]] CSound_environment* DbgCurrentEnv() override { return e_target; }
    void DbgCurrentEnvPaused(bool v) override { e_currentPaused = v; }
    void DbgCurrentEnvSave() override { env_save_all(); }

public:
    CSoundRender_Source* i_create_source(LPCSTR name);
    CSoundRender_Emitter* i_play(ref_sound* S, BOOL _loop, float delay);
    tmc::task<void> i_start(CSoundRender_Emitter* E) const;
    bool i_allow_play(const CSoundRender_Emitter* E) const;
    bool i_locked() const override { return bLocked; }

    void object_relcase(CObject* obj) override;

    [[nodiscard]] f32 get_occlusion_to(const Fvector3& hear_pt, const Fvector3& snd_pt, f32 dispersion = 0.2f) override;
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
    tmc::task<void> stop(std::array<std::byte, 16>& arg);

    float occRayTestMtl(const Fvector& pos, const Fvector& dir, float range, Occ* occ);
    float occRayTestSom(const Fvector& pos, const Fvector& dir, float range) const;
};

extern CSoundRender_Core* SoundRender;
