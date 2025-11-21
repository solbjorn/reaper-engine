#include "stdafx.h"

#include "dxRenderDeviceRender.h"

#include "D3DUtils.h"
#include "dxUIRender.h"
#include "ResourceManager.h"

#define USE_RENDERDOC

#ifdef USE_RENDERDOC
#include <../RenderDoc/renderdoc_app.h>

namespace
{
RENDERDOC_API_1_0_0* g_renderdoc_api{};
}
#endif

dxRenderDeviceRender::dxRenderDeviceRender() = default;
dxRenderDeviceRender::~dxRenderDeviceRender() = default;

void dxRenderDeviceRender::Copy(IRenderDeviceRender& _in)
{
    auto& in{*smart_cast<const dxRenderDeviceRender*>(&_in)};

    Resources = in.Resources;

    m_WireShader = in.m_WireShader;
    m_SelectionShader = in.m_SelectionShader;
    m_PortalFadeShader = in.m_PortalFadeShader;
    m_PortalFadeGeom = in.m_PortalFadeGeom;

    Vertex = in.Vertex;
    Index = in.Index;

    QuadIB = in.QuadIB;
    old_QuadIB = in.old_QuadIB;

    contexts_used = in.contexts_used;
    std::ranges::copy(in.contexts_pool, contexts_pool);

    m_Gamma = in.m_Gamma;
    b_loaded = in.b_loaded;
}

void dxRenderDeviceRender::setGamma(float fGamma) { m_Gamma.Gamma(fGamma); }
void dxRenderDeviceRender::setBrightness(float fGamma) { m_Gamma.Brightness(fGamma); }
void dxRenderDeviceRender::setContrast(float fGamma) { m_Gamma.Contrast(fGamma); }
void dxRenderDeviceRender::updateGamma() { m_Gamma.Update(); }

void dxRenderDeviceRender::OnDeviceDestroy(BOOL bKeepTextures)
{
    UIRender->DestroyUIGeom();

    m_PortalFadeGeom.destroy();
    m_PortalFadeShader.destroy();
    m_SelectionShader.destroy();
    m_WireShader.destroy();

    Resources->OnDeviceDestroy(bKeepTextures);

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        contexts_pool[id].cmd_list.OnDeviceDestroy();

    // Quad
    HW.stats_manager.decrement_stats_ib(QuadIB);
    _RELEASE(QuadIB);

    // streams
    Index.Destroy();
    Vertex.Destroy();
}

void dxRenderDeviceRender::DestroyHW()
{
    xr_delete(Resources);
    HW.DestroyDevice();
}

void dxRenderDeviceRender::Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
#ifdef DEBUG
    _SHOW_REF("*ref -CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG

    Resources->reset_begin();
    Memory.mem_compact();
    HW.Reset(hWnd);

    dwWidth = HW.m_ChainDesc.Width;
    dwHeight = HW.m_ChainDesc.Height;

    fWidth_2 = float(dwWidth / 2);
    fHeight_2 = float(dwHeight / 2);
    Resources->reset_end();

#ifdef DEBUG
    _SHOW_REF("*ref +CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG
}

void dxRenderDeviceRender::SetupStates()
{
    HW.Caps.Update();

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        contexts_pool[id].cmd_list.SetupStates();
}

void dxRenderDeviceRender::OnDeviceCreate()
{
    // Signal everyone - device created

    // streams
    Vertex.Create();
    Index.Create();

    CreateQuadIB();

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
    {
        contexts_pool[id].cmd_list.context_id = id;
        contexts_pool[id].cmd_list.OnDeviceCreate();
    }

    m_Gamma.Update();
    Resources->OnDeviceCreate();
    RImplementation.create();
    Device.Statistic->OnDeviceCreate();

    m_WireShader.create("editor\\wire");
    m_SelectionShader.create("editor\\selection");
    m_PortalFadeShader.create("portal");
    m_PortalFadeGeom.create(FVF::F_L, Vertex.Buffer(), nullptr);

    DUImpl.OnDeviceCreate();
    UIRender->CreateUIGeom();
}

void dxRenderDeviceRender::Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
#ifdef USE_RENDERDOC
    if (!g_renderdoc_api)
    {
        static HMODULE hModule = GetModuleHandle("renderdoc.dll");
        if (!hModule)
            hModule = LoadLibrary("renderdoc.dll");
        if (hModule)
        {
            const auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(GetProcAddress(hModule, "RENDERDOC_GetAPI"));
            const auto Result = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_0_0, reinterpret_cast<void**>(&g_renderdoc_api));

            if (Result == 1)
            {
                g_renderdoc_api->UnloadCrashHandler();

                string_path FolderName{};
                std::ignore = FS.update_path(FolderName, "$app_data_root$", "renderdoc_captures\\");
                VerifyPath(FolderName);
                g_renderdoc_api->SetCaptureFilePathTemplate(FolderName);
                Msg("~~[%s] RenderDoc folder: [%s]", __FUNCTION__, FolderName);

                RENDERDOC_InputButton CaptureButton[]{eRENDERDOC_Key_Home};
                g_renderdoc_api->SetCaptureKeys(CaptureButton, std::size(CaptureButton));
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_AllowVSync, 0);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);

                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_RefAllResources, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_VerifyBufferAccess, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_APIValidation, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, 1);
            }
        }
    }
