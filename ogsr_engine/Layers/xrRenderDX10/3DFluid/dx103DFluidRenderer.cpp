#include "stdafx.h"
#include "dx103DFluidRenderer.h"
#include "dx103DFluidBlenders.h"
#include "../dx10BufferUtils.h"
#include "../../xrRender/dxRenderDeviceRender.h"
#include "dx103DFluidData.h"
#include <DirectXPackedVector.h>

// For render call
constexpr const char* strZNear("ZNear");
constexpr const char* strZFar("ZFar");
constexpr const char* strGridScaleFactor("gridScaleFactor");
constexpr const char* strEyeOnGrid("eyeOnGrid");
constexpr const char* strWorldViewProjection("WorldViewProjection");
constexpr const char* strInvWorldViewProjection("InvWorldViewProjection");
constexpr const char* strRTWidth("RTWidth");
constexpr const char* strRTHeight("RTHeight");

constexpr const char* strDiffuseLight("DiffuseLight");

dx103DFluidRenderer::dx103DFluidRenderer() {}

dx103DFluidRenderer::~dx103DFluidRenderer() { Destroy(); }
void dx103DFluidRenderer::Initialize(int gridWidth, int gridHeight, int gridDepth)
{
    Destroy();

    m_vGridDim[0] = float(gridWidth);
    m_vGridDim[1] = float(gridHeight);
    m_vGridDim[2] = float(gridDepth);

    m_fMaxDim = _max(_max(m_vGridDim[0], m_vGridDim[1]), m_vGridDim[2]);

    // Initialize the grid offset matrix
    {
        // Make a scale matrix to scale the unit-sided box to be unit-length on the
        //  side/s with maximum dimension
        const auto scaleM = DirectX::XMMatrixScaling(m_vGridDim[0] / m_fMaxDim, m_vGridDim[1] / m_fMaxDim, m_vGridDim[2] / m_fMaxDim);

        // offset grid to be centered at origin
        const auto translationM = DirectX::XMMatrixTranslation(-0.5, -0.5, -0.5);

        m_gridMatrix = translationM * scaleM;
    }

    InitShaders();
    CreateGridBox();
    CreateScreenQuad();
    CreateJitterTexture();
    CreateHHGGTexture();

    m_bInited = true;
}

void dx103DFluidRenderer::Destroy()
{
    if (!m_bInited)
        return;

    m_JitterTexture = nullptr;
    m_HHGGTexture = nullptr;

    m_GeomQuadVertex = nullptr;
    _RELEASE(m_pQuadVertexBuffer);

    m_GeomGridBox = nullptr;
    _RELEASE(m_pGridBoxVertexBuffer);
    _RELEASE(m_pGridBoxIndexBuffer);

    DestroyShaders();
}

void dx103DFluidRenderer::InitShaders()
{
    {
        CBlender_fluid_raydata Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (size_t i = 0; i < 3; ++i)
            m_RendererTechnique[RS_CompRayData_Back + i] = shader->E[i];
    }

    {
        CBlender_fluid_raycast Blender;
        ref_shader shader;
        shader.create(&Blender, "null");
        for (size_t i = 0; i < 5; ++i)
            m_RendererTechnique[RS_QuadEdgeDetect + i] = shader->E[i];
    }
}

void dx103DFluidRenderer::DestroyShaders()
{
    for (size_t i = 0; i < RS_NumShaders; ++i)
    {
        //	Release shader's element.
        m_RendererTechnique[i] = nullptr;
    }
}

void dx103DFluidRenderer::CreateGridBox()
{
    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pGridBoxVertexBuffer, dx103DFluidConsts::vertices, sizeof(dx103DFluidConsts::vertices)));

    CHK_DX(dx10BufferUtils::CreateIndexBuffer(&m_pGridBoxIndexBuffer, dx103DFluidConsts::indices, sizeof(dx103DFluidConsts::indices)));
    HW.stats_manager.increment_stats(sizeof(dx103DFluidConsts::indices), enum_stats_buffer_type_index, D3DPOOL_MANAGED);

    // Define the input layout
    static constexpr D3DVERTEXELEMENT9 layout[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};

    // Create the input layout
    m_GeomGridBox.create(layout, m_pGridBoxVertexBuffer, m_pGridBoxIndexBuffer);
}

