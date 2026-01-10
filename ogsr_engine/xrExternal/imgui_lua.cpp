#include "pch.h"

#include "imgui.h"

// ImGui Sol bindings

#ifdef XR_IMGUI_LUA
#include <sol_ImGui.cpp>

#define ImGuiChildFlags_Border ImGuiChildFlags_Borders
#define ImGuiPopupFlags_MouseButtonDefault_ ImGuiPopupFlags_MouseButtonRight
#define ImGuiTabBarFlags_FittingPolicyResizeDown ImGuiTabBarFlags_FittingPolicyShrink
#define ImGuiTreeNodeFlags_NavLeftJumpsBackHere ImGuiTreeNodeFlags_NavLeftJumpsToParent

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wnewline-eof");

#include <sol_ImGui_Enums.cpp>

XR_DIAG_POP();

#undef ImGuiChildFlags_Border
#undef ImGuiPopupFlags_MouseButtonDefault_
#undef ImGuiTabBarFlags_FittingPolicyResizeDown
#undef ImGuiTreeNodeFlags_NavLeftJumpsBackHere

#include <imgui_internal.h>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wimplicit-int-float-conversion");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sol_ImGui_Input.cpp>

XR_DIAG_POP();

namespace ImGui
{
inline void PushFont(ImFont* font) { ImGui::PushFont(font, font != nullptr ? font->LegacySize : 0.0f); }
} // namespace ImGui

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sol_ImGui_Layout.cpp>
#include <sol_ImGui_Popups.cpp>
#include <sol_ImGui_Tables.cpp>
#include <sol_ImGui_Utility.cpp>
#include <sol_ImGui_Widgets.cpp>

XR_DIAG_POP();

namespace ImGui
{
namespace
{
void SetWindowFontScale(float scale)
{
    IM_ASSERT(scale > 0.0f);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->FontWindowScale = scale;
    ImGui::UpdateCurrentFontSize(0.0f);
}
} // namespace
} // namespace ImGui

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sol_ImGui_Windows.cpp>

XR_DIAG_POP();
#endif // XR_IMGUI_LUA
