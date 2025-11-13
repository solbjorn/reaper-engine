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

            E = T->get_emitter(); // update can stop itself
            if (E)
                T->priority = E->priority();
        }

        if (!T->get_emitter())
            T->priority = -1.f;
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

    for (auto& E : s_events)
        Handler(E.sound_data, E.range, E.time_to_stop);

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
        for (auto _E : s_emitters)
        {
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
                _I.game_object = nullptr;
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

    if (geom_SOM)
    {
        // Calculate RAY params
        Fvector pos, dir;
        pos.random_dir();
        pos.mul(dispersion);
        pos.add(snd_pt);

        dir.sub(pos, hear_pt);
        const float range = dir.normalize_magn();

        occ_value *= occRayTestSom(pos, dir, range);
    }

    return occ_value;
}

float CSoundRender_Core::get_occlusion(const Fvector& snd_pt, CSoundRender_Core::Occ* occ) { return calc_occlusion(listener_position(), snd_pt, occ); }

float CSoundRender_Core::calc_occlusion(const Fvector& hear_pt, const Fvector& snd_pt, CSoundRender_Core::Occ* occ)
{
    float occ_value = 1.f;

    if (occ->lastFrame == s_emitters_u)
        return occ->occ_value;

    occ->lastFrame = s_emitters_u;

    // Если источник звука находится вплотную к слушателю - нечего и заглушать.
    if (snd_pt.similar(hear_pt, 0.2f))
    {
        occ->occ_value = occ_value;
        return occ_value;
    }

    const Fvector pos = snd_pt;

    Fvector dir;
    dir.sub(hear_pt, pos);
    const float range = dir.normalize_magn();

    if (geom_OCC)
    {
        bool bNeedFullTest = true;

        // 1. Check cached polygon
        float _u, _v, _range;
        if (occ->valid && CDB::TestRayTri(pos, dir, occ->occ, _u, _v, _range, false))
        {
            if (_range > 0 && _range < range)
            {
                occ_value = occ->occ_value;
                bNeedFullTest = false;
            }
        }

        // 2. Polygon doesn't picked up - real database query
        if (bNeedFullTest)
        {
            // Проверяем препятствие в направлении от звука к камере
            occ_value = occRayTestMtl(pos, dir, range, occ);
            if (/*occ->checkReverse && */ occ_value < 1.f)
            {
                // Проверяем препятствие в обратном направлении, от камеры к
                // звуку и выбираем максимальное поглощение звука из этой и
                // предыдущей проверок. Бывают ситуации, когда на локации в
                // прямом и обратном направлениях находятся материалы с разными
                // fSndOcclusionFactor.
                Fvector reverseDir;
                reverseDir.sub(pos, hear_pt).normalize();

                Occ reverseOcc;
                const float reverseVal = occRayTestMtl(hear_pt, reverseDir, range, &reverseOcc);
                if (reverseVal < occ_value)
                    occ_value = reverseVal;
            }
            occ->occ_value = occ_value;
        }
    }

    if (geom_SOM && !fis_zero(occ_value))
        occ_value *= occRayTestSom(pos, dir, range);

    return occ_value;
}

float CSoundRender_Core::occRayTestMtl(const Fvector& pos, const Fvector& dir, float range, Occ* occ)
{
    float occ_value = 1.f;
    occ->valid = false;

    CDB::COLLIDER geom_DB;
    geom_DB.ray_query(CDB::OPT_CULL, geom_OCC, pos, dir, range);

    if (geom_DB.r_count() > 0)
    {
        for (gsl::index i{}; i < geom_DB.r_count(); ++i)
        {
            CDB::RESULT* R = geom_DB.r_begin() + i;
            const CDB::TRI& T = geom_OCC->get_tris()[R->id];

            // Если `pos` находится на поверхности, то из-за ограничений float,
            // точка может оказаться как за поверхностью, так и перед ней, ведь
            // поверхности в игре не имеют толщины. Поэтому будем сравнивать
            // расстояние до пересечения с расстоянием до `pos`, с точностью
            // 0.01. Если совпало, не будет засчитывать это пересечение. Будем
            // считать, что точка находится перед поверхность и это пересечение
            // не должно менять звук.
            if (fsimilar(R->range, range, 0.01f))
                continue;

            SGameMtl* mtl = GMLib.GetMaterialByIdx(T.material);
            if (mtl->Flags.test(SGameMtl::flPassable /*| SGameMtl::flPickable*/))
                continue;

            if (mtl->fSndOcclusionFactor < 1.f)
            {
                if (fis_zero(mtl->fSndOcclusionFactor))
                    occ_value = 0.f;
                else
                    occ_value *= mtl->fSndOcclusionFactor;
            }

            if (!occ->valid)
            {
                const Fvector* V = geom_OCC->get_verts();
                occ->occ[0].set(V[T.verts[0]]);
                occ->occ[1].set(V[T.verts[1]]);
                occ->occ[2].set(V[T.verts[2]]);
                occ->valid = true;
            }

            if (fis_zero(occ_value))
                break;
        }
    }

    return occ_value;
}

float CSoundRender_Core::occRayTestSom(const Fvector& pos, const Fvector& dir, float range) const
{
    float occ_value = 1.f;

    CDB::COLLIDER geom_DB;
    geom_DB.ray_query(CDB::OPT_CULL, geom_SOM, pos, dir, range);
    u32 r_cnt = u32(geom_DB.r_count());
    CDB::RESULT* _B = geom_DB.r_begin();

    if (r_cnt > 0)
    {
        for (u32 k = 0; k < r_cnt; k++)
        {
            CDB::RESULT* R = _B + k;
            occ_value *= std::bit_cast<float>(R->dummy);
            if (fis_zero(occ_value))
                break;
        }
    }

    return occ_value;
}