void dx103DFluidRenderer::CreateScreenQuad()
{
    // Create our quad input layout
    static constexpr D3DVERTEXELEMENT9 quadlayout[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};

    // Create a screen quad for all render to texture operations
    static constexpr Fvector3 svQuad[]{{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}};
    CHK_DX(dx10BufferUtils::CreateVertexBuffer(&m_pQuadVertexBuffer, svQuad, sizeof(svQuad)));
    m_GeomQuadVertex.create(quadlayout, m_pQuadVertexBuffer, 0);
}

void dx103DFluidRenderer::CreateJitterTexture()
{
    BYTE data[256 * 256];
    for (int i = 0; i < 256 * 256; i++)
    {
        data[i] = (unsigned char)(rand() / float(RAND_MAX) * 256);
    }

    D3D_TEXTURE2D_DESC desc{};
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_SHADER_RESOURCE;

    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA dataDesc{};
    dataDesc.pSysMem = data;
    dataDesc.SysMemPitch = 256;

    ID3DTexture2D* NoiseTexture = nullptr;

    CHK_DX(HW.pDevice->CreateTexture2D(&desc, &dataDesc, &NoiseTexture));

    m_JitterTexture = RImplementation.Resources->_CreateTexture("$user$NVjitterTex");
    m_JitterTexture->surface_set(NoiseTexture);

    _RELEASE(NoiseTexture);
}

template <size_t iNumSamples>
struct HHGG_Gen
{
    // cubic b-spline
    consteval float bsW0(float a) { return (1.0f / 6.0f * (-(a * a * a) + (3.0f * a * a) - (3.0f * a) + 1.0f)); }
    consteval float bsW1(float a) { return (1.0f / 6.0f * ((3.0f * a * a * a) - (6.0f * a * a) + 4.0f)); }
    consteval float bsW2(float a) { return (1.0f / 6.0f * (-(3.0f * a * a * a) + (3.0f * a * a) + (3.0f * a) + 1.0f)); }
    consteval float bsW3(float a) { return (1.0f / 6.0f * a * a * a); }
    consteval float g0(float a) { return (bsW0(a) + bsW1(a)); }
    consteval float g1(float a) { return (bsW2(a) + bsW3(a)); }
    consteval float h0texels(float a) { return (1.0f + a - (bsW1(a) / (bsW0(a) + bsW1(a)))); }
    consteval float h1texels(float a) { return (1.0f - a + (bsW3(a) / (bsW2(a) + bsW3(a)))); }

    consteval HHGG_Gen()
    {
        for (size_t i = 0; i < iNumSamples; i++)
        {
            float a = i / (float)(iNumSamples - 1);
            data[4 * i] = -h0texels(a);
            data[4 * i + 1] = h1texels(a);
            data[4 * i + 2] = 1.0f - g0(a);
            data[4 * i + 3] = g0(a);
        }
    }
    float data[4 * iNumSamples]{};
};

void dx103DFluidRenderer::CreateHHGGTexture()
{
    constexpr size_t iNumSamples = 16;
    constexpr auto hhgg_tex = HHGG_Gen<iNumSamples>();

    //	Min value is -1
    //	Max value is +1
    DirectX::PackedVector::HALF converted[std::size(hhgg_tex.data)];
    DirectX::PackedVector::XMConvertFloatToHalfStream(converted, sizeof(converted[0]), hhgg_tex.data, sizeof(hhgg_tex.data[0]), std::size(hhgg_tex.data));

    D3D_TEXTURE1D_DESC desc{};
    desc.Width = iNumSamples;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA dataDesc{};
    dataDesc.pSysMem = converted;
    dataDesc.SysMemPitch = sizeof(converted);

    ID3DTexture1D* HHGGTexture = nullptr;

    CHK_DX(HW.pDevice->CreateTexture1D(&desc, &dataDesc, &HHGGTexture));

    m_HHGGTexture = RImplementation.Resources->_CreateTexture("$user$NVHHGGTex");
    m_HHGGTexture->surface_set(HHGGTexture);

    _RELEASE(HHGGTexture);
}

