#include "stdafx.h"

#include "dx10State.h"

#include "dx10StateCache.h"

dx10State::dx10State() = default;

dx10State* dx10State::Create(SimulatorStates& state_code)
{
    dx10State* pState = xr_new<dx10State>();
    state_code.UpdateState(*pState);

    pState->m_pRasterizerState = RSManager.GetState(state_code);
    pState->m_pDepthStencilState = DSSManager.GetState(state_code);
    pState->m_pBlendState = BSManager.GetState(state_code);
    // ID3DxxDevice::CreateSamplerState

    //	Create samplers here
    InitSamplers(pState->m_VSSamplers, state_code, CTexture::rstVertex);
    InitSamplers(pState->m_PSSamplers, state_code, CTexture::rstPixel);
    InitSamplers(pState->m_GSSamplers, state_code, CTexture::rstGeometry);
    InitSamplers(pState->m_HSSamplers, state_code, CTexture::rstHull);
    InitSamplers(pState->m_DSSamplers, state_code, CTexture::rstDomain);
    InitSamplers(pState->m_CSSamplers, state_code, CTexture::rstCompute);

    return pState;
}

HRESULT dx10State::Apply(CBackend& cmd_list)
{
    const auto context_id = cmd_list.context_id;

    cmd_list.StateManager.SetRasterizerState(XR_ASSERT_VAL(m_pRasterizerState != nullptr));
    cmd_list.StateManager.SetDepthStencilState(XR_ASSERT_VAL(m_pDepthStencilState != nullptr));

    if (m_uiStencilRef != std::numeric_limits<u32>::max())
        cmd_list.StateManager.SetStencilRef(m_uiStencilRef);

    cmd_list.StateManager.SetBlendState(XR_ASSERT_VAL(m_pBlendState != nullptr));
    cmd_list.StateManager.SetAlphaRef(cmd_list, m_uiAlphaRef);

    SSManager.GSApplySamplers(context_id, m_GSSamplers);
    SSManager.VSApplySamplers(context_id, m_VSSamplers);
    SSManager.PSApplySamplers(context_id, m_PSSamplers);
    SSManager.HSApplySamplers(context_id, m_HSSamplers);
    SSManager.DSApplySamplers(context_id, m_DSSamplers);
    SSManager.CSApplySamplers(context_id, m_CSSamplers);

    return S_OK;
}

void dx10State::Release()
{
    auto pState = this;
    xr_delete(pState);
}

void dx10State::InitSamplers(tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
    D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]{};
    bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]{};

    for (auto& desc : descArray)
        dx10StateUtils::ResetDescription(desc);

    state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

    const auto view = SamplerUsed | std::views::enumerate | std::views::reverse;

    if (const auto it = std::ranges::find_if(view, [] [[nodiscard]] (auto elem) { return std::get<1>(elem); }); it != view.end())
        SamplerArray.assign_range(std::views::zip(SamplerUsed, descArray) | std::views::take(std::get<0>(*it) + 1) |
                                  std::views::transform([] [[nodiscard]] (auto tuple) {
                                      return std::get<0>(tuple) ? SSManager.GetState(std::get<1>(tuple)) : dx10SamplerStateCache::hInvalidHandle;
                                  }));
}
