#include "stdafx.h"

#include "dx103DFluidManager.h"

#include "../../xrRender/dxRenderDeviceRender.h"
#include "dx103DFluidBlenders.h"
#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"
#include "dx103DFluidRenderer.h"
#include "dx103DFluidObstacles.h"
#include "dx103DFluidEmitters.h"

dx103DFluidManager FluidManager;

namespace
{
// For project, advect
// ModulateShaderVariable = pEffect->GetVariableByName( "modulate")->AsScalar();
constexpr const char* strModulate("modulate");
// For confinement
// EpsilonShaderVariable = pEffect->GetVariableByName( "epsilon")->AsScalar();
constexpr const char* strEpsilon("epsilon");
// For confinement, advect
constexpr const char* strTimeStep("timestep");
// For advect BFECC
// ForwardShaderVariable = pEffect->GetVariableByName( "forward")->AsScalar();
constexpr const char* strForward("forward");
// HalfVolumeDimShaderVariable = pEffect->GetVariableByName( "halfVolumeDim")->AsVector();
constexpr const char* strHalfVolumeDim("halfVolumeDim");

constexpr const char* strGravityBuoyancy("GravityBuoyancy");
} // namespace

dx103DFluidManager::dx103DFluidManager() = default;
dx103DFluidManager::~dx103DFluidManager() { Destroy(); }

void dx103DFluidManager::Initialize(int width, int height, int depth)
{
    // if (strstr(Core.Params,"-no_volumetric_fog"))
    /*if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;*/

    Destroy();

    m_iTextureWidth = width;
    m_iTextureHeight = height;
    m_iTextureDepth = depth;

    InitShaders();

    D3D_TEXTURE3D_DESC desc{};
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.Width = width;
    desc.Height = height;
    desc.Depth = depth;

    D3D_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
    SRVDesc.Texture3D.MipLevels = 1;
    SRVDesc.Texture3D.MostDetailedMip = 0;

    for (int rtIndex = 0; rtIndex < NUM_RENDER_TARGETS; rtIndex++)
    {
        PrepareTexture(rtIndex);
        pRenderTargetViews[rtIndex] = nullptr;
    }

    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
    {
        desc.Format = dx103DFluidConsts::RenderTargetFormats[rtIndex];
        SRVDesc.Format = dx103DFluidConsts::RenderTargetFormats[rtIndex];
        CreateRTTextureAndViews(rtIndex, desc);
    }

    Reset();

    m_pGrid = xr_new<dx103DFluidGrid>();
    m_pGrid->Initialize(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth);

    m_pRenderer = xr_new<dx103DFluidRenderer>();
    m_pRenderer->Initialize(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth);

    m_pObstaclesHandler = xr_new<dx103DFluidObstacles>(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth, m_pGrid);
    m_pEmittersHandler = xr_new<dx103DFluidEmitters>(m_iTextureWidth, m_iTextureHeight, m_iTextureDepth, m_pGrid);

    m_bInited = true;
}

void dx103DFluidManager::Destroy()
{
    if (!m_bInited)
        return;

    //	Destroy grid and renderer here
    xr_delete(m_pEmittersHandler);
    xr_delete(m_pObstaclesHandler);
    xr_delete(m_pRenderer);
    xr_delete(m_pGrid);

    for (int rtIndex = 0; rtIndex < NUM_RENDER_TARGETS; rtIndex++)
        DestroyRTTextureAndViews(rtIndex);

    DestroyShaders();

    m_bInited = false;
}

void dx103DFluidManager::InitShaders()
{
    {
        CBlender_fluid_advect Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 4; ++i)
            m_SimulationTechnique[SS_Advect + i] = shader->E[i];
    }

    {
        CBlender_fluid_advect_velocity Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 2; ++i)
            m_SimulationTechnique[SS_AdvectVel + i] = shader->E[i];
    }

    {
        CBlender_fluid_simulate Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 5; ++i)
            m_SimulationTechnique[SS_Vorticity + i] = shader->E[i];
    }
}

void dx103DFluidManager::DestroyShaders()
{
    for (auto& tech : m_SimulationTechnique)
    {
        //	Release shader's element.
        tech._set(nullptr);
    }
}

void dx103DFluidManager::PrepareTexture(int rtIndex)
{
    pRTTextures[rtIndex]._set(RImplementation.Resources->_CreateTexture(dx103DFluidConsts::m_pEngineTextureNames[rtIndex]));
}

