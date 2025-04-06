#include "stdafx.h"

#include <efx.h>

#include "../xrCDB/cl_intersect.h"
#include "../xr_3da/device.h"
#include "../xr_3da/gamemtllib.h"

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"
#include "SoundRender_Source.h"

CSoundRender_Emitter* CSoundRender_Core::i_play(ref_sound* S, BOOL _loop, float delay)
{
    VERIFY(!S->_p->feedback);

    CSoundRender_Emitter* E = s_emitters.emplace_back(xr_new<CSoundRender_Emitter>());
    S->_p->feedback = E;
    E->start(S, _loop, delay);

    return E;
}

static u32 g_saved_event_count;

void CSoundRender_Core::update(const Fvector& P, const Fvector& D, const Fvector& N, const Fvector& R)
{
    if (!bReady)
        return;

    Device.Statistic->SoundUpdate.Begin();

    std::scoped_lock slock(m_bLocked);
    bLocked = true;

    Timer.time_factor(psSoundTimeFactor); //--#SM+#--
    const float new_tm = Timer.GetElapsed_sec();
    fTimer_Delta = new_tm - fTimer_Value;
    fTimer_Value = new_tm;

    s_emitters_u++;

    // Firstly update emitters, which are now being rendered
    for (CSoundRender_Target* T : s_targets)
    {
        if (CSoundRender_Emitter* E = T->get_emitter())
        {
            E->update(fTimer_Value, fTimer_Delta);
            E->marker = s_emitters_u;
        }
    }

    // Update emitters
    for (size_t it = 0; it < s_emitters.size(); it++)
    {
        CSoundRender_Emitter* pEmitter = s_emitters[it];
        if (pEmitter->marker != s_emitters_u)
        {
            pEmitter->update(fTimer_Value, fTimer_Delta);
            pEmitter->marker = s_emitters_u;
        }
        if (!pEmitter->isPlaying())
        {
            // Stopped
            xr_delete(pEmitter);
            s_emitters.erase(s_emitters.begin() + it);
            it--;
        }
    }

    // update listener
    update_listener(P, D, N, R, fTimer_Delta);

    // Events
    g_saved_event_count = s_events.size();

    for (auto& [sound, range] : s_events)
        Handler(sound, range);

    s_events.clear();

    bLocked = false;
    Device.Statistic->SoundUpdate.End();
}

void CSoundRender_Core::render()
{
    Device.Statistic->SoundRender.Begin();

    std::scoped_lock slock(m_bLocked);
    bLocked = true;

    for (CSoundRender_Target* T : s_targets)
    {
        if (CSoundRender_Emitter* emitter = T->get_emitter())
            emitter->render();
    }

    bLocked = false;
    Device.Statistic->SoundRender.End();
}

void CSoundRender_Core::i_efx_disable()
{
    for (auto& T : s_targets)
    {
        T->alAuxInit(AL_EFFECTSLOT_NULL);
        T->bEFX = false;
    }
}

void CSoundRender_Core::statistic(CSound_stats* dest, CSound_stats_ext* ext)
{
    if (dest)
    {
        dest->_rendered = 0;

        for (auto T : s_targets)
        {
            if (T->get_emitter() && T->get_Rendering())
                dest->_rendered++;
        }

        dest->_simulated = s_emitters.size();
        dest->_events = g_saved_event_count;
    }
    if (ext)
    {
        for (u32 it = 0; it < s_emitters.size(); it++)
        {
            CSoundRender_Emitter* _E = s_emitters[it];
            CSound_stats_ext::SItem _I;
            _I._3D = !_E->b2D;
            _I._rendered = !!_E->target;
            _I.params = _E->p_source;
            _I.volume = _E->smooth_volume;
            if (_E->owner_data)
            {
                _I.name = _E->source()->fname;
                _I.game_object = _E->owner_data->g_object;
                _I.game_type = _E->owner_data->g_type;
                _I.type = _E->owner_data->s_type;
            }
            else
            {
                _I.game_object = 0;
                _I.game_type = 0;
                _I.type = st_Effect;
            }
            ext->append(_I);
        }
    }
}

float CSoundRender_Core::get_occlusion_to(const Fvector& hear_pt, const Fvector& snd_pt, float dispersion)
{
    float occ_value = 1.f;

    if (nullptr != geom_SOM)
    {
        // Calculate RAY params
        Fvector pos, dir;
        pos.random_dir();
        pos.mul(dispersion);
        pos.add(snd_pt);
        dir.sub(pos, hear_pt);
        const float range = dir.magnitude();
        dir.div(range);

        geom_DB.ray_query(CDB::OPT_CULL, geom_SOM, hear_pt, dir, range);
        const auto r_cnt = geom_DB.r_count();
        CDB::RESULT* begin = geom_DB.r_begin();
        if (0 != r_cnt)
        {
            for (size_t k = 0; k < r_cnt; k++)
            {
                CDB::RESULT* R = begin + k;
                occ_value *= *reinterpret_cast<float*>(&R->dummy);
            }
        }
    }
    return occ_value;
}

float CSoundRender_Core::get_occlusion(const Fvector& P, float R, Fvector* occ)
{
    float occ_value = 1.f;

    // Calculate RAY params
    const Fvector base = listener_position();
    Fvector pos, dir;
    pos.random_dir();
    pos.mul(R);
    pos.add(P);
    dir.sub(pos, base);
    const float range = dir.magnitude();
    dir.div(range);

    if (nullptr != geom_MODEL)
    {
        bool bNeedFullTest = true;
        // 1. Check cached polygon
        float u, v, test_range;
        if (CDB::TestRayTri(base, dir, occ, u, v, test_range, true))
            if (test_range > 0 && test_range < range)
            {
                occ_value = psSoundOcclusionScale;
                bNeedFullTest = false;
            }
        // 2. Polygon doesn't picked up - real database query
        if (bNeedFullTest)
        {
            geom_DB.ray_query(CDB::OPT_ONLYNEAREST, geom_MODEL, base, dir, range);
            if (0 != geom_DB.r_count())
            {
                // cache polygon
                const CDB::RESULT* R2 = geom_DB.r_begin();
                const CDB::TRI& T = geom_MODEL->get_tris()[R2->id];
                const Fvector* V = geom_MODEL->get_verts();
                occ[0].set(V[T.verts[0]]);
                occ[1].set(V[T.verts[1]]);
                occ[2].set(V[T.verts[2]]);

                const SGameMtl* mtl = GMLib.GetMaterialByIdx(T.material);
                const float occlusion = fis_zero(mtl->fSndOcclusionFactor) ? 0.1f : mtl->fSndOcclusionFactor;
                occ_value = psSoundOcclusionScale * occlusion;
            }
        }
    }
    if (nullptr != geom_SOM)
    {
        geom_DB.ray_query(CDB::OPT_CULL, geom_SOM, base, dir, range);
        const auto r_cnt = geom_DB.r_count();
        CDB::RESULT* begin = geom_DB.r_begin();
        if (0 != r_cnt)
        {
            for (size_t k = 0; k < r_cnt; k++)
            {
                CDB::RESULT* R2 = begin + k;
                occ_value *= *reinterpret_cast<float*>(&R2->dummy);
            }
        }
    }
    return occ_value;
}
