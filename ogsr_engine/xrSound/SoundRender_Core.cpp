#include "stdafx.h"

#include "../xr_3da/xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"

#pragma warning(push)
#pragma warning(disable : 4995)
#include <eax.h>
#pragma warning(pop)

BOOL bSenvironmentXrExport{};
int psSoundTargets = 256; // 512; //--#SM+#-- //32;
Flags32 psSoundFlags = {/*ss_Hardware*/};

float psSoundOcclusionScale = 0.8f;
float psSoundOcclusionHf = 0.5f;
float psSoundOcclusionMtl = 0.4f;

float psSoundCull = 0.01f;
float psSoundRolloff = 0.8f;
float psSoundFadeSpeed = 4.f;

u32 psSoundModel = 0;
float psSoundVEffects = 1.0f;
float psSoundVFactor = 1.0f;
float psSoundTimeFactor = 1.0f; //--#SM+#--
float psSoundVMusic = 1.f;
float psSoundVMusicFactor = 1.f;

CSoundRender_Core* SoundRender = 0;
CSound_manager_interface* Sound = 0;

//////////////////////////////////////////////////
#include <efx.h>
#define LOAD_PROC(x, type) ((x) = (type)alGetProcAddress(#x))
static LPALEFFECTF alEffectf;
static LPALEFFECTI alEffecti;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALGENEFFECTS alGenEffects;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
//////////////////////////////////////////////////

CSoundRender_Core::CSoundRender_Core()
{
    bPresent = FALSE;
    bEAX = FALSE;
    bDeferredEAX = FALSE;
    geom_OCC = nullptr;
    geom_ENV = NULL;
    geom_SOM = NULL;
    s_environment = NULL;
    Handler = NULL;
    s_emitters_u = 0;
    e_current.set_identity();
    e_target = get_environment_def();
    bReady = FALSE;
    bLocked = FALSE;
    fTimer_Value = Timer.GetElapsed_sec();
    fTimer_Delta = 0.0f;
    m_iPauseCounter = 1;

    efx_reverb = EFX_REVERB_PRESET_GENERIC;
    bEFX = false;
    effect = 0;
    slot = 0;
}

void CSoundRender_Core::release_efx_objects() const
{
    if (alIsEffect(effect))
        alDeleteEffects(1, &effect);

    if (alIsAuxiliaryEffectSlot(slot))
        alDeleteAuxiliaryEffectSlots(1, &slot);
}

CSoundRender_Core::~CSoundRender_Core()
{
    xr_delete(geom_ENV);
    xr_delete(geom_SOM);
}

void CSoundRender_Core::_initialize(int stage)
{
    Timer.Start();

    // load environment
    env_load();
}

void CSoundRender_Core::_clear()
{
    env_unload();

    // remove sources
    for (auto& kv : s_sources)
        xr_delete(kv.second);

    s_sources.clear();

    // remove emmiters
    for (auto& s_emitter : s_emitters)
        xr_delete(s_emitter);

    s_emitters.clear();
}

void CSoundRender_Core::stop_emitters()
{
    for (auto& s_emitter : s_emitters)
        s_emitter->stop(false);
}

int CSoundRender_Core::pause_emitters(bool val)
{
    m_iPauseCounter += val ? +1 : -1;
    VERIFY(m_iPauseCounter >= 0);

    for (const auto& s_emitter : s_emitters)
        s_emitter->pause(val, val ? m_iPauseCounter : m_iPauseCounter + 1);

    return m_iPauseCounter;
}

#define SNDENV_FILENAME_XR "senvironment.xr"
#define SNDENV_FILENAME_LTX "senvironment.ltx"

