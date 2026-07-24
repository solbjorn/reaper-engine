#include "stdafx.h"

#include "dx10SamplerStateCache.h"

#include "../dx10StateUtils.h"

using dx10StateUtils::operator==;

dx10SamplerStateCache SSManager;

dx10SamplerStateCache::dx10SamplerStateCache()
{
    static constexpr std::size_t iMaxRSStates{10};
    m_StateArray.reserve(iMaxRSStates);
}

dx10SamplerStateCache::~dx10SamplerStateCache() { ClearStateArray(); }

dx10SamplerStateCache::SHandle dx10SamplerStateCache::GetState(D3D_SAMPLER_DESC& desc)
{
    SHandle hResult;

    //	MaxAnisitropy is reset by ValidateState if not aplicable
    //	to the filter mode used.
    desc.MaxAnisotropy = m_uiMaxAnisotropy;
    // RZ
    desc.MipLODBias = m_uiMipLODBias;

    dx10StateUtils::ValidateState(desc);

    u64 xxh = dx10StateUtils::GetHash(desc);

    hResult = FindState(desc, xxh);

    if (hResult == hInvalidHandle)
    {
        StateRecord rec;
        rec.m_xxh = xxh;
        CreateState(desc, &rec.m_pState);
        hResult = m_StateArray.size();
        m_StateArray.push_back(rec);
    }

    return hResult;
}

void dx10SamplerStateCache::CreateState(StateDecs desc, IDeviceState** ppIState) { XR_ASSERT(xr::hr(HW.pDevice->CreateSamplerState(&desc, ppIState))); }

dx10SamplerStateCache::SHandle dx10SamplerStateCache::FindState(const StateDecs& desc, u64 StateXXH)
{
    for (auto [i, rec] : std::views::enumerate(m_StateArray))
    {
        if (rec.m_xxh != StateXXH)
            continue;

        StateDecs descCandidate;
        rec.m_pState->GetDesc(&descCandidate);

        if (descCandidate == desc)
            return i;
    }

    return hInvalidHandle;
}

void dx10SamplerStateCache::ClearStateArray()
{
    for (auto& rec : m_StateArray)
        _RELEASE(rec.m_pState);

    m_StateArray.clear();
}

void dx10SamplerStateCache::PrepareSamplerStates(HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]) const
{
    XR_ASSERT(samplers.size() <= D3D_COMMONSHADER_SAMPLER_SLOT_COUNT);

    for (auto [samp, state] : std::views::zip(samplers, std::span{pSS, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT}))
    {
        if (samp != hInvalidHandle)
            state = m_StateArray[samp].m_pState;
    }
}

void dx10SamplerStateCache::VSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->VSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::PSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->PSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::GSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->GSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::HSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->HSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::DSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->DSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::CSApplySamplers(u32 context_id, HArray& samplers)
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->CSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::SetMaxAnisotropy(u32 uiMaxAniso)
{
    clamp(uiMaxAniso, 1u, 16u);

    if (m_uiMaxAnisotropy == uiMaxAniso)
        return;

    m_uiMaxAnisotropy = uiMaxAniso;

    for (auto& rec : m_StateArray)
    {
        StateDecs desc{};
        rec.m_pState->GetDesc(&desc);

        //	MaxAnisitropy is reset by ValidateState if not aplicable
        //	to the filter mode used.
        //	Reason: all checks for aniso applicability are done
        //	in ValidateState.
        desc.MaxAnisotropy = m_uiMaxAnisotropy;
        dx10StateUtils::ValidateState(desc);

        //	This can cause fragmentation if called too often
        rec.m_pState->Release();
        CreateState(desc, &rec.m_pState);
    }
}

void dx10SamplerStateCache::SetMipLODBias(float uiMipLODBias)
{
    if (fsimilar(m_uiMipLODBias, uiMipLODBias))
        return;

    m_uiMipLODBias = uiMipLODBias;

    for (auto& rec : m_StateArray)
    {
        StateDecs desc{};
        rec.m_pState->GetDesc(&desc);

        desc.MipLODBias = m_uiMipLODBias;
        dx10StateUtils::ValidateState(desc);

        // This can cause fragmentation if called too often
        rec.m_pState->Release();
        CreateState(desc, &rec.m_pState);
    }
}
