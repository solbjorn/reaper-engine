#include "stdafx.h"

#include "../xrRender/HW.h"
#include "../xrRender/dxRenderDeviceRender.h"

#ifndef IMGUI_DISABLE
#include "../xrExternal/imgui.h"

namespace
{
[[nodiscard]] f32 get_dpi_scale(ImGuiViewport* viewport)
{
    u32 w, h;
    GetMonitorResolution(w, h);

    const auto base = gsl::narrow_cast<f32>(GetDpiForWindow(static_cast<HWND>(viewport->PlatformHandleRaw))) / f32{USER_DEFAULT_SCREEN_DPI};
    if (Device.dwWidth == w && Device.dwHeight == h)
        return base;

    return base * (gsl::narrow_cast<f32>(Device.dwWidth) / gsl::narrow_cast<f32>(w) + gsl::narrow_cast<f32>(Device.dwHeight) / gsl::narrow_cast<f32>(h)) / 2.0f;
}
} // namespace

void CHW::imgui_init() const
{
    ImGui::SetAllocatorFunctions([](size_t size, void*) { return xr_malloc(gsl::narrow_cast<gsl::index>(size)); }, [](void* ptr, void*) { xr_free(ptr); });
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    string_path fName;

    std::ignore = FS.update_path(fName, "$app_data_root$", io.IniFilename);
    io.IniFilename = xr_strdup(fName);

    std::ignore = FS.update_path(fName, "$logs$", io.LogFilename);
    io.LogFilename = xr_strdup(fName);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    auto viewport = ImGui::GetMainViewport();
    viewport->PlatformHandle = m_hWnd;
    viewport->PlatformHandleRaw = m_hWnd;
    const auto scale = get_dpi_scale(viewport);

    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    ImGui::GetPlatformIO().Platform_GetWindowDpiScale = get_dpi_scale;
    ImGui_ImplDX11_Init(pDevice, contexts_pool[R__IMM_CTX_ID]);

    RECT rect;
    GetClientRect(m_hWnd, &rect);
    io.DisplaySize = ImVec2{gsl::narrow_cast<f32>(rect.right - rect.left), gsl::narrow_cast<f32>(rect.bottom - rect.top)};
}

void CHW::imgui_reset()
{
    if (ImGui::GetCurrentContext() != nullptr)
        ImGui_ImplDX11_InvalidateDeviceObjects();
}

void CHW::imgui_shutdown()
{
    ImGui_ImplDX11_Shutdown();
    auto& io = ImGui::GetIO();

    auto name = const_cast<gsl::zstring>(io.IniFilename);
    xr_free(name);
    io.IniFilename = nullptr;

    name = const_cast<gsl::zstring>(io.LogFilename);
    xr_free(name);
    io.LogFilename = nullptr;

    ImGui::DestroyContext();
}

void dxRenderDeviceRender::editor_new()
{
    XR_TRACY_ZONE_SCOPED();

    static u32 prev{};

    const auto delta = gsl::narrow_cast<f32>(Device.dwTimeContinual - prev) * 0.001f;
    prev = Device.dwTimeContinual;

    ImGui::GetIO().DeltaTime = std::clamp(delta, 0.0f, 0.1f);

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void dxRenderDeviceRender::editor_end()
{
    XR_TRACY_ZONE_SCOPED();

    ImGui::EndFrame();
}

void dxRenderDeviceRender::editor_render()
{
    XR_TRACY_ZONE_SCOPED();

    ImGui::Render();

    auto data = ImGui::GetDrawData();
    if (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f)
        return;

    get_imm_context().cmd_list.ApplyRTandZB();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplDX11_RenderDrawData(data);
}
#else // IMGUI_DISABLE
void CHW::imgui_init() const {}
void CHW::imgui_reset() {}
void CHW::imgui_shutdown() {}

void dxRenderDeviceRender::editor_new() {}
void dxRenderDeviceRender::editor_end() {}
void dxRenderDeviceRender::editor_render() {}
#endif // IMGUI_DISABLE