void CSoundRender_Core::env_load()
{
    // Load environment
    string_path fn;
    if (FS.exist(fn, "$game_data$", SNDENV_FILENAME_LTX))
    {
        Msg("Loading of [%s]", SNDENV_FILENAME_LTX);

        if (!s_environment)
            s_environment = xr_new<SoundEnvironment_LIB>();

        CInifile inifile{fn, 1, 1, 0};

        s_environment->LoadIni(&inifile);
    }
    else if (FS.exist(fn, "$game_data$", SNDENV_FILENAME_XR))
    {
        Msg("Loading of [%s]", SNDENV_FILENAME_XR);

        if (!s_environment)
            s_environment = xr_new<SoundEnvironment_LIB>();

        s_environment->Load(fn);
    }

    if (s_environment)
    {
        for (u32 chunk = 0; chunk < s_environment->Library().size(); chunk++)
        {
            shared_str name = s_environment->Library()[chunk]->name;

            Msg("~ env id=[%d] name=[%s]", chunk, name.c_str());
        }
    }
}

void CSoundRender_Core::env_unload()
{
    // Unload
    if (s_environment)
    {
        if (bSenvironmentXrExport)
        {
            env_save_all();
        }

        s_environment->Unload();
    }

    xr_delete(s_environment);
}

void CSoundRender_Core::env_save_all() const
{
    string_path fn;
    FS.update_path(fn, "$game_data$", SNDENV_FILENAME_LTX);

    CInifile inifile{fn, 0, 1, 1};

    s_environment->SaveIni(&inifile);
}

void CSoundRender_Core::_restart() { env_apply(); }
void CSoundRender_Core::set_handler(sound_event* E) { Handler = E; }
void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M) { geom_OCC = M; }

void CSoundRender_Core::set_geometry_som(IReader* I)
{
    xr_delete(geom_SOM);
    if (0 == I)
        return;

    // check version
    R_ASSERT(I->find_chunk(0));

#ifdef DEBUG
    u32 version =
#endif
        I->r_u32();
    VERIFY2(version == 0, "Invalid SOM version");

    struct SOM_poly
    {
        Fvector3 v1;
        Fvector3 v2;
        Fvector3 v3;
        u32 b2sided;
        float occ;
    };

    CDB::Collector CL;
    {
        // load geometry
        IReader* geom = I->open_chunk(1);
        VERIFY2(geom, "Corrupted SOM file");
        if (!geom)
            return;

        // Load tris and merge them
        const auto begin = static_cast<SOM_poly*>(geom->pointer());
        const auto end = static_cast<SOM_poly*>(geom->end());
        for (SOM_poly* poly = begin; poly != end; ++poly)
        {
            CL.add_face_packed_D(poly->v1, poly->v2, poly->v3, *reinterpret_cast<u32*>(&poly->occ), 0.01f);
            if (poly->b2sided)
                CL.add_face_packed_D(poly->v3, poly->v2, poly->v1, *reinterpret_cast<u32*>(&poly->occ), 0.01f);
        }
        geom->close();
    }

    // Create AABB-tree
    geom_SOM = xr_new<CDB::MODEL>();
    geom_SOM->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()));
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
    xr_delete(geom_ENV);

    if (0 == I)
        return;
    if (0 == s_environment)
        return;

    // Associate names
    xr_vector<u16> ids;
    IReader* names = I->open_chunk(0);
    while (!names->eof())
    {
        string256 n;
        names->r_stringZ(n, sizeof(n));
        int id = s_environment->GetID(n);
        R_ASSERT(id >= 0);
        ids.push_back(u16(id));
        Msg("~ set_geometry_env id=%d name[%s]=environment id[%d]", ids.size() - 1, n, id);
    }
    names->close();

    // Load geometry
    IReader* geom_ch = I->open_chunk(1);

    u8* _data = (u8*)xr_malloc(geom_ch->length());

    memcpy(_data, geom_ch->pointer(), geom_ch->length());

    IReader* geom = xr_new<IReader>(_data, geom_ch->length(), 0);

    hdrCFORM H;
    geom->r(&H, sizeof(hdrCFORM));
    R_ASSERT(H.version == CFORM_CURRENT_VERSION);
    Fvector* verts = (Fvector*)geom->pointer();
    CDB::TRI* tris = (CDB::TRI*)(verts + H.vertcount);

    for (u32 it = 0; it < H.facecount; it++)
    {
        CDB::TRI* T = tris + it;
        const u16 id_front = (u16)((T->dummy & 0x0000ffff) >> 0); //	front face
        const u16 id_back = (u16)((T->dummy & 0xffff0000) >> 16); //	back face
        R_ASSERT(id_front < (u16)ids.size());
        R_ASSERT(id_back < (u16)ids.size());
        T->dummy = u32(ids[id_back] << 16) | u32(ids[id_front]);
    }

    geom_ENV = xr_new<CDB::MODEL>();
    geom_ENV->build(verts, H.vertcount, tris, H.facecount);
    geom_ch->close();
    geom->close();
    xr_free(_data);
}