#endif

    HW.CreateDevice(hWnd);

    dwWidth = HW.m_ChainDesc.Width;
    dwHeight = HW.m_ChainDesc.Height;

    fWidth_2 = gsl::narrow_cast<f32>(dwWidth / 2);
    fHeight_2 = gsl::narrow_cast<f32>(dwHeight / 2);

    Resources = xr_new<CResourceManager>();
}

void dxRenderDeviceRender::SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF)
{
    HW.Caps.bForceGPU_SW = bForceGPU_SW;
    HW.Caps.bForceGPU_NonPure = bForceGPU_NonPure;
    HW.Caps.bForceGPU_REF = bForceGPU_REF;
}

void dxRenderDeviceRender::overdrawBegin()
{
    //	TODO: DX10: Implement overdrawBegin
    VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
}

void dxRenderDeviceRender::overdrawEnd()
{
    //	TODO: DX10: Implement overdrawEnd
    VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
}

void dxRenderDeviceRender::DeferredLoad(BOOL E) { Resources->DeferredLoad(E); }

void dxRenderDeviceRender::ResourcesDeferredUpload() { Resources->DeferredUpload(); }

void dxRenderDeviceRender::ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
    if (Resources)
        Resources->_GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
}

void dxRenderDeviceRender::ResourcesDumpMemoryUsage() { Resources->_DumpMemoryUsage(); }

DeviceState dxRenderDeviceRender::GetDeviceState() { return HW.GetDeviceState(); }

BOOL dxRenderDeviceRender::GetForceGPU_REF() { return HW.Caps.bForceGPU_REF; }

u32 dxRenderDeviceRender::GetCacheStatPolys() { return RCache.stat.polys; }

void dxRenderDeviceRender::Begin()
{
    get_imm_context().context_id = R__IMM_CTX_ID;
    contexts_used.set(R__IMM_CTX_ID);

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
    {
        contexts_pool[id].cmd_list.OnFrameBegin();
        contexts_pool[id].cmd_list.set_CullMode(CULL_CW);
        contexts_pool[id].cmd_list.set_CullMode(CULL_CCW);
    }

    Vertex.Flush();
    Index.Flush();

    if (HW.Caps.SceneMode)
        overdrawBegin();
}

void dxRenderDeviceRender::Clear()
{
    RCache.ClearZB(RCache.get_ZB(), 1.0f, 0);

    if (psDeviceFlags.test(rsClearBB))
        RCache.ClearRT(RCache.get_RT(), {});
}

void dxRenderDeviceRender::End()
{
    VERIFY(HW.pDevice);

    if (HW.Caps.SceneMode)
        overdrawEnd();

    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        contexts_pool[id].cmd_list.OnFrameEnd();

    // we're done with rendering
    cleanup_contexts();

    HW.Present();
}

void dxRenderDeviceRender::ClearTarget()
{
    RCache.ClearRT(RCache.get_RT(), {}); // black
}

void dxRenderDeviceRender::SetCacheXform(Fmatrix& mView, Fmatrix& mProject)
{
    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
    {
        contexts_pool[id].cmd_list.set_xform_view(mView);
        contexts_pool[id].cmd_list.set_xform_project(mProject);
    }
}

void dxRenderDeviceRender::OnAssetsChanged()
{
    Resources->m_textures_description.UnLoad();
    Resources->m_textures_description.Load();
}

IResourceManager* dxRenderDeviceRender::GetResourceManager() const { return smart_cast<IResourceManager*>(Resources); }

ctx_id_t dxRenderDeviceRender::alloc_context()
{
    const auto id = contexts_used.first_zero();
    if (id == std::numeric_limits<size_t>::max())
        return R__INVALID_CTX_ID;

    contexts_used.set(id);

    auto& dsgraph = contexts_pool[id];

    dsgraph.reset();
    dsgraph.context_id = id;
    dsgraph.cmd_list.context_id = id;

    return id;
}

void dxRenderDeviceRender::cleanup_contexts()
{
    for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
        contexts_pool[id].reset();

    contexts_used.reset();
}