void dx103DFluidManager::CreateRTTextureAndViews(int rtIndex, D3D_TEXTURE3D_DESC TexDesc)
{
    //	Resources must be already released by Destroy().

    // Create the texture
    ID3DTexture3D* pRT;
    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture3D(&TexDesc, nullptr, &pRT)));

    // Create the render target view
    D3D_RENDER_TARGET_VIEW_DESC DescRT{};
    DescRT.Format = TexDesc.Format;
    DescRT.ViewDimension = D3D_RTV_DIMENSION_TEXTURE3D;
    DescRT.Texture3D.FirstWSlice = 0;
    DescRT.Texture3D.MipSlice = 0;
    DescRT.Texture3D.WSize = TexDesc.Depth;

    XR_ASSERT(xr::hr(HW.pDevice->CreateRenderTargetView(pRT, &DescRT, &pRenderTargetViews[rtIndex])));
    pRTTextures[rtIndex]->surface_set(pRT);

    //	CTexture owns ID3DxxTexture3D interface
    pRT->Release();
}

void dx103DFluidManager::DestroyRTTextureAndViews(int rtIndex)
{
    pRTTextures[rtIndex]._set(nullptr);
    _RELEASE(pRenderTargetViews[rtIndex]);
}

void dx103DFluidManager::Reset()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    for (int rtIndex = 0; rtIndex < NUM_OWN_RENDER_TARGETS; rtIndex++)
        cmd_list.ClearRT(pRenderTargetViews[rtIndex], {});
}

void dx103DFluidManager::Update(CBackend& cmd_list, dx103DFluidData& FluidData, f32 timestep)
{
    PIX_EVENT_CTX(cmd_list, simulate_fluid);

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();
    const bool bSimulateFire = (VolumeSettings.m_SimulationType == dx103DFluidData::ST_FIRE);

    AttachFluidData(cmd_list, FluidData);

    // All drawing will take place to a viewport with the dimensions of a 3D texture slice
    D3D_VIEWPORT rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0.0f;
    rtViewport.MaxDepth = 1.0f;

    rtViewport.Width = gsl::narrow_cast<f32>(m_iTextureWidth);
    rtViewport.Height = gsl::narrow_cast<f32>(m_iTextureHeight);

    cmd_list.SetViewport(rtViewport);
    cmd_list.set_ZB(nullptr);

    UpdateObstacles(cmd_list, FluidData, timestep);

    // Set vorticity confinment and decay parameters
    m_fConfinementScale = VolumeSettings.m_fConfinementScale;
    m_fDecay = VolumeSettings.m_fDecay;

    if (m_bUseBFECC)
        AdvectColorBFECC(cmd_list, timestep, bSimulateFire);
    else
        AdvectColor(cmd_list, timestep, bSimulateFire);

    AdvectVelocity(cmd_list, timestep, VolumeSettings.m_fGravityBuoyancy);
    ApplyVorticityConfinement(cmd_list, timestep);
    ApplyExternalForces(cmd_list, FluidData);

    ComputeVelocityDivergence(cmd_list);
    ComputePressure(cmd_list);
    ProjectVelocity(cmd_list);

    DetachAndSwapFluidData(cmd_list, FluidData);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    if (!RImplementation.o.dx10_msaa)
        pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0, {}, {}, pTarget->rt_Base_Depth); // LDR RT
    else
        pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0_r, {}, {}, pTarget->rt_MSAADepth); // LDR RT

    RImplementation.rmNormal(cmd_list);
}

void dx103DFluidManager::AttachFluidData(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, AttachFluidData);

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        ID3DTexture3D* pT = FluidData.GetTexture((dx103DFluidData::eVolumePrivateRT)i);
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(pT);
        _RELEASE(pT);

        XR_ASSERT(pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i] == nullptr);
        pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i] = FluidData.GetView((dx103DFluidData::eVolumePrivateRT)i);
    }
}

void dx103DFluidManager::DetachAndSwapFluidData(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, DetachAndSwapFluidData);

    ID3DTexture3D* pTTarg = (ID3DTexture3D*)pRTTextures[RENDER_TARGET_COLOR]->surface_get();
    ID3DTexture3D* pTSrc = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    FluidData.SetTexture(dx103DFluidData::VP_COLOR, pTTarg);
    pRTTextures[RENDER_TARGET_COLOR]->surface_set(pTSrc);
    _RELEASE(pTSrc);

    ID3DRenderTargetView* pV = FluidData.GetView(dx103DFluidData::VP_COLOR);
    FluidData.SetView(dx103DFluidData::VP_COLOR, pRenderTargetViews[RENDER_TARGET_COLOR]);
    _RELEASE(pRenderTargetViews[RENDER_TARGET_COLOR]);
    pRenderTargetViews[RENDER_TARGET_COLOR] = pV;

    for (int i = 0; i < dx103DFluidData::VP_NUM_TARGETS; ++i)
    {
        pRTTextures[RENDER_TARGET_VELOCITY0 + i]->surface_set(nullptr);
        _RELEASE(pRenderTargetViews[RENDER_TARGET_VELOCITY0 + i]);
    }
}

