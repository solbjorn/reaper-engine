#include "stdafx.h"
#include "dx10StateCache.h"

dx10StateCache<ID3DRasterizerState, D3D_RASTERIZER_DESC> RSManager;
dx10StateCache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC> DSSManager;
dx10StateCache<ID3DBlendState, D3D_BLEND_DESC> BSManager;

template <class IDeviceState, class StateDecs>
dx10StateCache<IDeviceState, StateDecs>::dx10StateCache()
{
    constexpr int iMasRSStates = 10;
    m_StateArray.reserve(iMasRSStates);
}

template <class IDeviceState, class StateDecs>
dx10StateCache<IDeviceState, StateDecs>::~dx10StateCache()
{
    ClearStateArray();
}

template <>
void dx10StateCache<ID3DRasterizerState, D3D_RASTERIZER_DESC>::ClearStateArray()
{
    for (u32 i = 0; i < m_StateArray.size(); ++i)
    {
        _RELEASE(m_StateArray[i].m_pState);
    }

    m_StateArray.clear();
}

template <>
void dx10StateCache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC>::ClearStateArray()
{
    for (u32 i = 0; i < m_StateArray.size(); ++i)
    {
        _RELEASE(m_StateArray[i].m_pState);
    }

    m_StateArray.clear();
}

template <>
void dx10StateCache<ID3DBlendState, D3D_BLEND_DESC>::ClearStateArray()
{
    for (u32 i = 0; i < m_StateArray.size(); ++i)
    {
        _RELEASE(m_StateArray[i].m_pState);
    }

    m_StateArray.clear();
}

template <>
void dx10StateCache<ID3DRasterizerState, D3D_RASTERIZER_DESC>::CreateState(D3D_RASTERIZER_DESC desc, ID3DRasterizerState** ppIState)
{
    CHK_DX(HW.pDevice->CreateRasterizerState(&desc, ppIState));
}

template <>
void dx10StateCache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC>::CreateState(D3D_DEPTH_STENCIL_DESC desc, ID3DDepthStencilState** ppIState)
{
    CHK_DX(HW.pDevice->CreateDepthStencilState(&desc, ppIState));
}

template <>
void dx10StateCache<ID3DBlendState, D3D_BLEND_DESC>::CreateState(D3D_BLEND_DESC desc, ID3DBlendState** ppIState)
{
    CHK_DX(HW.pDevice->CreateBlendState(&desc, ppIState));
}
