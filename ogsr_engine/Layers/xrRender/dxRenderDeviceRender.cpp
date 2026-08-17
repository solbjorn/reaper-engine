#include "stdafx.h"

#include "dxRenderDeviceRender.h"

#include "D3DUtils.h"
#include "dxUIRender.h"
#include "ResourceManager.h"

#ifdef XR_USE_RENDERDOC
#include <app/renderdoc_app.h>

namespace
{
RENDERDOC_API_1_0_0* g_renderdoc_api{nullptr};
}
#endif // XR_USE_RENDERDOC

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

    QuadIB = in.QuadIB;
    old_QuadIB = in.old_QuadIB;

    contexts_used = in.contexts_used;
    std::memcpy(static_cast<void*>(&contexts_pool), static_cast<const void*>(&in.contexts_pool), xr::size_bytes(contexts_pool));

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

    for (auto& ctx : contexts_pool)
        ctx.cmd_list.OnDeviceDestroy();

    // Quad
    HW.stats_manager.decrement_stats_ib(QuadIB);
    _RELEASE(QuadIB);
}

tmc::task<void> dxRenderDeviceRender::DestroyHW()
{
    xr_delete(Resources);
    co_await HW.DestroyDevice();
}

tmc::task<void> dxRenderDeviceRender::Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, f32& fWidth_2, f32& fHeight_2)
{
#ifdef DEBUG
    _SHOW_REF("*ref -CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG

    Resources->reset_begin();

    for (auto& ctx : contexts_pool)
    {
        ctx.cmd_list.Index.Destroy();
        ctx.cmd_list.Vertex.Destroy();
    }

    Memory.mem_compact();
    co_await HW.Reset(hWnd, dwWidth, dwHeight);

    fWidth_2 = gsl::narrow_cast<f32>(dwWidth) / 2.0f;
    fHeight_2 = gsl::narrow_cast<f32>(dwHeight) / 2.0f;

    for (auto [id, ctx] : std::views::enumerate(contexts_pool))
    {
        ctx.cmd_list.Vertex.Create(id);
        ctx.cmd_list.Index.Create(id);
    }

    Resources->reset_end();

#ifdef DEBUG
    _SHOW_REF("*ref +CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG
}

void dxRenderDeviceRender::SetupStates()
{
    HW.Caps.Update();

    for (auto& ctx : contexts_pool)
        ctx.cmd_list.SetupStates();
}

tmc::task<void> dxRenderDeviceRender::OnDeviceCreate()
{
    // Signal everyone - device created

    CreateQuadIB();

    for (auto [id, ctx] : std::views::enumerate(contexts_pool))
    {
        ctx.cmd_list.context_id = id;
        ctx.cmd_list.OnDeviceCreate();
    }

    m_Gamma.Update();
    co_await Resources->OnDeviceCreate();
    RImplementation.create();
    Device.Statistic->OnDeviceCreate();

    m_WireShader.create("editor\\wire");
    m_SelectionShader.create("editor\\selection");
    m_PortalFadeShader.create("portal");

    m_PortalFadeGeom.create(FVF::F_L, SGeometry::default_vb(), nullptr);
    XR_ASSERT(m_PortalFadeGeom.stride() == sizeof(FVF::L));

    DUImpl.OnDeviceCreate();
    UIRender->CreateUIGeom();
}

tmc::task<void> dxRenderDeviceRender::Create(HWND hWnd, u32& dwWidth, u32& dwHeight, f32& fWidth_2, f32& fHeight_2)
{
#ifdef XR_USE_RENDERDOC
    if (!g_renderdoc_api)
    {
        static HMODULE hModule = GetModuleHandleA("renderdoc.dll");
        if (!hModule)
            hModule = LoadLibraryA("renderdoc.dll");
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

                Msg("~~[{}] RenderDoc folder: [{}]", std::source_location::current().function_name(), FolderName);

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
#endif // XR_USE_RENDERDOC

    co_await HW.CreateDevice(hWnd, dwWidth, dwHeight);

    fWidth_2 = gsl::narrow_cast<f32>(dwWidth) / 2.0f;
    fHeight_2 = gsl::narrow_cast<f32>(dwHeight) / 2.0f;

    Resources = xr_new<CResourceManager>();
}

void dxRenderDeviceRender::SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF)
{
    HW.Caps.bForceGPU_SW = bForceGPU_SW;
    HW.Caps.bForceGPU_NonPure = bForceGPU_NonPure;
    HW.Caps.bForceGPU_REF = bForceGPU_REF;
}

void dxRenderDeviceRender::DeferredLoad(BOOL E) { Resources->DeferredLoad(E); }
tmc::task<void> dxRenderDeviceRender::ResourcesDeferredUpload() { co_await Resources->DeferredUpload(); }

void dxRenderDeviceRender::ResourcesGetMemoryUsage(xr::render_memory_usage& usage) const
{
    if (Resources != nullptr)
        Resources->_GetMemoryUsage(usage);
    else
        std::memset(&usage, 0, sizeof(usage));
}

void dxRenderDeviceRender::ResourcesDumpMemoryUsage() const { Resources->_DumpMemoryUsage(); }

tmc::task<DeviceState> dxRenderDeviceRender::GetDeviceState() { co_return co_await tmc::spawn_clang(HW.GetDeviceState(), xr::tmc_cpu_st_executor()); }

BOOL dxRenderDeviceRender::GetForceGPU_REF() { return HW.Caps.bForceGPU_REF; }
u32 dxRenderDeviceRender::GetCacheStatPolys() { return RImplementation.get_imm_context().cmd_list.stat.polys; }

void dxRenderDeviceRender::Begin()
{
    XR_TRACY_ZONE_SCOPED();

    get_imm_context().context_id = R__IMM_CTX_ID;
    contexts_used.set(R__IMM_CTX_ID);

    for (auto& ctx : contexts_pool)
        ctx.cmd_list.OnFrameBegin();
}

void dxRenderDeviceRender::Clear()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    cmd_list.ClearZB(cmd_list.get_ZB(), 1.0f, 0);

    if (psDeviceFlags.test(rsClearBB))
        cmd_list.ClearRT(cmd_list.get_RT(), {});
}

tmc::task<void> dxRenderDeviceRender::End()
{
    XR_TRACY_ZONE_SCOPED();

    XR_ASSERT(HW.pDevice != nullptr);

    for (auto& ctx : contexts_pool)
        ctx.cmd_list.OnFrameEnd();

    // we're done with rendering
    cleanup_contexts();

    co_await tmc::spawn_clang(HW.Present(), xr::tmc_cpu_st_executor());
}

void dxRenderDeviceRender::ClearTarget()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    cmd_list.ClearRT(cmd_list.get_RT(), {}); // black
}

void dxRenderDeviceRender::SetCacheXform(Fmatrix& mView, Fmatrix& mProject)
{
    for (auto& ctx : contexts_pool)
    {
        ctx.cmd_list.set_xform_view(mView);
        ctx.cmd_list.set_xform_project(mProject);
    }
}

IResourceManager* dxRenderDeviceRender::GetResourceManager() const { return static_cast<IResourceManager*>(Resources); }

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
    for (auto& ctx : contexts_pool)
        ctx.reset();

    contexts_used.reset();
}
