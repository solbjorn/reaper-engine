#include "stdafx.h"

#include "ObjectAnimator.h"

#include "motion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectAnimator::CObjectAnimator()
{
    bLoop = false;
    m_Current = nullptr;
    m_Speed = 1.f;
    m_Name._set("");
}

CObjectAnimator::~CObjectAnimator() { Clear(); }

void CObjectAnimator::Clear()
{
    for (auto& m_Motion : m_Motions)
        xr_delete(m_Motion);

    m_Motions.clear();
    SetActiveMotion(nullptr);
}

void CObjectAnimator::SetActiveMotion(COMotion* mot)
{
    m_Current = mot;
    if (m_Current)
        m_MParam.Set(m_Current);

    m_XFORM.identity();
}

void CObjectAnimator::LoadMotions(LPCSTR fname)
{
    string_path full_path;
    XR_ASSERT(FS.exist(full_path, "$level$", fname) || FS.exist(full_path, "$game_anims$", fname), "can't find motion file", fname);

    LPCSTR ext = strext(full_path);
    if (ext)
    {
        Clear();

        if (0 == xr_strcmp(ext, ".anm"))
        {
            COMotion* M = xr_new<COMotion>();
            XR_ASSERT(M->LoadMotion(full_path), "can't load motion", full_path);
            m_Motions.push_back(M);
        }
        else if (0 == xr_strcmp(ext, ".anms"))
        {
            const auto F = absl::WrapUnique(FS.r_open(full_path));
            const auto dwMCnt = XR_ASSERT_VAL(F->r_u32() > 0);

            for (u32 i = 0; i < dwMCnt; i++)
            {
                COMotion* M = xr_new<COMotion>();
                XR_ASSERT(M->Load(*F), "can't load motion", full_path, dwMCnt, i);
                m_Motions.push_back(M);
            }
        }

        std::ranges::sort(m_Motions, [](COMotion* a, COMotion* b) { return a->name < b->name; });
    }
}

void CObjectAnimator::Load(const char* name)
{
    m_Name._set(name);
    LoadMotions(name);
    SetActiveMotion(nullptr);
}

void CObjectAnimator::Update(float dt)
{
    if (m_Current)
    {
        Fvector R, P;
        m_Current->_Evaluate(m_MParam.Frame(), P, R);
        m_MParam.Update(dt, m_Speed, bLoop);
        m_XFORM.setXYZi(R.x, R.y, R.z);
        m_XFORM.translate_over(P);
    }
}

COMotion* CObjectAnimator::Play(bool loop, LPCSTR name)
{
    if (name != nullptr && name[0] != '\0')
    {
        auto it = std::ranges::lower_bound(m_Motions, name, [](auto a, auto b) { return std::is_lt(xr_strcmp(a, b)); }, &COMotion::name);
        XR_ASSERT(it != m_Motions.end() && std::is_eq(xr_strcmp((*it)->Name(), name)), "anim cycle not found", name);

        bLoop = loop;
        SetActiveMotion(*it);
        m_MParam.Play();

        return *it;
    }
    else
    {
        XR_ASSERT(!m_Motions.empty(), "anim cycle not found", name);

        bLoop = loop;
        SetActiveMotion(m_Motions.front());
        m_MParam.Play();

        return m_Motions.front();
    }
}

void CObjectAnimator::Stop()
{
    SetActiveMotion(nullptr);
    m_MParam.Stop();
}

float CObjectAnimator::GetLength() const
{
    if (!m_Current)
        return 0.0f;
    float res = m_Current->Length() / m_Current->FPS();
    return res;
}