void dx103DFluidRenderer::SetScreenSize(int width, int height) { CreateRayDataResources(width, height); }
void dx103DFluidRenderer::CalculateRenderTextureSize(int screenWidth, int screenHeight)
{
    int maxProjectedSide = int(3.0 * _sqrt(3.0) * m_fMaxDim);
    int maxScreenDim = _max(screenWidth, screenHeight);

    if (maxScreenDim > maxProjectedSide)
    {
        m_iRenderTextureWidth = screenWidth / 8;
        m_iRenderTextureHeight = screenHeight / 8;
    }
    else
    {
        m_iRenderTextureWidth = screenWidth;
        m_iRenderTextureHeight = screenHeight;
    }
}

void dx103DFluidRenderer::CreateRayDataResources(int width, int height)
{
    // find a good resolution for raycasting purposes
    CalculateRenderTextureSize(width, height);

    RT[0] = nullptr;
    RT[0].create(dx103DFluidConsts::m_pRTNames[0], width, height, dx103DFluidConsts::RTFormats[0]);

    for (size_t i = 1; i < RRT_NumRT; ++i)
    {
        RT[i] = nullptr;
        RT[i].create(dx103DFluidConsts::m_pRTNames[i], m_iRenderTextureWidth, m_iRenderTextureHeight, dx103DFluidConsts::RTFormats[i]);
    }
}

void dx103DFluidRenderer::Draw(const dx103DFluidData& FluidData)
{
    //	We don't need ZB anyway
    RCache.set_ZB(nullptr);

    CRenderTarget* pTarget = RImplementation.Target;
    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();
    const bool bRenderFire = (VolumeSettings.m_SimulationType == dx103DFluidData::ST_FIRE);

    FogLighting LightData;

    CalculateLighting(FluidData, LightData);

    //	Set shader element to set up all necessary constants to constant buffer
    //	If you change constant buffer layout make sure this hack works ok.
    RCache.set_Element(m_RendererTechnique[RS_CompRayData_Back]);

    // Ray cast and render to a temporary buffer
    //=========================================================================

    // Compute the ray data required by the raycasting pass below.
    //  This function will render to a buffer of float4 vectors, where
    //  xyz is starting position of the ray in grid space
    //  w is the length of the ray in view space
    ComputeRayData(FluidData);

    // Do edge detection on this image to find any
    //  problematic areas where we need to raycast at higher resolution
    ComputeEdgeTexture(FluidData);

    // Raycast into the temporary render target:
    //  raycasting is done at the smaller resolution, using a fullscreen quad
    RCache.ClearRT(RT[RRT_RayCastTex], {});

    pTarget->u_setrt(RCache, RT[RRT_RayCastTex], nullptr, nullptr, nullptr); // LDR RT

    RImplementation.rmNormal(RCache);

    if (bRenderFire)
        RCache.set_Element(m_RendererTechnique[RS_QuadRaycastFire]);
    else
        RCache.set_Element(m_RendererTechnique[RS_QuadRaycastFog]);

    PrepareCBuffer(FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);
    DrawScreenQuad();

    // Render to the back buffer sampling from the raycast texture that we just created
    //  If and edge was detected at the current pixel we will raycast again to avoid
    //  smoke aliasing artifacts at scene edges
    if (!RImplementation.o.dx10_msaa)
        pTarget->u_setrt(RCache, pTarget->rt_Generic_0, nullptr, nullptr, pTarget->get_base_zb()); // LDR RT
    else
        pTarget->u_setrt(RCache, pTarget->rt_Generic_0_r, nullptr, nullptr, pTarget->rt_MSAADepth); // LDR RT

    if (bRenderFire)
        RCache.set_Element(m_RendererTechnique[RS_QuadRaycastCopyFire]);
    else
        RCache.set_Element(m_RendererTechnique[RS_QuadRaycastCopyFog]);

    RImplementation.rmNormal(RCache);

    PrepareCBuffer(FluidData, Device.dwWidth, Device.dwHeight);
    RCache.set_c(strDiffuseLight, LightData.m_vLightIntencity.x, LightData.m_vLightIntencity.y, LightData.m_vLightIntencity.z, 1.0f);

    DrawScreenQuad();
}

