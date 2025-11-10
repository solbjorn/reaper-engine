#include "stdafx.h"

#include "dx103DFluidEmitters.h"
#include "dx103DFluidBlenders.h"
#include "dx103DFluidData.h"
#include "dx103DFluidGrid.h"

namespace
{
constexpr const char* strImpulseSize("size");
constexpr const char* strImpulseCenter("center");
constexpr const char* strSplatColor("splatColor");
} // namespace

dx103DFluidEmitters::dx103DFluidEmitters(int gridWidth, int gridHeight, int gridDepth, dx103DFluidGrid* pGrid) : m_pGrid(pGrid)
{
    m_vGridDim[0] = gsl::narrow_cast<f32>(gridWidth);
    m_vGridDim[1] = gsl::narrow_cast<f32>(gridHeight);
    m_vGridDim[2] = gsl::narrow_cast<f32>(gridDepth);

    InitShaders();
}

dx103DFluidEmitters::~dx103DFluidEmitters()
{
    DestroyShaders();
    m_pGrid = nullptr;
}

void dx103DFluidEmitters::InitShaders()
{
    {
        CBlender_fluid_emitter Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (int i = 0; i < 1; ++i)
            m_EmitterTechnique[ET_SimpleGausian + i] = shader->E[i];
    }
}

void dx103DFluidEmitters::DestroyShaders()
{
    for (auto& tech : m_EmitterTechnique)
    {
        //	Release shader's element.
        tech._set(nullptr);
    }
}

void dx103DFluidEmitters::RenderDensity(const dx103DFluidData& FluidData)
{
    const xr_vector<CEmitter>& Emitters = FluidData.GetEmittersList();
    u32 iNumEmitters = Emitters.size();

    for (u32 i = 0; i < iNumEmitters; ++i)
    {
        const CEmitter& Emitter = Emitters[i];
        if (Emitter.m_bApplyDensity)
            ApplyDensity(Emitter);
    }
}

void dx103DFluidEmitters::RenderVelocity(const dx103DFluidData& FluidData)
{
    const xr_vector<CEmitter>& Emitters = FluidData.GetEmittersList();
    u32 iNumEmitters = Emitters.size();

    for (u32 i = 0; i < iNumEmitters; ++i)
    {
        const CEmitter& Emitter = Emitters[i];
        if (Emitter.m_bApplyImpulse)
            ApplyVelocity(Emitter);
    }
}

void dx103DFluidEmitters::ApplyDensity(const CEmitter& Emitter)
{
    // Draw gaussian ball of color
    RCache.set_Element(m_EmitterTechnique[ET_SimpleGausian]);

    float t = Device.fTimeGlobal;

    float fRadius = Emitter.m_fRadius;

    // Color is the density of the smoke. We use a sinusoidal function of 't' to make it more interesting.
    constexpr float fMiddleIntencity = 1.f;
    const float saturation = Emitter.m_fSaturation;
    FLOAT density = 1.5f * (((_sin(t * 1.5f + 2.0f * float(PI) / 3.0f) * 0.5f + 0.5f)) * saturation + fMiddleIntencity * (1.0f - saturation));
    density *= Emitter.m_fDensity;
    Fvector4 color;
    color.set(density, density, density, 1.0f);

    Fvector4 center;
    center.set(Emitter.m_vPosition.x, Emitter.m_vPosition.y, Emitter.m_vPosition.z, 0);

    RCache.set_c(strImpulseSize, fRadius);
    RCache.set_c(strSplatColor, color);
    RCache.set_c(strImpulseCenter, center);

    m_pGrid->DrawSlices();
}

namespace
{
//	TODO: DX10: Remove this hack
[[nodiscard]] f32 lilrand() { return xr::random_f32(-2.5f, 2.5f); }
} // namespace

void dx103DFluidEmitters::ApplyVelocity(const CEmitter& Emitter)
{
    // Draw gaussian ball of velocity
    RCache.set_Element(m_EmitterTechnique[ET_SimpleGausian]);

    float fRadius = Emitter.m_fRadius;
    Fvector FlowVelocity = Emitter.m_vFlowVelocity;

    float t = Device.fTimeGlobal;

    switch (Emitter.m_eType)
    {
    case ET_SimpleDraught: {
        float fPeriod = Emitter.m_DraughtParams.m_fPeriod;
        if (fPeriod < 0.0001f)
            fPeriod = 0.0001f;
        float fFactor = 1.0f + Emitter.m_DraughtParams.m_fAmp * _sin((t + Emitter.m_DraughtParams.m_fPhase) * (2.0f * float(PI)) / fPeriod);
        FlowVelocity.mul(fFactor);
    }
    break;
    default: break;
    }

    RCache.set_c(strImpulseSize, fRadius);

    // Color in this case is the initial velocity given to the emitted smoke
    Fvector4 color;
    color.set(FlowVelocity.x, FlowVelocity.y, FlowVelocity.z, 0);
    RCache.set_c(strSplatColor, color);

    Fvector4 center;
    center.set(Emitter.m_vPosition.x + lilrand(), Emitter.m_vPosition.y + lilrand(), Emitter.m_vPosition.z + lilrand(), 0);
    RCache.set_c(strImpulseCenter, center);

    m_pGrid->DrawSlices();
}
