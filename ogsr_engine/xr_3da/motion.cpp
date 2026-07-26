#include "stdafx.h"

#include "motion.h"
#include "envelope.h"

#define EOBJ_OMOTION 0x1100
#define EOBJ_OMOTION_VERSION 0x0005

#ifdef _LW_EXPORT
extern void ReplaceSpaceAndLowerCase(shared_str& s);
#endif

//------------------------------------------------------------------------------------------
// CCustomMotion
//------------------------------------------------------------------------------------------
CCustomMotion::CCustomMotion()
{
    iFrameStart = 0;
    iFrameEnd = 0;
    fFPS = 30.f;
}

void CCustomMotion::Save(IWriter& F)
{
#ifdef _LW_EXPORT
    ReplaceSpaceAndLowerCase(name);
#endif
    F.w_stringZ(name);
    F.w_u32(iFrameStart);
    F.w_u32(iFrameEnd);
    F.w_float(fFPS);
}

bool CCustomMotion::Load(IReader& F)
{
    F.r_stringZ(name);
    iFrameStart = F.r_u32();
    iFrameEnd = F.r_u32();
    fFPS = F.r_float();
    return true;
}

//------------------------------------------------------------------------------------------
// Object Motion
//------------------------------------------------------------------------------------------
COMotion::COMotion() : CCustomMotion()
{
    mtype = mtObject;
    for (int ch = 0; ch < ctMaxChannel; ch++)
        envs[ch] = xr_new<CEnvelope>();
}

COMotion::~COMotion() { Clear(); }

void COMotion::Clear()
{
    for (int ch = 0; ch < ctMaxChannel; ch++)
        xr_delete(envs[ch]);
}

void COMotion::_Evaluate(float t, Fvector& T, Fvector& R)
{
    T.x = envs[ctPositionX]->Evaluate(t);
    T.y = envs[ctPositionY]->Evaluate(t);
    T.z = envs[ctPositionZ]->Evaluate(t);

    R.y = envs[ctRotationH]->Evaluate(t);
    R.x = envs[ctRotationP]->Evaluate(t);
    R.z = envs[ctRotationB]->Evaluate(t);
}

void COMotion::SaveMotion(const char* buf)
{
    CMemoryWriter F;
    F.open_chunk(EOBJ_OMOTION);
    Save(F);
    F.close_chunk();

    if (!F.save_to(buf))
        Msg("!Can't save object motion: [{}]", buf);
}

bool COMotion::LoadMotion(const char* buf)
{
    const auto F = absl::WrapUnique(FS.r_open(buf));
    XR_ASSERT(F->find_chunk(EOBJ_OMOTION) > 0, "", buf);

    return Load(*F);
}

void COMotion::Save(IWriter& F)
{
    CCustomMotion::Save(F);
    F.w_u16(EOBJ_OMOTION_VERSION);
    for (int ch = 0; ch < ctMaxChannel; ch++)
        envs[ch]->Save(F);
}

bool COMotion::Load(IReader& F)
{
    std::ignore = CCustomMotion::Load(F);

    u16 vers = F.r_u16();
    if (vers == 0x0003)
    {
        Clear();
        for (int ch = 0; ch < ctMaxChannel; ch++)
        {
            envs[ch] = xr_new<CEnvelope>();
            envs[ch]->Load_1(F);
        }
    }
    else if (vers == 0x0004)
    {
        Clear();
        envs[ctPositionX] = xr_new<CEnvelope>();
        envs[ctPositionX]->Load_2(F);
        envs[ctPositionY] = xr_new<CEnvelope>();
        envs[ctPositionY]->Load_2(F);
        envs[ctPositionZ] = xr_new<CEnvelope>();
        envs[ctPositionZ]->Load_2(F);
        envs[ctRotationP] = xr_new<CEnvelope>();
        envs[ctRotationP]->Load_2(F);
        envs[ctRotationH] = xr_new<CEnvelope>();
        envs[ctRotationH]->Load_2(F);
        envs[ctRotationB] = xr_new<CEnvelope>();
        envs[ctRotationB]->Load_2(F);
    }
    else
    {
        if (vers != EOBJ_OMOTION_VERSION)
            return false;
        Clear();
        for (int ch = 0; ch < ctMaxChannel; ch++)
        {
            envs[ch] = xr_new<CEnvelope>();
            envs[ch]->Load_2(F);
        }
    }
    return true;
}

void SAnimParams::Set(float start_frame, float end_frame, float fps)
{
    min_t = start_frame / fps;
    max_t = end_frame / fps;
}

void SAnimParams::Set(CCustomMotion* M)
{
    Set((float)M->FrameStart(), (float)M->FrameEnd(), M->FPS());
    t = min_t;
    //    bPlay=true;
}
void SAnimParams::Update(float dt, float speed, bool loop)
{
    if (!bPlay)
        return;
    bWrapped = false;
    t += speed * dt;
    if (t > max_t)
    {
        bWrapped = true;
        if (loop)
        {
            float len = max_t - min_t;
            float k = float(iFloor((t - min_t) / len));
            t = t - k * len;
        }
        else
            t = max_t;
    }
}