void dx103DFluidRenderer::ComputeRayData(const dx103DFluidData& FluidData)
{
    // Clear the color buffer to 0
    RCache.ClearRT(RT[RRT_RayDataTex], {});

    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(RCache, RT[RRT_RayDataTex], nullptr, nullptr, nullptr); // LDR RT
    RCache.set_Element(m_RendererTechnique[RS_CompRayData_Back]);

    RImplementation.rmNormal(RCache);

    PrepareCBuffer(FluidData, Device.dwWidth, Device.dwHeight);

    // Render volume back faces
    // We output xyz=(0,-1,0) and w=min(sceneDepth, boxDepth)
    DrawBox();

    // Render volume front faces using subtractive blending
    // We output xyz="position in grid space" and w=boxDepth,
    //  unless the pixel is occluded by the scene, in which case we output xyzw=(1,0,0,0)
    pTarget->u_setrt(RCache, RT[RRT_RayDataTex], nullptr, nullptr, nullptr); // LDR RT
    RCache.set_Element(m_RendererTechnique[RS_CompRayData_Front]);
    PrepareCBuffer(FluidData, Device.dwWidth, Device.dwHeight);

    // Render
    DrawBox();
}

void dx103DFluidRenderer::ComputeEdgeTexture(const dx103DFluidData& FluidData)
{
    CRenderTarget* pTarget = RImplementation.Target;
    pTarget->u_setrt(RCache, RT[RRT_RayDataTexSmall], nullptr, nullptr, nullptr); // LDR RT
    RCache.set_Element(m_RendererTechnique[RS_QuadDownSampleRayDataTexture]);

    // First setup viewport to match the size of the destination low-res texture
    RImplementation.rmNormal(RCache);

    PrepareCBuffer(FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);

    // Downsample the rayDataTexture to a new small texture, simply using point sample (no filtering)
    DrawScreenQuad();

    // Create an edge texture, performing edge detection on 'rayDataTexSmall'
    pTarget->u_setrt(RCache, RT[RRT_EdgeTex], nullptr, nullptr, nullptr); // LDR RT
    RCache.set_Element(m_RendererTechnique[RS_QuadEdgeDetect]);
    PrepareCBuffer(FluidData, m_iRenderTextureWidth, m_iRenderTextureHeight);

    // Render
    DrawScreenQuad();
}

void dx103DFluidRenderer::DrawScreenQuad()
{
    RCache.set_Geometry(m_GeomQuadVertex);
    RCache.Render(D3DPT_TRIANGLESTRIP, 0, 2);
}

void dx103DFluidRenderer::DrawBox()
{
    RCache.set_Geometry(m_GeomGridBox);
    RCache.Render(D3DPT_TRIANGLELIST, 0, 0, dx103DFluidConsts::m_iGridBoxVertNum, 0, dx103DFluidConsts::m_iGridBoxFaceNum);
}

