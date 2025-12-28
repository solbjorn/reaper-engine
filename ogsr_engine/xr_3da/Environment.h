#ifndef EnvironmentH
#define EnvironmentH

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/EnvironmentRender.h"

// refs
class CInifile;
class CEnvironment;

// refs - effects
class CLensFlare;
class CEffect_Rain;
class CEffect_Thunderbolt;

class CPerlinNoise1D;

struct SThunderboltDesc;
struct SThunderboltCollection;
class CLensFlareDescriptor;

class xr_task_group;

#define DAY_LENGTH 86400.f

// t-defs
class CEnvModifier
{
public:
    Fvector3 position;
    float radius;
    float power;

    float far_plane;
    Fvector3 fog_color;
    float fog_density;
    Fvector3 ambient;
    Fvector3 sky_color;
    Fvector3 hemi_color;
    Flags16 use_flags;

    void load(IReader* fs, u32 version);
    float sum(CEnvModifier& _another, Fvector3& view);
};

class CEnvAmbient
{
public:
    struct SEffect
    {
        ref_sound sound;
        shared_str particles;
        Fvector offset;
        u32 life_time;

        float wind_gust_factor;
        float wind_blast_in_time;
        float wind_blast_out_time;
        float wind_blast_strength;
        Fvector wind_blast_direction;
    };
    DEFINE_VECTOR(SEffect*, EffectVec, EffectVecIt);

    struct SSndChannel
    {
        shared_str m_load_section;
        Fvector2 m_sound_dist;
        Ivector4 m_sound_period;

        typedef xr_vector<ref_sound> sounds_type;

        void load(CInifile& config, LPCSTR sect);
        ref_sound& get_rnd_sound() { return sounds()[Random.randI(sounds().size())]; }
        u32 get_rnd_sound_time() const { return (m_sound_period.z < m_sound_period.w) ? Random.randI(m_sound_period.z, m_sound_period.w) : 0; }
        u32 get_rnd_sound_first_time() const { return (m_sound_period.x < m_sound_period.y) ? Random.randI(m_sound_period.x, m_sound_period.y) : 0; }
        float get_rnd_sound_dist() const { return (m_sound_dist.x < m_sound_dist.y) ? Random.randF(m_sound_dist.x, m_sound_dist.y) : 0; }
        inline sounds_type& sounds() { return m_sounds; }

    protected:
        xr_vector<ref_sound> m_sounds;
    };
    DEFINE_VECTOR(SSndChannel*, SSndChannelVec, SSndChannelVecIt);

protected:
    shared_str m_load_section;

    EffectVec m_effects;
    Ivector2 m_effect_period{};

    SSndChannelVec m_sound_channels;
    shared_str m_ambients_config_filename;

    shared_str section;
    xr_vector<SEffect> effects;
    xr_vector<ref_sound> sounds;

    Fvector2 sound_dist{};
    Ivector2 sound_period{};
    Ivector2 effect_period{};

public:
    IC const shared_str& name() { return m_load_section.size() == 0 ? section : m_load_section; }
    IC const shared_str& get_ambients_config_filename() { return m_ambients_config_filename; }

    void load(CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config, const shared_str& section);
    IC SEffect* get_rnd_effect() { return m_effects.empty() ? nullptr : m_effects[Random.randI(m_effects.size())]; }
    IC u32 get_rnd_effect_time() { return Random.randI(m_effect_period.x, m_effect_period.y); }

    SEffect* create_effect(CInifile& config, LPCSTR id);
    SSndChannel* create_sound_channel(CInifile& config, LPCSTR id);
    ~CEnvAmbient();
    void destroy();
    inline SSndChannelVec& get_snd_channels() { return m_sound_channels; }
    void load_shoc(const shared_str& section);

    IC ref_sound* get_rnd_sound() { return sounds.empty() ? nullptr : &sounds[Random.randI(sounds.size())]; }
    IC u32 get_rnd_sound_time() { return Random.randI(sound_period.x, sound_period.y); }
    IC float get_rnd_sound_dist() { return Random.randF(sound_dist.x, sound_dist.y); }
    IC u32 get_rnd_effect_time_shoc() { return Random.randI(effect_period.x, effect_period.y); }
};