void CSoundRender_Core::set_master_gain(float low_pass, float high_pass)
{
    if (bEFX)
    {
        master_low_pass = low_pass;
        master_high_pass = high_pass;
    }
}

void CSoundRender_Core::create(ref_sound& S, LPCSTR fName, esound_type sound_type, u32 game_type)
{
    if (!bPresent)
        return;

    S._p = xr_new<ref_sound_data>(fName, sound_type, game_type);
}

void CSoundRender_Core::attach_tail(ref_sound& S, const char* fName)
{
    if (!bPresent)
        return;
    string_path fn;
    xr_strcpy(fn, fName);
    if (strext(fn))
        *strext(fn) = 0;
    if (S._p->fn_attached[0].size() && S._p->fn_attached[1].size())
    {
#ifdef DEBUG
        Msg("! 2 file already in queue [%s][%s]", S._p->fn_attached[0].c_str(), S._p->fn_attached[1].c_str());
#endif // #ifdef DEBUG
        return;
    }

    u32 idx = S._p->fn_attached[0].size() ? 1 : 0;

    S._p->fn_attached[idx] = fn;

    CSoundRender_Source* s = SoundRender->i_create_source(fn);
    S._p->dwBytesTotal += s->bytes_total();
    S._p->fTimeTotal += s->length_sec();
    if (S._feedback())
        ((CSoundRender_Emitter*)S._feedback())->fTimeToStop += s->length_sec();

    SoundRender->i_destroy_source(s);
}

void CSoundRender_Core::clone(ref_sound& S, const ref_sound& from, esound_type sound_type, u32 game_type)
{
    if (!bPresent)
        return;

    S._p = xr_new<ref_sound_data>();
    S._p->handle = from._p->handle;
    S._p->dwBytesTotal = from._p->dwBytesTotal;
    S._p->fTimeTotal = from._p->fTimeTotal;

    S._p->fn_attached[0] = from._p->fn_attached[0];
    S._p->fn_attached[1] = from._p->fn_attached[1];

    S._p->g_type = (game_type == sg_SourceType) ? S._p->handle->game_type() : game_type;
    S._p->s_type = sound_type;
}

void CSoundRender_Core::play(ref_sound& S, CObject* O, u32 flags, float delay)
{
    if (!bPresent || !S._handle())
        return;

    S._p->g_object = O;
    if (S._feedback())
        ((CSoundRender_Emitter*)S._feedback())->rewind();
    else
        i_play(&S, flags & sm_Looped, delay);

    if (flags & sm_2D)
        S._feedback()->switch_to_2D();
}

void CSoundRender_Core::play_no_feedback(ref_sound& S, CObject* O, u32 flags, float delay, Fvector* pos, float* vol, float* freq, Fvector2* range)
{
    if (!bPresent || !S._handle())
        return;

    ref_sound_data_ptr orig = S._p;
    S._p = xr_new<ref_sound_data>();
    S._p->handle = orig->handle;
    S._p->s_type = orig->s_type;

    S._p->g_type = orig->g_type;
    S._p->g_object = O;

    S._p->dwBytesTotal = orig->dwBytesTotal;
    S._p->fTimeTotal = orig->fTimeTotal;

    S._p->fn_attached[0] = orig->fn_attached[0];
    S._p->fn_attached[1] = orig->fn_attached[1];

    i_play(&S, flags & sm_Looped, delay);

    if (pos)
        S._feedback()->set_position(*pos);
    if (vol)
        S._feedback()->set_volume(*vol);
    if (freq)
        S._feedback()->set_frequency(*freq);
    if (range)
        S._feedback()->set_range((*range)[0], (*range)[1]);

    if (flags & sm_2D)
        S._feedback()->switch_to_2D();

    S._p = orig;
}