void dx103DFluidRenderer::CalculateLighting(const dx103DFluidData& FluidData, FogLighting& LightData)
{
    m_lstRenderables.clear();

    LightData.Reset();

    const dx103DFluidData::Settings& VolumeSettings = FluidData.GetSettings();

    Fvector4 hemi_color = g_pGamePersistent->Environment().CurrentEnv->hemi_color;
    hemi_color.mul(VolumeSettings.m_fHemi);
    LightData.m_vLightIntencity.set(hemi_color.x, hemi_color.y, hemi_color.z);
    LightData.m_vLightIntencity.add(g_pGamePersistent->Environment().CurrentEnv->ambient);

    const Fmatrix& Transform = FluidData.GetTransform();

    Fbox box;
    box.min.set(-0.5f, -0.5f, -0.5f);
    box.max.set(0.5f, 0.5f, 0.5f);
    box.xform(Transform);
    Fvector3 center;
    Fvector3 size;
    box.getcenter(center);
    box.getradius(size);

    // Traverse object database
    g_SpatialSpace->q_box(m_lstRenderables,
                          0, // ISpatial_DB::O_ORDERED,
                          STYPE_LIGHTSOURCE, center, size);

    // Determine visibility for dynamic part of scene
    for (ISpatial* spatial : m_lstRenderables)
    {
        // Light
        light* pLight = (light*)spatial->dcast_Light();
        R_ASSERT(pLight);

        if (pLight->flags.bMoveable || pLight->flags.bHudMode)
            continue;

        float d = pLight->position.distance_to(Transform.c);

        float R = pLight->range + _max(size.x, _max(size.y, size.z));
        if (d >= R)
            continue;

        Fvector3 LightIntencity{pLight->color.r, pLight->color.g, pLight->color.b};

        float r = pLight->range;
        float a = clampr(1.f - d / (r + EPS), 0.f, 1.f) * (pLight->flags.bStatic ? 1.f : 2.f);

        LightIntencity.mul(a);

        LightData.m_vLightIntencity.add(LightIntencity);
    }
}

void dx103DFluidRenderer::PrepareCBuffer(const dx103DFluidData& FluidData, u32 RTWidth, u32 RTHeight)
{
    using namespace DirectX;

    const Fmatrix& transform = FluidData.GetTransform();
    RCache.set_xform_world(transform);

    // The near and far planes are used to unproject the scene's z-buffer values
    RCache.set_c(strZNear, VIEWPORT_NEAR);
    RCache.set_c(strZFar, g_pGamePersistent->Environment().CurrentEnv->far_plane);

    auto WorldView = transform.mm * RCache.xforms.m_v.mm;

    // The length of one of the axis of the worldView matrix is the length of longest side of the box
    //  in view space. This is used to convert the length of a ray from view space to grid space.
    const float worldScale = XMVectorGetX(XMVector3Length(WorldView.r[0]));
    RCache.set_c(strGridScaleFactor, worldScale);

    // We prepend the current world matrix with this other matrix which adds an offset (-0.5, -0.5, -0.5)
    //  and scale factors to account for unequal number of voxels on different sides of the volume box.
    // This is because we want to preserve the aspect ratio of the original simulation grid when
    //  raytracing through it.
    WorldView = m_gridMatrix * WorldView;

    // worldViewProjection is used to transform the volume box to screen space
    const auto WorldViewProjection = WorldView * RCache.xforms.m_p.mm;
    RCache.set_c(strWorldViewProjection, *reinterpret_cast<const Fmatrix*>(&WorldViewProjection));

    // invWorldViewProjection is used to transform positions in the "near" plane into grid space
    const auto InvWorldViewProjection = XMMatrixInverse(nullptr, WorldViewProjection);
    RCache.set_c(strInvWorldViewProjection, *reinterpret_cast<const Fmatrix*>(&InvWorldViewProjection));

    // Compute the inverse of the worldView matrix
    const auto WorldViewInv = XMMatrixInverse(nullptr, WorldView);
    // Compute the eye's position in "grid space" (the 0-1 texture coordinate cube)
    const auto EyeInGridSpace = XMVector3Transform(XMVectorSet(0, 0, 0, 0), WorldViewInv);
    RCache.set_c(strEyeOnGrid, *reinterpret_cast<const Fvector4*>(&EyeInGridSpace));

    RCache.set_c(strRTWidth, (float)RTWidth);
    RCache.set_c(strRTHeight, (float)RTHeight);
}
