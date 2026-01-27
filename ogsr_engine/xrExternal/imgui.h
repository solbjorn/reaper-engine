#ifndef __XREXTERNAL_IMGUI_H
#define __XREXTERNAL_IMGUI_H

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#define IMGUI_ENABLE_FREETYPE
#define IMGUI_ENABLE_FREETYPE_PLUTOSVG
#define IMGUI_ENABLE_STB_TRUETYPE
#define IMGUI_USE_WCHAR32

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>

#ifndef IMGUI_DISABLE
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImGuizmo.h>

XR_DIAG_POP();
#endif // !IMGUI_DISABLE

#ifdef IMGUI_DISABLE
#undef XR_IMGUI_LUA
#endif

#include "../xrExternal/sol.h"

#ifdef XR_IMGUI_LUA
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wimplicit-float-conversion");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <sol_ImGui.h>

XR_DIAG_POP();

#undef ImMin
#else // !XR_IMGUI_LUA
namespace sol_ImGui
{
inline void Init(sol::state&) {}
} // namespace sol_ImGui
#endif // !XR_IMGUI_LUA

#endif // !__XREXTERNAL_IMGUI_H
