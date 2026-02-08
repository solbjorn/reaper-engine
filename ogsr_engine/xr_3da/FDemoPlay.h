// CDemoPlay.h: interface for the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "effector.h"

// refs
class COMotion;
struct SAnimParams;

// class
class CDemoPlay : public CEffectorCam
{
    RTTI_DECLARE_TYPEINFO(CDemoPlay, CEffectorCam);

private:
    COMotion* m_pMotion{};
    SAnimParams* m_MParam{};

    xr_vector<Fmatrix> seq;
    int m_count;
    float fStartTime{};
    float fSpeed;
    u32 dwCyclesLeft;

    // statistics
    BOOL stat_started;
    CTimer stat_Timer_frame;
    CTimer stat_Timer_total;
    u32 stat_StartFrame{};
    xr_vector<float> stat_table;

    tmc::task<void> stat_Start();
    void stat_Stop();

public:
    explicit CDemoPlay(const char* name, float ms, u32 cycles, float life_time = 60.0f * 60.0f * 1000.0f);
    ~CDemoPlay() override;

    tmc::task<bool> ProcessCam(SCamEffectorInfo& info) override;
};