void dx103DFluidManager::AdvectColorBFECC(CBackend& cmd_list, f32 timestep, bool bTeperature)
{
    PIX_EVENT_CTX(cmd_list, AdvectColorBFECC);

    constexpr Fcolor color{};
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], color);
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], color);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_Advect]);

    cmd_list.set_c(strTimeStep, timestep);
    cmd_list.set_c(strModulate, 1.0f);
    cmd_list.set_c(strForward, 1.0f);

    m_pGrid->DrawSlices(cmd_list);

    // Advect back to get \bar{\phi}
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
    ref_selement AdvectElement;
    if (bTeperature)
        AdvectElement = m_SimulationTechnique[SS_AdvectTemp];
    else
        AdvectElement = m_SimulationTechnique[SS_Advect];

    cmd_list.set_Element(AdvectElement);

    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strColorName(dx103DFluidConsts::m_pEngineTextureNames[RENDER_TARGET_COLOR_IN]);
    STextureList* _T = &*(AdvectElement->passes[0]->T);
    u32 dwTextureStage = _T->find_texture_stage(strColorName);
    //	This will be overritten by the next technique.
    //	Otherwise we had to reset current texture list manually.
    pRTTextures[RENDER_TARGET_TEMPVECTOR]->bind(cmd_list, dwTextureStage);

    cmd_list.set_c(strTimeStep, timestep);
    cmd_list.set_c(strModulate, 1.0f);
    cmd_list.set_c(strForward, -1.0f);

    m_pGrid->DrawSlices(cmd_list);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectBFECCTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectBFECC]);

    Fvector4 halfVol;
    halfVol.set((float)m_iTextureWidth / 2.0f, (float)m_iTextureHeight / 2.0f, (float)m_iTextureDepth / 2.0f, 0.0f);
    cmd_list.set_c(strHalfVolumeDim, halfVol);
    cmd_list.set_c(strModulate, m_fDecay);
    cmd_list.set_c(strForward, 1.0f);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::AdvectColor(CBackend& cmd_list, f32 timestep, bool bTeperature)
{
    PIX_EVENT_CTX(cmd_list, AdvectColor);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);

    if (bTeperature)
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectTemp]);
    else
        cmd_list.set_Element(m_SimulationTechnique[SS_Advect]);

    cmd_list.set_c(strTimeStep, timestep);
    cmd_list.set_c(strModulate, 1.0f);
    cmd_list.set_c(strForward, 1.0f);
    cmd_list.set_c(strModulate, m_fDecay);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::AdvectVelocity(CBackend& cmd_list, f32 timestep, f32 fGravity)
{
    PIX_EVENT_CTX(cmd_list, AdvectVelocity);

    //  Advect velocity by the fluid velocity
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);

    if (_abs(fGravity) < 0.000001)
    {
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectVel]);
    }
    else
    {
        cmd_list.set_Element(m_SimulationTechnique[SS_AdvectVelGravity]);
        cmd_list.set_c(strGravityBuoyancy, fGravity);
    }

    cmd_list.set_c(strTimeStep, timestep);
    cmd_list.set_c(strModulate, 1.0f);
    cmd_list.set_c(strForward, 1.0f);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::ApplyVorticityConfinement(CBackend& cmd_list, f32 timestep)
{
    PIX_EVENT_CTX(cmd_list, ApplyVorticityConfinement);

    // Compute vorticity
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], {});
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);

    cmd_list.set_Element(m_SimulationTechnique[SS_Vorticity]);
    m_pGrid->DrawSlices(cmd_list);

    // Compute and apply vorticity confinement force
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Confinement]);

    cmd_list.set_c(strEpsilon, m_fConfinementScale);
    cmd_list.set_c(strTimeStep, timestep);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::ApplyExternalForces(CBackend& cmd_list, const dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, ApplyExternalForces);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_COLOR]);
    m_pEmittersHandler->RenderDensity(cmd_list, FluidData);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY1]);
    m_pEmittersHandler->RenderVelocity(cmd_list, FluidData);
}