class CEnvDescriptor
{
public:
    float exec_time;
    float exec_time_loaded;

    shared_str sky_texture_name;
    shared_str sky_texture_env_name;
    shared_str clouds_texture_name;

    FactoryPtr<IEnvDescriptorRender> m_pDescriptor;

    Fvector4 clouds_color;
    float clouds_rotation;
    Fvector3 sky_color;
    float sky_rotation;

    float far_plane;

    Fvector3 fog_color;
    float fog_density;
    float fog_distance;

    float rain_density;
    Fvector3 rain_color;

    float bolt_period;
    float bolt_duration;

    float wind_velocity;
    float wind_direction;

    Fvector4 hemi_color; // w = R2 correction
    Fvector3 ambient;
    Fvector3 sun_color;
    Fvector3 sun_dir;
    float m_fSunShaftsIntensity;
    float m_fWaterIntensity;
    float m_fTreeAmplitudeIntensity;

    Fvector4 bloom;

    shared_str lens_flare_id;
    shared_str tb_id;
    float sky_height;

    void set_sun(LPCSTR sect, CEnvironment* parent);

    CEnvAmbient* env_ambient{};
    void setEnvAmbient(LPCSTR sect, CEnvironment* parent);

    explicit CEnvDescriptor(const shared_str& identifier = {});

    void load(CEnvironment& environment, CInifile& config);
    void load_shoc(CEnvironment& environment, LPCSTR exec_tm, LPCSTR S);
    void load_shoc(float exec_tm, LPCSTR S, CEnvironment& environment);
    void load_common(CInifile* config);

    void copy(const CEnvDescriptor& src)
    {
        float tm0 = exec_time;
        float tm1 = exec_time_loaded;
        *this = src;
        exec_time = tm0;
        exec_time_loaded = tm1;
    }

    void get();
    void put();

    shared_str m_identifier;
};

class CEnvDescriptorMixer : public CEnvDescriptor
{
public:
    float weight;
    float fog_near;
    float fog_far;

public:
    explicit CEnvDescriptorMixer(shared_str const& identifier);

    void lerp(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
};

class CEnvironment
{
    friend class dxEnvironmentRender;
    struct str_pred
    {
        IC bool operator()(const shared_str& x, const shared_str& y) const { return xr_strcmp(x, y) < 0; }
    };

public:
    DEFINE_VECTOR(CEnvAmbient*, EnvAmbVec, EnvAmbVecIt);
    DEFINE_VECTOR(CEnvDescriptor*, EnvVec, EnvIt);
    DEFINE_MAP_PRED(shared_str, EnvVec, EnvsMap, EnvsMapIt, str_pred);

private:
    // clouds
    FvectorVec CloudsVerts;
    U16Vec CloudsIndices;

    float NormalizeTime(float tm);
    float TimeDiff(float prev, float cur);
    float TimeWeight(float val, float min_t, float max_t);
    void SelectEnvs(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float tm);
    void SelectEnv(EnvVec* envs, CEnvDescriptor*& e, float tm);

    void calculate_dynamic_sun_dir();
    void invalidate_descs();

public:
    Fvector3 calculate_config_sun_dir(float ftime);

    static bool sort_env_pred(const CEnvDescriptor* x, const CEnvDescriptor* y) { return x->exec_time < y->exec_time; }
    static bool sort_env_etl_pred(const CEnvDescriptor* x, const CEnvDescriptor* y) { return x->exec_time_loaded < y->exec_time_loaded; }

protected:
    CPerlinNoise1D* PerlinNoise1D;

    float fGameTime{};

public:
    FactoryPtr<IEnvironmentRender> m_pRender;

    float wind_strength_factor{};
    float wind_gust_factor{};
    float wetness_factor{};
    Fvector4 wind_anim{};

    // wind blast params
    float wind_blast_strength{};
    Fvector wind_blast_direction;
    Fquaternion wind_blast_start_time;
    Fquaternion wind_blast_stop_time;
    float wind_blast_strength_start_value{};
    float wind_blast_strength_stop_value{};
    Fquaternion wind_blast_current;

