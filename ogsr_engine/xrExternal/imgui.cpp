#include "pch.h"

#include "imgui.h"

// ImGui

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wnrvo");

#include <imgui.cpp>

XR_DIAG_POP();

#include <imgui_demo.cpp>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi-stmt");

#include <imgui_draw.cpp>

XR_DIAG_POP();

#include <imgui_tables.cpp>
#include <imgui_widgets.cpp>

// Rendering backend

#include <backends/imgui_impl_dx11.cpp>

// ImGuizmo

#ifndef IMGUI_DISABLE
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wignored-qualifiers");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-compare");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wunused-variable");

#include <GraphEditor.cpp>

XR_DIAG_POP();

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImCurveEdit.cpp>

XR_DIAG_POP();

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wsign-compare");

#include <ImGradient.cpp>

XR_DIAG_POP();

#define alloca _alloca

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wcast-qual");
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wfloat-conversion");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wformat-nonliteral");
XR_DIAG_IGNORE("-Wmissing-prototypes");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImGuizmo.cpp>

XR_DIAG_POP();

#undef alloca

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wformat-nonliteral");
XR_DIAG_IGNORE("-Wimplicit-int-float-conversion");
XR_DIAG_IGNORE("-Wold-style-cast");
XR_DIAG_IGNORE("-Wsign-conversion");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImSequencer.cpp>

XR_DIAG_POP();
#endif // !IMGUI_DISABLE
