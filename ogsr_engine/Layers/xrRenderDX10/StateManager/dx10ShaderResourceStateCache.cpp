#include "stdafx.h"
#include "dx10ShaderResourceStateCache.h"

dx10ShaderResourceStateCache::dx10ShaderResourceStateCache() { ResetDeviceState(); }

void dx10ShaderResourceStateCache::ResetDeviceState()
{
    ZeroMemory(m_PSViews, sizeof(m_PSViews));
    ZeroMemory(m_GSViews, sizeof(m_GSViews));
    ZeroMemory(m_VSViews, sizeof(m_VSViews));
    ZeroMemory(m_HSViews, sizeof(m_HSViews));
    ZeroMemory(m_DSViews, sizeof(m_DSViews));

    m_uiMinPSView = 0xFFFFFFFF;
    m_uiMaxPSView = 0xFFFFFFFF;

    m_uiMinGSView = 0xFFFFFFFF;
    m_uiMaxGSView = 0xFFFFFFFF;

    m_uiMinVSView = 0xFFFFFFFF;
    m_uiMaxVSView = 0xFFFFFFFF;

    m_uiMinHSView = 0xFFFFFFFF;
    m_uiMaxHSView = 0xFFFFFFFF;

    m_uiMinDSView = 0xFFFFFFFF;
    m_uiMaxDSView = 0xFFFFFFFF;

    m_bUpdatePSViews = false;
    m_bUpdateGSViews = false;
    m_bUpdateVSViews = false;
    m_bUpdateDSViews = false;
    m_bUpdateHSViews = false;
}

void dx10ShaderResourceStateCache::Apply(ctx_id_t context_id)
{
    auto* pContext = HW.get_context(context_id);

    if (m_bUpdatePSViews)
    {
        pContext->PSSetShaderResources(m_uiMinPSView, m_uiMaxPSView - m_uiMinPSView + 1, &m_PSViews[m_uiMinPSView]);
        m_uiMinPSView = 0xFFFFFFFF;
        m_uiMaxPSView = 0xFFFFFFFF;
        m_bUpdatePSViews = false;
    }

    if (m_bUpdateGSViews)
    {
        pContext->GSSetShaderResources(m_uiMinGSView, m_uiMaxGSView - m_uiMinGSView + 1, &m_GSViews[m_uiMinGSView]);
        m_uiMinGSView = 0xFFFFFFFF;
        m_uiMaxGSView = 0xFFFFFFFF;
        m_bUpdateGSViews = false;
    }

    if (m_bUpdateVSViews)
    {
        pContext->VSSetShaderResources(m_uiMinVSView, m_uiMaxVSView - m_uiMinVSView + 1, &m_VSViews[m_uiMinVSView]);
        m_uiMinVSView = 0xFFFFFFFF;
        m_uiMaxVSView = 0xFFFFFFFF;
        m_bUpdateVSViews = false;
    }

    if (m_bUpdateHSViews)
    {
        pContext->HSSetShaderResources(m_uiMinHSView, m_uiMaxHSView - m_uiMinHSView + 1, &m_HSViews[m_uiMinHSView]);
        m_uiMinHSView = 0xFFFFFFFF;
        m_uiMaxHSView = 0xFFFFFFFF;
        m_bUpdateHSViews = false;
    }

    if (m_bUpdateDSViews)
    {
        pContext->DSSetShaderResources(m_uiMinDSView, m_uiMaxDSView - m_uiMinDSView + 1, &m_DSViews[m_uiMinDSView]);
        m_uiMinDSView = 0xFFFFFFFF;
        m_uiMaxDSView = 0xFFFFFFFF;
        m_bUpdateDSViews = false;
    }

    if (m_bUpdateCSViews)
    {
        pContext->CSSetShaderResources(m_uiMinCSView, m_uiMaxCSView - m_uiMinCSView + 1, &m_CSViews[m_uiMinCSView]);
        m_uiMinCSView = 0xFFFFFFFF;
        m_uiMaxCSView = 0xFFFFFFFF;
        m_bUpdateCSViews = false;
    }
}

void dx10ShaderResourceStateCache::SetPSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxPixelShaderTextures);

    if (m_PSViews[uiSlot] != pRes)
    {
        m_PSViews[uiSlot] = pRes;
        if (m_bUpdatePSViews)
        {
            m_uiMinPSView = std::min(m_uiMinPSView, uiSlot);
            m_uiMaxPSView = std::max(m_uiMaxPSView, uiSlot);
        }
        else
        {
            m_bUpdatePSViews = true;
            m_uiMinPSView = uiSlot;
            m_uiMaxPSView = uiSlot;
        }
    }
}

void dx10ShaderResourceStateCache::SetGSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxGeometryShaderTextures);

    if (m_GSViews[uiSlot] != pRes)
    {
        m_GSViews[uiSlot] = pRes;
        if (m_bUpdateGSViews)
        {
            m_uiMinGSView = std::min(m_uiMinGSView, uiSlot);
            m_uiMaxGSView = std::max(m_uiMaxGSView, uiSlot);
        }
        else
        {
            m_bUpdateGSViews = true;
            m_uiMinGSView = uiSlot;
            m_uiMaxGSView = uiSlot;
        }
    }
}

void dx10ShaderResourceStateCache::SetVSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxVertexShaderTextures);

    if (m_VSViews[uiSlot] != pRes)
    {
        m_VSViews[uiSlot] = pRes;
        if (m_bUpdateVSViews)
        {
            m_uiMinVSView = std::min(m_uiMinVSView, uiSlot);
            m_uiMaxVSView = std::max(m_uiMaxVSView, uiSlot);
        }
        else
        {
            m_bUpdateVSViews = true;
            m_uiMinVSView = uiSlot;
            m_uiMaxVSView = uiSlot;
        }
    }
}

void dx10ShaderResourceStateCache::SetHSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxHullShaderTextures);

    if (m_HSViews[uiSlot] != pRes)
    {
        m_HSViews[uiSlot] = pRes;
        if (m_bUpdateHSViews)
        {
            m_uiMinHSView = _min(m_uiMinHSView, uiSlot);
            m_uiMaxHSView = _max(m_uiMaxHSView, uiSlot);
        }
        else
        {
            m_bUpdateHSViews = true;
            m_uiMinHSView = uiSlot;
            m_uiMaxHSView = uiSlot;
        }
    }
}

void dx10ShaderResourceStateCache::SetDSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxHullShaderTextures);

    if (m_DSViews[uiSlot] != pRes)
    {
        m_DSViews[uiSlot] = pRes;
        if (m_bUpdateDSViews)
        {
            m_uiMinDSView = _min(m_uiMinDSView, uiSlot);
            m_uiMaxDSView = _max(m_uiMaxDSView, uiSlot);
        }
        else
        {
            m_bUpdateDSViews = true;
            m_uiMinDSView = uiSlot;
            m_uiMaxDSView = uiSlot;
        }
    }
}

void dx10ShaderResourceStateCache::SetCSResource(u32 uiSlot, ID3DShaderResourceView* pRes)
{
    VERIFY(uiSlot < CTexture::mtMaxComputeShaderTextures);

    if (m_CSViews[uiSlot] != pRes)
    {
        m_CSViews[uiSlot] = pRes;
        if (m_bUpdateCSViews)
        {
            m_uiMinCSView = _min(m_uiMinCSView, uiSlot);
            m_uiMaxCSView = _max(m_uiMaxCSView, uiSlot);
        }
        else
        {
            m_bUpdateCSViews = true;
            m_uiMinCSView = uiSlot;
            m_uiMaxCSView = uiSlot;
        }
    }
}