    // Environments
    CEnvDescriptorMixer* CurrentEnv{};
    CEnvDescriptor* Current[2]{};

    bool bWFX{};
    float wfx_time;
    CEnvDescriptor* WFX_end_desc[2];

    EnvVec* CurrentWeather{};
    shared_str CurrentWeatherName;
    shared_str PrevWeatherName;
    shared_str CurrentCycleName;
    u32 m_last_weather_shift{};

    EnvsMap WeatherCycles;
    EnvsMap WeatherFXs;
    xr_vector<CEnvModifier> Modifiers;
    EnvAmbVec Ambients;

    CEffect_Rain* eff_Rain{};
    CLensFlare* eff_LensFlare{};
    CEffect_Thunderbolt* eff_Thunderbolt{};

    bool USED_COP_WEATHER{};

    float fTimeFactor;

    void SelectEnvs(float gt);

    void UpdateAmbient();
    CEnvAmbient* AppendEnvAmb(const shared_str& sect);

    void Invalidate();

public:
    CEnvironment();
    ~CEnvironment();

    void load();
    void unload();

    void mods_load();
    void mods_unload();

    void OnFrame();
    void lerp(float& current_weight);

    void RenderSky();
    void RenderClouds();
    void RenderFlares();
    void RenderLast();

    bool SetWeatherFX(shared_str name);
    bool StartWeatherFXFromTime(shared_str name, float time);
    bool IsWFXPlaying() { return bWFX; }
    void StopWFX();

    void SetWeather(shared_str name, bool forced = false);
    shared_str GetWeather() { return CurrentWeatherName; }
    shared_str GetPrevWeather() { return PrevWeatherName; }
    void SetWeatherNext(shared_str name);
    void ChangeGameTime(float game_time);
    void SetGameTime(float game_time, float time_factor);
    u32 GetWeatherLastShift() { return m_last_weather_shift; }

    void OnDeviceCreate();
    void OnDeviceDestroy();

    bool m_dynamic_sun_movement{};
    bool m_sun_hp_loaded{};

    Fvector2 sun_hp[24];

    CInifile* m_ambients_config{};
    CInifile* m_sound_channels_config{};
    CInifile* m_effects_config{};
    CInifile* m_suns_config{};
    CInifile* m_sun_pos_config{};
    CInifile* m_thunderbolt_collections_config{};
    CInifile* m_thunderbolts_config{};

private:
    xr_task_group* tg{};

protected:
    CEnvDescriptor* create_descriptor(shared_str const& identifier, CInifile* config);
    CEnvDescriptor* create_descriptor_shoc(LPCSTR exec_tm, LPCSTR S);
    void load_sun();
    void load_weathers();
    void load_weather_effects();
    void create_mixer();
    void destroy_mixer();
    void load_level_specific_ambients();

public:
    SThunderboltDesc* thunderbolt_description(CInifile& config, shared_str const& section);
    SThunderboltCollection* thunderbolt_collection(CInifile* pIni, CInifile* thunderbolts, LPCSTR section);
    SThunderboltDesc* thunderbolt_description_shoc(CInifile* config, shared_str const& section);
    SThunderboltCollection* thunderbolt_collection_shoc(CInifile* pIni, LPCSTR section);
    SThunderboltCollection* thunderbolt_collection(xr_vector<SThunderboltCollection*>& collection, shared_str const& id);
    CLensFlareDescriptor* add_flare(xr_vector<CLensFlareDescriptor*>& collection, shared_str const& id);

public:
    float p_var_alt;
    float p_var_long;
    float p_min_dist;
    float p_tilt;
    float p_second_prop;
    float p_sky_color;
    float p_sun_color;
    float p_fog_color;
};

extern Flags32 psEnvFlags;
extern float psVisDistance;

// embedded_editor_weather.cpp
extern bool s_ScriptNoMixer;
extern bool s_ScriptTime;
extern bool s_ScriptWeather;

// xr_ioc_cmd.cpp
extern float ps_r2_sun_shafts_min;
extern float ps_r2_sun_shafts_value;
extern Fvector3 ssfx_wetness_multiplier;

#endif // EnvironmentH