void CSoundRender_Core::play_at_pos(ref_sound& S, CObject* O, const Fvector& pos, u32 flags, float delay)
{
    if (!bPresent || !S._handle())
        return;

    S._p->g_object = O;
    if (S._feedback())
        ((CSoundRender_Emitter*)S._feedback())->rewind();
    else
        i_play(&S, flags & sm_Looped, delay);

    S._feedback()->set_position(pos);

    if (flags & sm_2D)
        S._feedback()->switch_to_2D();
}

void CSoundRender_Core::destroy(ref_sound& S)
{
    if (S._feedback())
    {
        CSoundRender_Emitter* E = (CSoundRender_Emitter*)S._feedback();
        E->stop(FALSE);
    }

    S._p = nullptr;
}

void CSoundRender_Core::_create_data(ref_sound_data& S, LPCSTR fName, esound_type sound_type, u32 game_type)
{
    string_path fn;
    xr_strcpy(fn, fName);
    if (strext(fn))
        *strext(fn) = 0;
    S.handle = (CSound_source*)SoundRender->i_create_source(fn);
    S.g_type = (game_type == sg_SourceType) ? S.handle->game_type() : game_type;
    S.s_type = sound_type;
    S.feedback = 0;
    S.g_object = 0;
    S.g_userdata = 0;
    S.dwBytesTotal = S.handle->bytes_total();
    S.fTimeTotal = S.handle->length_sec();
}

void CSoundRender_Core::_destroy_data(ref_sound_data& S)
{
    if (S.feedback)
    {
        CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
        E->stop(FALSE);
    }

    R_ASSERT(0 == S.feedback);
    SoundRender->i_destroy_source((CSoundRender_Source*)S.handle);

    S.handle = nullptr;
}

CSoundRender_Environment* CSoundRender_Core::get_environment_def()
{
    static CSoundRender_Environment identity;
    identity.set_identity();
    return &identity;
}

CSoundRender_Environment* CSoundRender_Core::get_environment(const Fvector& P)
{
    if (geom_ENV)
    {
        constexpr Fvector dir = {0, -1, 0};
        CDB::COLLIDER geom_DB;

        geom_DB.ray_query(CDB::OPT_ONLYNEAREST, geom_ENV, P, dir, 1000.f);
        if (geom_DB.r_count())
        {
            CDB::RESULT* r = geom_DB.r_begin();
            CDB::TRI* T = geom_ENV->get_tris() + r->id;
            Fvector* V = geom_ENV->get_verts();

            Fvector tri_norm;
            tri_norm.mknormal(V[T->verts[0]], V[T->verts[1]], V[T->verts[2]]);
            const float dot = dir.dotproduct(tri_norm);
            if (dot <= 0)
            {
                u16 id_front = (u16)((T->dummy & 0x0000ffff) >> 0); //	front face
                return s_environment->Get(id_front);
            }
            else
            {
                u16 id_back = (u16)((T->dummy & 0xffff0000) >> 16); //	back face
                return s_environment->Get(id_back);
            }
        }
    }

    return get_environment_def();
}

void CSoundRender_Core::env_apply() { bListenerMoved = true; }

