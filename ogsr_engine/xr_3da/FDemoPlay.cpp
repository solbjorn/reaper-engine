// CDemoPlay.cpp: implementation of the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "fdemoplay.h"
#include "xr_ioconsole.h"
#include "motion.h"
#include "Render.h"
#include "CameraManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoPlay::CDemoPlay(const char* name, float ms, u32 cycles, float life_time) : CEffectorCam(cefDemo, life_time /*,FALSE*/)
{
    Msg("*** Playing demo: %s", name);
    // Console->Execute("hud_weapon 0");
    if (g_bBenchmark)
        Console->Execute("hud_draw 0");

    fSpeed = ms;
    dwCyclesLeft = cycles ? cycles : 1;

    m_pMotion = 0;
    m_MParam = 0;
    string_path nm, fn;
    strcpy_s(nm, sizeof(nm), name);

    if (strext(nm))
        strcpy(strext(nm), ".anm");

    if (FS.exist(fn, "$level$", nm) || FS.exist(fn, "$game_anims$", nm))
    {
        m_pMotion = xr_new<COMotion>();
        m_pMotion->LoadMotion(fn);
        m_MParam = xr_new<SAnimParams>();
        m_MParam->Set(m_pMotion);
        m_MParam->Play();
    }
    else
    {
        if (!FS.exist(name))
        {
            g_pGameLevel->Cameras().RemoveCamEffector(cefDemo);
            return;
        }
        IReader* fs = FS.r_open(name);
        u32 sz = fs->length();
        if (sz % sizeof(Fmatrix) != 0)
        {
            FS.r_close(fs);
            g_pGameLevel->Cameras().RemoveCamEffector(cefDemo);
            return;
        }

        seq.resize(sz / sizeof(Fmatrix));
        m_count = seq.size();
        CopyMemory(&*seq.begin(), fs->pointer(), sz);
        FS.r_close(fs);
        Msg("~ Total key-frames: [%d]", m_count);
    }
    stat_started = FALSE;
    Device.PreCache(50, false, false);
}

CDemoPlay::~CDemoPlay()
{
    stat_Stop();
    xr_delete(m_pMotion);
    xr_delete(m_MParam);
    // Console->Execute("hud_weapon 1");
    if (g_bBenchmark)
        Console->Execute("hud_draw 1");
}

void CDemoPlay::stat_Start()
{
    if (stat_started)
        return;
    stat_started = TRUE;
    Sleep(1);
    stat_StartFrame = Device.dwFrame;
    stat_Timer_frame.Start();
    stat_Timer_total.Start();
    stat_table.clear();
    stat_table.reserve(1024);
    fStartTime = 0;
}

extern string512 g_sBenchmarkName;

void CDemoPlay::stat_Stop()
{
    if (!stat_started)
        return;
    stat_started = FALSE;
    float stat_total = stat_Timer_total.GetElapsed_sec();

    float rfps_min, rfps_max, rfps_middlepoint, rfps_average;

    // total
    u32 dwFramesTotal = Device.dwFrame - stat_StartFrame;
    rfps_average = float(dwFramesTotal) / stat_total;

    // min/max/average
    rfps_min = flt_max;
    rfps_max = flt_min;
    rfps_middlepoint = 0;
    for (u32 it = 1; it < stat_table.size(); it++)
    {
        float fps = 1.f / stat_table[it];
        if (fps < rfps_min)
            rfps_min = fps;
        if (fps > rfps_max)
            rfps_max = fps;
        rfps_middlepoint += fps;
    }
    rfps_middlepoint /= float(stat_table.size() - 1);

    Msg("* [DEMO] FPS: average[%f], min[%f], max[%f], middle[%f]", rfps_average, rfps_min, rfps_max, rfps_middlepoint);

    if (g_bBenchmark)
    {
        string_path fname;

        if (xr_strlen(g_sBenchmarkName))
            sprintf_s(fname, sizeof(fname), "%s.result", g_sBenchmarkName);
        else
            strcpy_s(fname, sizeof(fname), "benchmark.result");

        FS.update_path(fname, "$app_data_root$", fname);
        CInifile res(fname, FALSE, FALSE, TRUE);
        res.w_float("general", "renderer", 9.f);
        res.w_float("general", "min", rfps_min);
        res.w_float("general", "max", rfps_max);
        res.w_float("general", "average", rfps_average);
        res.w_float("general", "middle", rfps_middlepoint);
        for (u32 it = 1; it < stat_table.size(); it++)
        {
            string32 id;
            sprintf_s(id, sizeof(id), "%7d", it);
            for (u32 c = 0; id[c]; c++)
                if (' ' == id[c])
                    id[c] = '0';
            res.w_float("per_frame_stats", id, 1.f / stat_table[it]);
        }

        Console->Execute("quit");
    }
}

