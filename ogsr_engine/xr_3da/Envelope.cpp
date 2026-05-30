#include "stdafx.h"

#include "envelope.h"

CEnvelope::CEnvelope(const CEnvelope& source) { *this = source; }
CEnvelope::~CEnvelope() { Clear(); }

CEnvelope& CEnvelope::operator=(const CEnvelope& source)
{
    keys.resize(source.keys.size());

    size_t i = 0;
    for (auto key : source.keys)
        keys[i++] = xr_new<st_Key>(*key);

    std::ranges::copy(source.behavior, behavior);

    return *this;
}

void CEnvelope::Clear()
{
    for (KeyIt k_it = keys.begin(); k_it != keys.end(); k_it++)
        xr_delete(*k_it);
}

void CEnvelope::FindNearestKey(float t, KeyIt& min_k, KeyIt& max_k, float eps)
{
    for (KeyIt k_it = keys.begin(); k_it != keys.end(); k_it++)
    {
        if (fsimilar((*k_it)->time, t, eps))
        {
            max_k = k_it + 1;
            min_k = (k_it == keys.begin()) ? k_it : k_it - 1;
            return;
        }
        if ((*k_it)->time > t)
        {
            max_k = k_it;
            min_k = (k_it == keys.begin()) ? k_it : k_it - 1;
            return;
        }
    }
    min_k = keys.empty() ? keys.end() : keys.end() - 1;
    max_k = keys.end();
}

KeyIt CEnvelope::FindKey(float t, float eps)
{
    KeyIt ret = keys.end();

    for (KeyIt k_it = keys.begin(); k_it != ret; k_it++)
    {
        if (fsimilar((*k_it)->time, t, eps))
        {
            ret = k_it;
            return ret;
        }

        if ((*k_it)->time > t)
            return ret;
    }

    return ret;
}

void CEnvelope::InsertKey(float t, float val)
{
    KeyIt k_it = keys.begin();
    for (; k_it != keys.end(); k_it++)
    {
        if (fsimilar((*k_it)->time, t, EPS_L))
        {
            (*k_it)->value = val;
            return;
        }
        // insert before
        if ((*k_it)->time > t)
            break;
    }
    // create _new key
    st_Key* K = xr_new<st_Key>();
    K->time = t;
    K->value = val;
    K->shape = SHAPE_TCB;
    behavior[0] = BEH_CONSTANT;
    behavior[1] = BEH_CONSTANT;
    keys.insert(k_it, K);
}

void CEnvelope::DeleteKey(float t)
{
    for (KeyIt k_it = keys.begin(); k_it != keys.end(); k_it++)
    {
        if (fsimilar((*k_it)->time, t, EPS_L))
        {
            xr_delete(*k_it);
            keys.erase(k_it);
            return;
        }
    }
}

BOOL CEnvelope::ScaleKeys(float from_time, float to_time, float scale_factor, float eps)
{
    KeyIt min_k = FindKey(from_time, eps);
    if (min_k == keys.end())
    {
        KeyIt k0;
        FindNearestKey(from_time, k0, min_k, eps);
    }
    KeyIt max_k = FindKey(to_time, eps);
    if (max_k == keys.end())
    {
        KeyIt k1;
        FindNearestKey(to_time, max_k, k1, eps);
    }
    if (min_k != keys.end() && min_k != max_k)
    {
        if (max_k != keys.end())
            max_k++;
        float t0 = (*min_k)->time;
        float offset = 0;
        KeyIt it = min_k + 1;
        for (; it != max_k; it++)
        {
            float new_time = offset + t0 + ((*it)->time - t0) * scale_factor;
            offset += ((new_time - (*(it - 1))->time) - ((*it)->time - t0));
            t0 = (*it)->time;
            (*it)->time = new_time;
        }
        for (; it != keys.end(); it++)
        {
            float new_time = offset + (*it)->time;
            offset += ((new_time - (*(it - 1))->time) - ((*it)->time - t0));
            (*it)->time = new_time;
        }
        return TRUE;
    }
    return FALSE;
}