void CSoundRender_Core::update_listener(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R, float dt)
{
    if (!Listener.position.similar(P))
    {
        Listener.position = P;
        bListenerMoved = true;
    }

    Listener.orientation[0] = D;
    Listener.orientation[1] = N;
    Listener.orientation[2] = R;

    if (bListenerMoved)
    {
        bListenerMoved = false;
        e_target = get_environment(P);
    }

    // update EAX or EFX
    if (!(psSoundFlags.test(ss_EFX) && (bEAX || bEFX)))
        return;

    if (!e_currentPaused)
        e_current.lerp(e_current, *e_target, dt / 2);
    else
        e_current.set_from(*e_target);

    if (bEAX)
    {
        i_eax_listener_set(&e_current);
        i_eax_commit_setting();
    }
    else if (bEFX)
    {
        i_efx_listener_set(&e_current);

        bEFX = i_efx_commit_setting();
        if (!bEFX)
        {
            i_efx_disable();
            release_efx_objects();
        }
    }
}

//////////////////////////////////////////////////
void CSoundRender_Core::InitAlEFXAPI()
{
    using namespace efx_api;

    LOAD_PROC(alGenEffects, LPALGENEFFECTS);
    LOAD_PROC(alDeleteEffects, LPALDELETEEFFECTS);
    LOAD_PROC(alIsEffect, LPALISEFFECT);
    LOAD_PROC(alEffecti, LPALEFFECTI);
    LOAD_PROC(alEffectf, LPALEFFECTF);

    LOAD_PROC(alGenFilters, LPALGENFILTERS);
    LOAD_PROC(alDeleteFilters, LPALDELETEFILTERS);
    LOAD_PROC(alIsFilter, LPALISFILTER);

    LOAD_PROC(alFilteri, LPALFILTERI);
    LOAD_PROC(alFilterf, LPALFILTERF);

    LOAD_PROC(alAuxiliaryEffectSloti, LPALAUXILIARYEFFECTSLOTI);
    LOAD_PROC(alGenAuxiliaryEffectSlots, LPALGENAUXILIARYEFFECTSLOTS);
    LOAD_PROC(alDeleteAuxiliaryEffectSlots, LPALDELETEAUXILIARYEFFECTSLOTS);
    LOAD_PROC(alIsAuxiliaryEffectSlot, LPALISAUXILIARYEFFECTSLOT);
}

bool CSoundRender_Core::EFXTestSupport()
{
    alGenEffects(1, &effect);

    alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
    alEffectf(effect, AL_REVERB_DENSITY, efx_reverb.flDensity);
    alEffectf(effect, AL_REVERB_DIFFUSION, efx_reverb.flDiffusion);
    alEffectf(effect, AL_REVERB_GAIN, efx_reverb.flGain);
    alEffectf(effect, AL_REVERB_GAINHF, efx_reverb.flGainHF);
    alEffectf(effect, AL_REVERB_DECAY_TIME, efx_reverb.flDecayTime);
    alEffectf(effect, AL_REVERB_DECAY_HFRATIO, efx_reverb.flDecayHFRatio);
    alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, efx_reverb.flReflectionsGain);
    alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, efx_reverb.flReflectionsDelay);
    alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, efx_reverb.flLateReverbGain);
    alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, efx_reverb.flLateReverbDelay);
    alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, efx_reverb.flAirAbsorptionGainHF);
    alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, efx_reverb.flRoomRolloffFactor);
    alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, efx_reverb.iDecayHFLimit);

    /* Check if an error occured, and clean up if so. */
    ALenum err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Msg("!![%s] OpenAL error: %s", __FUNCTION__, alGetString(err));
        if (alIsEffect(effect))
            alDeleteEffects(1, &effect);
        return false;
    }

    alGenAuxiliaryEffectSlots(1, &slot);
    err = alGetError();
    ASSERT_FMT_DBG(err == AL_NO_ERROR, "!![%s] OpenAL EFX error: [%s]", __FUNCTION__, alGetString(err));

    return true;
}

static inline float mB_to_gain(float mb) { return powf(10.0f, mb / 2000.0f); }

