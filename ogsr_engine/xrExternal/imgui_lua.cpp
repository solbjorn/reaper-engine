#include "pch.h"

#include "imgui.h"

// ImGui Sol bindings

#ifdef XR_IMGUI_LUA
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define ImGuiCol_NavHighlight ImGuiCol_NavCursor
#define ImGuiCol_TabActive ImGuiCol_TabSelected
#define ImGuiCol_TabUnfocused ImGuiCol_TabDimmed
#define ImGuiCol_TabUnfocusedActive ImGuiCol_TabDimmedSelected
#define ImGuiColorEditFlags_AlphaPreview 0

#define ImGuiConfigFlags_NavEnableSetMousePos (1 << 2)
#define ImGuiConfigFlags_NavNoCaptureKeyboard (1 << 3)
#define ImGuiConfigFlags_DpiEnableScaleFonts (1 << 14)
#define ImGuiConfigFlags_DpiEnableScaleViewports (1 << 15)

#define ImGuiDockNodeFlags_NoDockingInCentralNode ImGuiDockNodeFlags_NoDockingOverCentralNode
#define ImGuiDockNodeFlags_NoSplit ImGuiDockNodeFlags_NoDockingSplit
#define ImGuiDragDropFlags_SourceAutoExpirePayload ImGuiDragDropFlags_PayloadAutoExpire

#define ImGuiKey_COUNT ImGuiKey_NamedKey_END
#define ImGuiMod_Shortcut ImGuiMod_Ctrl

#define ImGuiMultiSelectFlags_SelectOnClick ImGuiMultiSelectFlags_SelectOnAuto
#define ImGuiSelectableFlags_DontClosePopups ImGuiSelectableFlags_NoAutoClosePopups

#define ImGuiTabBarFlags_FittingPolicyResizeDown ImGuiTabBarFlags_FittingPolicyShrink
#define ImGuiTreeNodeFlags_NavLeftJumpsBackHere ImGuiTreeNodeFlags_NavLeftJumpsToParent
#define ImGuiTreeNodeFlags_SpanTextWidth ImGuiTreeNodeFlags_SpanLabelWidth
#endif // IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#include <generated/sol_ImGui_Enums_gen.cpp>

#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#undef ImGuiTreeNodeFlags_SpanTextWidth
#undef ImGuiTreeNodeFlags_NavLeftJumpsBackHere
#undef ImGuiTabBarFlags_FittingPolicyResizeDown

#undef ImGuiSelectableFlags_DontClosePopups
#undef ImGuiMultiSelectFlags_SelectOnClick

#undef ImGuiMod_Shortcut
#undef ImGuiKey_COUNT

#undef ImGuiDragDropFlags_SourceAutoExpirePayload
#undef ImGuiDockNodeFlags_NoSplit
#undef ImGuiDockNodeFlags_NoDockingInCentralNode

#undef ImGuiConfigFlags_DpiEnableScaleViewports
#undef ImGuiConfigFlags_DpiEnableScaleFonts
#undef ImGuiConfigFlags_NavNoCaptureKeyboard
#undef ImGuiConfigFlags_NavEnableSetMousePos

#undef ImGuiColorEditFlags_AlphaPreview
#undef ImGuiCol_TabUnfocusedActive
#undef ImGuiCol_TabUnfocused
#undef ImGuiCol_TabActive
#undef ImGuiCol_NavHighlight

namespace ImGui
{
namespace
{
inline void PopButtonRepeat() { PopItemFlag(); }
} // namespace
} // namespace ImGui
#endif // IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#include <generated/sol_ImGui_gen_0.cpp>

#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui_internal.h>

namespace ImGui
{
namespace
{
[[nodiscard]] ImVec2 GetContentRegionMax() { return GetContentRegionAvail() + GetCursorScreenPos() - GetWindowPos(); }

[[nodiscard]] ImVec2 GetWindowContentRegionMax()
{
    const auto window = GImGui->CurrentWindow;
    return window->ContentRegionRect.Max - window->Pos;
}

[[nodiscard]] ImVec2 GetWindowContentRegionMin()
{
    const auto window = GImGui->CurrentWindow;
    return window->ContentRegionRect.Min - window->Pos;
}

void SetWindowFontScale(float scale)
{
    IM_ASSERT(scale > 0.0f);

    GetCurrentWindow()->FontWindowScale = scale;
    UpdateCurrentFontSize(0.0f);
}
} // namespace
} // namespace ImGui
#endif // IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#include <generated/sol_ImGui_gen_1.cpp>

#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
namespace ImGui
{
namespace
{
inline void PopTabStop() { PopItemFlag(); }
inline void PushButtonRepeat(bool repeat) { PushItemFlag(ImGuiItemFlags_ButtonRepeat, repeat); }
inline void PushTabStop(bool tab_stop) { PushItemFlag(ImGuiItemFlags_NoTabStop, !tab_stop); }
} // namespace
} // namespace ImGui
#endif // IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#include <generated/sol_ImGui_gen_2.cpp>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wsign-conversion");

#include <manual/sol_ImGui_Manual.cpp>

XR_DIAG_POP();

#include <sol_ImGui.cpp>
#endif // XR_IMGUI_LUA