void dx103DFluidManager::ComputeVelocityDivergence(CBackend& cmd_list)
{
    PIX_EVENT_CTX(cmd_list, ComputeVelocityDivergence);

    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], {});

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPVECTOR]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Divergence]);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::ComputePressure(CBackend& cmd_list)
{
    PIX_EVENT_CTX(cmd_list, ComputePressure);

    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], {});

    cmd_list.set_RT(nullptr);
    ref_selement CurrentTechnique = m_SimulationTechnique[SS_Jacobi];
    cmd_list.set_Element(CurrentTechnique);

    //	Find texture index and patch texture manually using DirecX call!
    static const shared_str strPressureName(dx103DFluidConsts::m_pEngineTextureNames[RENDER_TARGET_PRESSURE]);
    STextureList* _T = &*(CurrentTechnique->passes[0]->T);
    u32 dwTextureStage = _T->find_texture_stage(strPressureName);

    for (int iteration = 0; iteration < m_nIterations / 2.0; iteration++)
    {
        cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_TEMPSCALAR]);
        pRTTextures[RENDER_TARGET_PRESSURE]->bind(cmd_list, dwTextureStage);
        m_pGrid->DrawSlices(cmd_list);

        cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_PRESSURE]);
        pRTTextures[RENDER_TARGET_TEMPSCALAR]->bind(cmd_list, dwTextureStage);
        m_pGrid->DrawSlices(cmd_list);
    }
}

void dx103DFluidManager::ProjectVelocity(CBackend& cmd_list)
{
    PIX_EVENT_CTX(cmd_list, ProjectVelocity);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_VELOCITY0]);
    cmd_list.set_Element(m_SimulationTechnique[SS_Project]);

    cmd_list.set_c(strModulate, 1.0f);

    m_pGrid->DrawSlices(cmd_list);
}

void dx103DFluidManager::RenderFluid(CBackend& cmd_list, dx103DFluidData& FluidData)
{
    PIX_EVENT_CTX(cmd_list, render_fluid);

    //	Bind input texture
    ID3DTexture3D* pT = FluidData.GetTexture(dx103DFluidData::VP_COLOR);
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(pT);
    _RELEASE(pT);

    //	Do rendering
    m_pRenderer->Draw(cmd_list, FluidData);

    //	Unbind input texture
    pRTTextures[RENDER_TARGET_COLOR_IN]->surface_set(nullptr);

    //	Restore render state
    CRenderTarget* pTarget = RImplementation.Target;
    if (!RImplementation.o.dx10_msaa)
        pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0, {}, {}, pTarget->rt_Base_Depth); // LDR RT
    else
        pTarget->u_setrt(cmd_list, pTarget->rt_Generic_0_r, {}, {}, pTarget->rt_MSAADepth); // LDR RT

    RImplementation.rmNormal(cmd_list);
}

void dx103DFluidManager::UpdateObstacles(CBackend& cmd_list, const dx103DFluidData& FluidData, float timestep)
{
    PIX_EVENT_CTX(cmd_list, Fluid_update_obstacles);

    //	Reset data
    constexpr Fcolor color{};
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_OBSTACLES], color);
    cmd_list.ClearRT(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], color);

    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTACLES], 0);
    cmd_list.set_RT(pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], 1);

    m_pObstaclesHandler->ProcessObstacles(cmd_list, FluidData, timestep);

    //	Just reset render targets:
    //	later only rt 0 will be reassigned so rt1
    //	would be bound all the time
    //	Reset to avoid confusion.
    cmd_list.set_RT(nullptr, 0);
    cmd_list.set_RT(nullptr, 1);
}

//	Allow real-time config reload
#ifdef DEBUG
void dx103DFluidManager::RegisterFluidData(dx103DFluidData* pData, const xr_string& SectionName)
{
    int iDataNum = m_lstFluidData.size();

    int i;

    for (i = 0; i < iDataNum; ++i)
    {
        if (m_lstFluidData[i] == pData)
            break;
    }

    if (iDataNum == i)
    {
        m_lstFluidData.push_back(pData);
        m_lstSectionNames.push_back(SectionName);
    }
    else
    {
        m_lstSectionNames[i] = SectionName;
    }
}

void dx103DFluidManager::DeregisterFluidData(dx103DFluidData* pData)
{
    int iDataNum = m_lstFluidData.size();

    int i;

    for (i = 0; i < iDataNum; ++i)
    {
        if (m_lstFluidData[i] == pData)
            break;
    }

    if (i != iDataNum)
    {
        xr_vector<xr_string>::iterator it1 = m_lstSectionNames.begin();
        xr_vector<dx103DFluidData*>::iterator it2 = m_lstFluidData.begin();
        // it1.advance(i);
        it1 += i;
        it2 += i;

        m_lstSectionNames.erase(it1);
        m_lstFluidData.erase(it2);
    }
}

void dx103DFluidManager::UpdateProfiles()
{
    const size_t iDataNum = m_lstFluidData.size();

    for (size_t i = 0; i < iDataNum; i++)
        m_lstFluidData[i]->ReparseProfile(m_lstSectionNames[i]);
}

#endif //	DEBUG