void CSoundRender_Core::i_efx_listener_set(CSound_environment* _E)
{
    const auto E = static_cast<CSoundRender_Environment*>(_E);
    if (!E)
        return;

    // http://openal.org/pipermail/openal/2014-March/000083.html
    float density = powf(E->EnvironmentSize, 3.0f) / 16.0f;
    if (density > 1.0f)
        density = 1.0f;

    alEffectf(effect, AL_REVERB_DENSITY, density);
    alEffectf(effect, AL_REVERB_DIFFUSION, E->EnvironmentDiffusion);
    alEffectf(effect, AL_REVERB_GAIN, mB_to_gain(E->Room));
    alEffectf(effect, AL_REVERB_GAINHF, mB_to_gain(E->RoomHF));
    alEffectf(effect, AL_REVERB_DECAY_TIME, E->DecayTime);
    alEffectf(effect, AL_REVERB_DECAY_HFRATIO, E->DecayHFRatio);
    alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, mB_to_gain(E->Reflections));
    alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, E->ReflectionsDelay);
    alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, mB_to_gain(E->Reverb));
    alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, E->ReverbDelay);
    alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, mB_to_gain(E->AirAbsorptionHF));
    alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, E->RoomRolloffFactor);
}

bool CSoundRender_Core::i_efx_commit_setting()
{
    alGetError();
    /* Tell the effect slot to use the loaded effect object. Note that the this
     * effectively copies the effect properties. You can modify or delete the
     * effect object afterward without affecting the effect slot.
     */
    alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Msg("!![%s] OpenAL EFX commit error: [%s]. EFX will be disabled.", __FUNCTION__, alGetString(err));
        return false;
    }

    return true;
}
//////////////////////////////////////////////////

void CSoundRender_Core::i_eax_listener_set(CSound_environment* _E)
{
    VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES ep;
    ep.lRoom = iFloor(E->Room); // room effect level at low frequencies
    ep.lRoomHF = iFloor(E->RoomHF); // room effect high-frequency level re. low frequency level
    ep.flRoomRolloffFactor = E->RoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
    ep.flDecayTime = E->DecayTime; // reverberation decay time at low frequencies
    ep.flDecayHFRatio = E->DecayHFRatio; // high-frequency to low-frequency decay time ratio
    ep.lReflections = iFloor(E->Reflections); // early reflections level relative to room effect
    ep.flReflectionsDelay = E->ReflectionsDelay; // initial reflection delay time
    ep.lReverb = iFloor(E->Reverb); // late reverberation level relative to room effect
    ep.flReverbDelay = E->ReverbDelay; // late reverberation delay time relative to initial reflection
    ep.flEnvironmentDiffusion = E->EnvironmentDiffusion; // environment diffusion
    ep.flAirAbsorptionHF = E->AirAbsorptionHF; // change in level per meter at 5 kHz
    ep.dwFlags = EAXLISTENER_DEFAULTFLAGS; // modifies the behavior of properties

    u32 deferred = bDeferredEAX ? DSPROPERTY_EAXLISTENER_DEFERRED : 0;

    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOM, &ep.lRoom, sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMHF, &ep.lRoomHF, sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, &ep.flRoomRolloffFactor, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYTIME, &ep.flDecayTime, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYHFRATIO, &ep.flDecayHFRatio, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONS, &ep.lReflections, sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, &ep.flReflectionsDelay, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERB, &ep.lReverb, sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERBDELAY, &ep.flReverbDelay, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION, &ep.flEnvironmentDiffusion, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, &ep.flAirAbsorptionHF, sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_FLAGS, &ep.dwFlags, sizeof(DWORD));
}

void CSoundRender_Core::i_eax_commit_setting()
{
    // commit eax
    if (bDeferredEAX)
        i_eax_set(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS, nullptr, 0);
}

void CSoundRender_Core::object_relcase(CObject* obj)
{
    if (!obj)
        return;

    for (const auto& s_emitter : s_emitters)
    {
        if (s_emitter && s_emitter->owner_data && s_emitter->owner_data->g_object == obj)
            s_emitter->owner_data->g_object = nullptr;
    }
}
