#ifndef __XRGAME_EMBEDDED_EDITOR_HELPER_H
#define __XRGAME_EMBEDDED_EDITOR_HELPER_H

#include "../xrExternal/imgui.h"

#ifndef IMGUI_DISABLE
namespace xr
{
namespace detail
{
struct ImguiWnd
{
    explicit ImguiWnd(gsl::czstring name, bool* pOpen = nullptr)
    {
        Opened = pOpen == nullptr || *pOpen;
        if (Opened)
            Collapsed = !ImGui::Begin(name, pOpen);
        else
            Collapsed = true;
    }

    ~ImguiWnd()
    {
        if (Opened)
            ImGui::End();
    }

    bool Collapsed;
    bool Opened;
};

[[nodiscard]] bool ImGui_HoverDragFloat(gsl::czstring label, f32& v, f32 v_speed, f32 v_min, f32 v_max, gsl::czstring format);
[[nodiscard]] bool ImGui_HoverDragFloat3(gsl::czstring label, std::span<f32, 3> v, f32 v_speed, f32 v_min, f32 v_max, gsl::czstring format);

[[nodiscard]] bool SelectFile(gsl::czstring label, gsl::czstring initial, shared_str& file_name);
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE

#endif // !__XRGAME_EMBEDDED_EDITOR_HELPER_H