float CEnvelope::GetLength(float* mn, float* mx)
{
    if (!keys.empty())
    {
        if (mn)
            *mn = keys.front()->time;
        if (mx)
            *mx = keys.back()->time;
        return keys.back()->time - keys.front()->time;
    }
    if (mn)
        *mn = 0.f;
    if (mx)
        *mx = 0.f;
    return 0.f;
}

void CEnvelope::RotateKeys(float angle)
{
    for (u32 i = 0; i < keys.size(); i++)
        keys[i]->value += angle;
}

float CEnvelope::Evaluate(float time) { return evalEnvelope(this, time); }

void CEnvelope::Save(IWriter& F)
{
    F.w_u8((u8)behavior[0]);
    F.w_u8((u8)behavior[1]);
    F.w_u16((u16)keys.size());
    for (KeyIt k_it = keys.begin(); k_it != keys.end(); k_it++)
        (*k_it)->Save(F);
}

void CEnvelope::Load_1(IReader& F)
{
    Clear();
    F.r(behavior, sizeof(int) * 2);
    int y = F.r_u32();
    keys.resize(y);
    for (u32 i = 0; i < keys.size(); i++)
    {
        keys[i] = xr_new<st_Key>();
        keys[i]->Load_1(F);
    }
}

void CEnvelope::Load_2(IReader& F)
{
    Clear();
    behavior[0] = F.r_u8();
    behavior[1] = F.r_u8();
    keys.resize(F.r_u16());
    for (u32 i = 0; i < keys.size(); i++)
    {
        keys[i] = xr_new<st_Key>();
        keys[i]->Load_2(F);
    }
}

void CEnvelope::SaveA(IWriter&) {}

void CEnvelope::LoadA(IReader& F)
{
    Clear();

    string512 buf;
    F.r_string(buf, sizeof(buf));

    if (strstr(buf, "{ Envelope"))
    {
        F.r_string(buf, sizeof(buf));

        const auto res = scn::scan_int<size_t>(buf);
        R_ASSERT(res, res.error().msg());
        keys.resize(res->value());

        for (u32 i = 0; i < keys.size(); i++)
        {
            keys[i] = xr_new<st_Key>();
            st_Key& K = *keys[i];
            F.r_string(buf, sizeof(buf));

            const auto resf = scn::scan<f32, f32, f32, f32, f32, f32, f32, f32, f32>(std::string_view{buf}, "Key {} {} {} {} {} {} {} {} {}");
            R_ASSERT(resf, resf.error().msg());
            const auto [f0, f1, f2, f3, f4, f5, f6, f7, f8] = resf->values();

            K.value = f0;
            K.time = f1;
            K.shape = gsl::narrow_cast<u8>(f2);

            if (K.shape == SHAPE_TCB)
            {
                K.tension = f3;
                K.continuity = f4;
                K.bias = f5;
            }
            else if (K.shape == SHAPE_BEZ2)
            {
                K.param[0] = f3;
                K.param[1] = f4;
                K.param[2] = f5;
                K.param[3] = f6;
            }
            else
            {
                K.param[0] = f6;
                K.param[1] = f7;
            }
        }

        // behavior <pre> <post>
        F.r_string(buf, sizeof(buf));

        const auto resd = scn::scan<s32, s32>(std::string_view{buf}, "Behaviors {} {}");
        R_ASSERT(resd, resd.error().msg());

        const auto [b0, b1] = resd->values();
        behavior[0] = b0;
        behavior[1] = b1;
    }
}

void CEnvelope::Optimize()
{
    KeyIt it = keys.begin();
    st_Key K = **it;
    it++;
    bool equal = true;
    for (; it != keys.end(); it++)
    {
        if (!(*it)->equal(K))
        {
            equal = false;
            break;
        }
    }
    if (equal && (keys.size() > 2))
    {
        KeyVec new_keys;
        new_keys.push_back(xr_new<st_Key>(*keys.front()));
        new_keys.push_back(xr_new<st_Key>(*keys.back()));
        for (KeyIt k_it = keys.begin(); k_it != keys.end(); k_it++)
            xr_delete(*k_it);
        keys.clear();
        keys = new_keys;
    }
}