#define FIX(a) \
    while (a >= m_count) \
    a -= m_count
void spline1(float t, const Fvector4* p, Fvector4* ret)
{
    float t2 = t * t;
    float t3 = t2 * t;
    Fvector4 m, mul;

    ret->set(0, 0, 0, 0);
    m.set((0.5f * ((-1.0f * t3) + (2.0f * t2) + (-1.0f * t))), (0.5f * ((3.0f * t3) + (-5.0f * t2) + (0.0f * t) + 2.0f)), (0.5f * ((-3.0f * t3) + (4.0f * t2) + (1.0f * t))),
          (0.5f * ((1.0f * t3) + (-1.0f * t2) + (0.0f * t))));

    mul.mul(p[0], m.x);
    ret->add(mul);

    mul.mul(p[1], m.y);
    ret->add(mul);

    mul.mul(p[2], m.z);
    ret->add(mul);

    mul.mul(p[3], m.w);
    ret->add(mul);
}

BOOL CDemoPlay::ProcessCam(SCamEffectorInfo& info)
{
    // skeep a few frames before counting
    if (Device.dwPrecacheFrame)
        return TRUE;
    stat_Start();

    // Per-frame statistics
    {
        stat_table.push_back(stat_Timer_frame.GetElapsed_sec());
        stat_Timer_frame.Start();
    }

    // Process motion
    if (m_pMotion)
    {
        Fvector R;
        Fmatrix mRotate;
        m_pMotion->_Evaluate(m_MParam->Frame(), info.p, R);
        m_MParam->Update(Device.fTimeDelta, 1.f, true);
        fLifeTime -= Device.fTimeDelta;
        if (m_MParam->bWrapped)
        {
            stat_Stop();
            stat_Start();
        }
        mRotate.setXYZi(R.x, R.y, R.z);
        info.d.set(mRotate.k);
        info.n.set(mRotate.j);
    }
    else
    {
        if (seq.empty())
        {
            g_pGameLevel->Cameras().RemoveCamEffector(cefDemo);
            return TRUE;
        }

        fStartTime += Device.fTimeDelta;

        float ip;
        float p = fStartTime / fSpeed;
        float t = modff(p, &ip);
        int frame = iFloor(ip);
        VERIFY(t >= 0);

        if (frame >= m_count)
        {
            dwCyclesLeft--;
            if (0 == dwCyclesLeft)
                return FALSE;
            fStartTime = 0;
            // just continue
            // stat_Stop			();
            // stat_Start			();
        }

        int f1 = frame;
        FIX(f1);
        int f2 = f1 + 1;
        FIX(f2);
        int f3 = f2 + 1;
        FIX(f3);
        int f4 = f3 + 1;
        FIX(f4);

        Fmatrix *m1, *m2, *m3, *m4;
        Fvector4 v[4];
        m1 = (Fmatrix*)&seq[f1];
        m2 = (Fmatrix*)&seq[f2];
        m3 = (Fmatrix*)&seq[f3];
        m4 = (Fmatrix*)&seq[f4];

        for (int i = 0; i < 4; i++)
        {
            v[0] = m1->vm[i];
            v[1] = m2->vm[i];
            v[2] = m3->vm[i];
            v[3] = m4->vm[i];
            spline1(t, &v[0], &Device.mView.vm[i]);
        }

        const auto& mInvCamera = Device.mInvView;
        info.n.set(mInvCamera._21, mInvCamera._22, mInvCamera._23);
        info.d.set(mInvCamera._31, mInvCamera._32, mInvCamera._33);
        info.p.set(mInvCamera._41, mInvCamera._42, mInvCamera._43);

        fLifeTime -= Device.fTimeDelta;
    }
    return TRUE;
}
