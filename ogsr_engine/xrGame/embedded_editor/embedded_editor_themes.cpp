#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_themes.h"

#include "embedded_editor_helper.h"

namespace xr
{
namespace detail
{
namespace
{
struct theme
{
    gsl::czstring name;
    gsl::czstring author;

    f32 Alpha;
    f32 DisabledAlpha;
    ImVec2 WindowPadding;
    f32 WindowRounding;
    f32 WindowBorderSize;
    ImVec2 WindowMinSize;
    ImVec2 WindowTitleAlign;
    ImGuiDir WindowMenuButtonPosition;
    f32 ChildRounding;
    f32 ChildBorderSize;
    f32 PopupRounding;
    f32 PopupBorderSize;
    ImVec2 FramePadding;
    f32 FrameRounding;
    f32 FrameBorderSize;
    ImVec2 ItemSpacing;
    ImVec2 ItemInnerSpacing;
    ImVec2 CellPadding;
    f32 IndentSpacing;
    f32 ColumnsMinSpacing;
    f32 ScrollbarSize;
    f32 ScrollbarRounding;
    f32 GrabMinSize;
    f32 GrabRounding;
    f32 TabRounding;
    f32 TabBorderSize;
    f32 TabCloseButtonMinWidthUnselected;
    ImGuiDir ColorButtonPosition;
    ImVec2 ButtonTextAlign;
    ImVec2 SelectableTextAlign;

    std::array<std::pair<ImGuiCol_, ImVec4>, 53> Colors;

    void apply(ImGuiStyle& style = ImGui::GetStyle()) const;
};

void theme::apply(ImGuiStyle& style) const
{
    style.Alpha = Alpha;
    style.DisabledAlpha = DisabledAlpha;
    style.WindowPadding = WindowPadding;
    style.WindowRounding = WindowRounding;
    style.WindowBorderSize = WindowBorderSize;
    style.WindowMinSize = WindowMinSize;
    style.WindowTitleAlign = WindowTitleAlign;
    style.WindowMenuButtonPosition = WindowMenuButtonPosition;
    style.ChildRounding = ChildRounding;
    style.ChildBorderSize = ChildBorderSize;
    style.PopupRounding = PopupRounding;
    style.PopupBorderSize = PopupBorderSize;
    style.FramePadding = FramePadding;
    style.FrameRounding = FrameRounding;
    style.FrameBorderSize = FrameBorderSize;
    style.ItemSpacing = ItemSpacing;
    style.ItemInnerSpacing = ItemInnerSpacing;
    style.CellPadding = CellPadding;
    style.IndentSpacing = IndentSpacing;
    style.ColumnsMinSpacing = ColumnsMinSpacing;
    style.ScrollbarSize = ScrollbarSize;
    style.ScrollbarRounding = ScrollbarRounding;
    style.GrabMinSize = GrabMinSize;
    style.GrabRounding = GrabRounding;
    style.TabRounding = TabRounding;
    style.TabBorderSize = TabBorderSize;
    style.TabCloseButtonMinWidthUnselected = TabCloseButtonMinWidthUnselected;
    style.ColorButtonPosition = ColorButtonPosition;
    style.ButtonTextAlign = ButtonTextAlign;
    style.SelectableTextAlign = SelectableTextAlign;

    for (auto& color : Colors)
        style.Colors[color.first] = color.second;

    style.Colors[ImGuiCol_InputTextCursor] = style.Colors[ImGuiCol_Text];
    style.Colors[ImGuiCol_TabSelectedOverline] = style.Colors[ImGuiCol_HeaderActive];
    style.Colors[ImGuiCol_DockingPreview] = style.Colors[ImGuiCol_HeaderActive] * ImVec4{1.0f, 1.0f, 1.0f, 0.7f};
    style.Colors[ImGuiCol_TextLink] = style.Colors[ImGuiCol_HeaderActive];
    style.Colors[ImGuiCol_TreeLines] = style.Colors[ImGuiCol_Border];
}

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wbraced-scalar-init");

// 17 (3 stock + 14 custom) most interesting themes from ImThemes
// (https://github.com/Patitotective/ImThemes), adapted to the latest
// docking branch of ImGui.
constexpr std::array<theme, 17> themes{{
    {
        .name{"##BlackDevil"},
        .author{"Naeemullah1"},

        .Alpha{1.0f},
        .DisabledAlpha{0.5f},
        .WindowPadding{10.0f, 10.0f},
        .WindowRounding{5.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{20.0f, 20.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{5.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{5.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{5.0f, 5.0f},
        .FrameRounding{5.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 5.0f},
        .ItemInnerSpacing{5.0f, 5.0f},
        .CellPadding{5.0f, 5.0f},
        .IndentSpacing{10.0f},
        .ColumnsMinSpacing{5.0f},
        .ScrollbarSize{10.0f},
        .ScrollbarRounding{5.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{10.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.78431374f, 0.78431374f, 0.78431374f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.43529412f, 0.40784314f, 0.3137255f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.05882353f, 0.11764706f, 0.05882353f, 0.39215687f}},
            {ImGuiCol_ChildBg, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.78431374f}},
            {ImGuiCol_Border, ImVec4{0.39215687f, 0.0f, 0.0f, 0.78431374f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.11764706f, 0.23529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.3529412f, 0.3529412f, 0.11764706f, 0.78431374f}},
            {ImGuiCol_TitleBg, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.19607843f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.78431374f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.11764706f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.11764706f, 0.3529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.11764706f, 0.5882353f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_CheckMark, ImVec4{0.11764706f, 0.5882353f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_SliderGrab, ImVec4{0.11764706f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.11764706f, 0.5882353f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_Button, ImVec4{0.3529412f, 0.3529412f, 0.11764706f, 0.78431374f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.3529412f, 0.47058824f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_ButtonActive, ImVec4{0.5882353f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_Header, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.11764706f, 0.3529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_HeaderActive, ImVec4{0.11764706f, 0.5882353f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_Separator, ImVec4{0.3529412f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.11764706f, 0.3529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.5882353f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.5882353f, 0.3529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.5882353f, 0.23529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_Tab, ImVec4{0.3529412f, 0.3529412f, 0.11764706f, 0.78431374f}},
            {ImGuiCol_TabHovered, ImVec4{0.3529412f, 0.47058824f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_TabSelected, ImVec4{0.5882353f, 0.3529412f, 0.23529412f, 0.78431374f}},
            {ImGuiCol_TabDimmed, ImVec4{0.05882353f, 0.11764706f, 0.15686275f, 0.78431374f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.5882353f, 0.3529412f, 0.3529412f, 0.78431374f}},
            {ImGuiCol_PlotLines, ImVec4{0.39215687f, 0.78431374f, 0.39215687f, 0.78431374f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.42745098f, 0.34901962f, 0.78431374f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.0f, 0.3529412f, 0.39215687f, 0.78431374f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.19607843f, 0.5882353f, 0.5882353f, 0.78431374f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 0.78431374f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 0.78431374f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 0.78431374f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.78431374f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.39215687f, 0.3529412f, 0.39215687f, 0.39215687f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Classic"},
        .author{"ocornut"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{4.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.8980392f, 0.8980392f, 0.8980392f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.6f, 0.6f, 0.6f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.85f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.10980392f, 0.10980392f, 0.13725491f, 0.92f}},
            {ImGuiCol_Border, ImVec4{0.49803922f, 0.49803922f, 0.49803922f, 0.5f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.42745098f, 0.42745098f, 0.42745098f, 0.39f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.46666667f, 0.46666667f, 0.6862745f, 0.4f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.41960785f, 0.40784314f, 0.6392157f, 0.69f}},
            {ImGuiCol_TitleBg, ImVec4{0.26666668f, 0.26666668f, 0.5372549f, 0.83f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.31764707f, 0.31764707f, 0.627451f, 0.87f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.4f, 0.4f, 0.8f, 0.2f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.4f, 0.4f, 0.54901963f, 0.8f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.2f, 0.24705882f, 0.29803923f, 0.6f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.4f, 0.4f, 0.8f, 0.3f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.4f, 0.4f, 0.8f, 0.4f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.40784314f, 0.3882353f, 0.8f, 0.6f}},
            {ImGuiCol_CheckMark, ImVec4{0.8980392f, 0.8980392f, 0.8980392f, 0.5f}},
            {ImGuiCol_SliderGrab, ImVec4{1.0f, 1.0f, 1.0f, 0.3f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.40784314f, 0.3882353f, 0.8f, 0.6f}},
            {ImGuiCol_Button, ImVec4{0.34901962f, 0.4f, 0.60784316f, 0.62f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.4f, 0.47843137f, 0.70980394f, 0.79f}},
            {ImGuiCol_ButtonActive, ImVec4{0.45882353f, 0.5372549f, 0.8f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.4f, 0.4f, 0.8980392f, 0.45f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.44705883f, 0.44705883f, 0.8980392f, 0.8f}},
            {ImGuiCol_HeaderActive, ImVec4{0.5294118f, 0.5294118f, 0.8666667f, 0.8f}},
            {ImGuiCol_Separator, ImVec4{0.49803922f, 0.49803922f, 0.49803922f, 0.6f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.6f, 0.6f, 0.69803923f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.69803923f, 0.69803923f, 0.8980392f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{1.0f, 1.0f, 1.0f, 0.1f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.7764706f, 0.81960785f, 1.0f, 0.6f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.7764706f, 0.81960785f, 1.0f, 0.9f}},
            {ImGuiCol_Tab, ImVec4{0.33333334f, 0.33333334f, 0.68235296f, 0.786f}},
            {ImGuiCol_TabHovered, ImVec4{0.44705883f, 0.44705883f, 0.8980392f, 0.8f}},
            {ImGuiCol_TabSelected, ImVec4{0.40392157f, 0.40392157f, 0.7254902f, 0.842f}},
            {ImGuiCol_TabDimmed, ImVec4{0.28235295f, 0.28235295f, 0.5686275f, 0.8212f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.34901962f, 0.34901962f, 0.6509804f, 0.8372f}},
            {ImGuiCol_PlotLines, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.8980392f, 0.69803923f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.8980392f, 0.69803923f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.6f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.26666668f, 0.26666668f, 0.3764706f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.44705883f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.25882354f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.07f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.0f, 0.0f, 1.0f, 0.35f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.44705883f, 0.44705883f, 0.8980392f, 0.8f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.2f, 0.2f, 0.2f, 0.35f}},
        }},
    },
    {
        .name{"##Classic Steam"},
        .author{"metasprite"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{1.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{0.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{0.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.49803922f, 0.49803922f, 0.49803922f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.28627452f, 0.3372549f, 0.25882354f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.28627452f, 0.3372549f, 0.25882354f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.23921569f, 0.26666668f, 0.2f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 0.5f}},
            {ImGuiCol_BorderShadow, ImVec4{0.13725491f, 0.15686275f, 0.10980392f, 0.52f}},
            {ImGuiCol_FrameBg, ImVec4{0.23921569f, 0.26666668f, 0.2f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.26666668f, 0.29803923f, 0.22745098f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.29803923f, 0.3372549f, 0.25882354f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.23921569f, 0.26666668f, 0.2f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.28627452f, 0.3372549f, 0.25882354f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.51f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.23921569f, 0.26666668f, 0.2f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.2784314f, 0.31764707f, 0.23921569f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.24705882f, 0.29803923f, 0.21960784f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.22745098f, 0.26666668f, 0.20784314f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 0.5f}},
            {ImGuiCol_Button, ImVec4{0.28627452f, 0.3372549f, 0.25882354f, 0.4f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 0.5f}},
            {ImGuiCol_Header, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 0.6f}},
            {ImGuiCol_HeaderActive, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 0.5f}},
            {ImGuiCol_Separator, ImVec4{0.13725491f, 0.15686275f, 0.10980392f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.1882353f, 0.22745098f, 0.1764706f, 0.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.5372549f, 0.5686275f, 0.50980395f, 0.78f}},
            {ImGuiCol_TabSelected, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.23921569f, 0.26666668f, 0.2f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.34901962f, 0.41960785f, 0.30980393f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{1.0f, 0.7764706f, 0.2784314f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.6f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.7294118f, 0.6666667f, 0.23921569f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.5882353f, 0.5372549f, 0.1764706f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Clean Dark/Red"},
        .author{"ImBritish"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{1.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{0.0f},
        .TabBorderSize{1.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.7294118f, 0.7490196f, 0.7372549f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.08627451f, 0.08627451f, 0.08627451f, 0.94f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.94f}},
            {ImGuiCol_Border, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 0.54f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.1764706f, 0.1764706f, 0.1764706f, 0.4f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 0.67f}},
            {ImGuiCol_TitleBg, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 0.65236056f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 0.67f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.019607844f, 0.019607844f, 0.019607844f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.30980393f, 0.30980393f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.40784314f, 0.40784314f, 0.40784314f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{1.0f, 0.38039216f, 0.38039216f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.5411765f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.1764706f, 0.1764706f, 0.1764706f, 0.4f}},
            {ImGuiCol_ButtonActive, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 0.67058825f}},
            {ImGuiCol_Header, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.27058825f, 0.27058825f, 0.27058825f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.3529412f, 0.3529412f, 0.3529412f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{1.0f, 0.32941177f, 0.32941177f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{1.0f, 0.4862745f, 0.4862745f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{1.0f, 0.4862745f, 0.4862745f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.21960784f, 0.21960784f, 0.21960784f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.2901961f, 0.2901961f, 0.2901961f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.1764706f, 0.1764706f, 0.1764706f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.14901961f, 0.06666667f, 0.06666667f, 0.97f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.40392157f, 0.15294118f, 0.15294118f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.8980392f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.3647059f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.2627451f, 0.63529414f, 0.8784314f, 0.43776822f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.46666667f, 0.18431373f, 0.18431373f, 0.9656652f}},
            {ImGuiCol_NavCursor, ImVec4{0.40784314f, 0.40784314f, 0.40784314f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Comfortable Dark Cyan"},
        .author{"SouthCraftX"},

        .Alpha{1.0f},
        .DisabledAlpha{1.0f},
        .WindowPadding{20.0f, 20.0f},
        .WindowRounding{11.5f},
        .WindowBorderSize{0.0f},
        .WindowMinSize{20.0f, 20.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_None},
        .ChildRounding{20.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{17.4f},
        .PopupBorderSize{1.0f},
        .FramePadding{20.0f, 3.4f},
        .FrameRounding{11.9f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.9f, 13.4f},
        .ItemInnerSpacing{7.1f, 1.8f},
        .CellPadding{12.1f, 9.2f},
        .IndentSpacing{0.0f},
        .ColumnsMinSpacing{8.7f},
        .ScrollbarSize{11.6f},
        .ScrollbarRounding{15.9f},
        .GrabMinSize{3.7f},
        .GrabRounding{20.0f},
        .TabRounding{9.8f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.27450982f, 0.31764707f, 0.4509804f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.09411765f, 0.101960786f, 0.11764706f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.11372549f, 0.1254902f, 0.15294118f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.03137255f, 0.9490196f, 0.84313726f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.03137255f, 0.9490196f, 0.84313726f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.6f, 0.9647059f, 0.03137255f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.18039216f, 0.1882353f, 0.19607843f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.15294118f, 0.15294118f, 0.15294118f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.14117648f, 0.16470589f, 0.20784314f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.105882354f, 0.105882354f, 0.105882354f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.12941177f, 0.14901961f, 0.19215687f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.14509805f, 0.14509805f, 0.14509805f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.03137255f, 0.9490196f, 0.84313726f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.1254902f, 0.27450982f, 0.57254905f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.52156866f, 0.6f, 0.7019608f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.039215688f, 0.98039216f, 0.98039216f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.03137255f, 0.9490196f, 0.84313726f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.9372549f, 0.9372549f, 0.9372549f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.26666668f, 0.2901961f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
        }},
    },
    {
        .name{"##Dark"},
        .author{"dougbinks"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{4.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.49803922f, 0.49803922f, 0.49803922f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.05882353f, 0.05882353f, 0.05882353f, 0.94f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.94f}},
            {ImGuiCol_Border, ImVec4{0.42745098f, 0.42745098f, 0.49803922f, 0.5f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.15686275f, 0.28627452f, 0.47843137f, 0.54f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.4f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.67f}},
            {ImGuiCol_TitleBg, ImVec4{0.039215688f, 0.039215688f, 0.039215688f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.15686275f, 0.28627452f, 0.47843137f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.51f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.019607844f, 0.019607844f, 0.019607844f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.30980393f, 0.30980393f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.40784314f, 0.40784314f, 0.40784314f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.23921569f, 0.5176471f, 0.8784314f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.4f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.05882353f, 0.5294118f, 0.9764706f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.31f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.8f}},
            {ImGuiCol_HeaderActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.42745098f, 0.42745098f, 0.49803922f, 0.5f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.09803922f, 0.4f, 0.7490196f, 0.78f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.09803922f, 0.4f, 0.7490196f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.2f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.67f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.95f}},
            {ImGuiCol_Tab, ImVec4{0.1764706f, 0.34901962f, 0.5764706f, 0.862f}},
            {ImGuiCol_TabHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.8f}},
            {ImGuiCol_TabSelected, ImVec4{0.19607843f, 0.40784314f, 0.6784314f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.06666667f, 0.101960786f, 0.14509805f, 0.9724f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.13333334f, 0.25882354f, 0.42352942f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.42745098f, 0.34901962f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.8980392f, 0.69803923f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.6f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.35f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##DUCK RED nope! is DARK RED"},
        .author{"for40255"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{0.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{0.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.49803922f, 0.49803922f, 0.49803922f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.039215688f, 0.039215688f, 0.039215688f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.05490196f, 0.05490196f, 0.05490196f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_Border, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.11764706f, 0.11764706f, 0.11764706f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{1.0f, 0.0f, 0.0f, 0.5647059f}},
            {ImGuiCol_FrameBgActive, ImVec4{1.0f, 0.0f, 0.0f, 0.5647059f}},
            {ImGuiCol_TitleBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.039215688f, 0.039215688f, 0.039215688f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.9411765f}},
            {ImGuiCol_ScrollbarBg, ImVec4{1.0f, 0.0f, 0.0f, 0.5647059f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{1.0f, 0.0f, 0.0f, 0.5019608f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{1.0f, 0.0f, 0.0f, 0.81545067f}},
            {ImGuiCol_SliderGrabActive, ImVec4{1.0f, 0.0f, 0.0f, 0.8156863f}},
            {ImGuiCol_Button, ImVec4{1.0f, 0.0f, 0.0f, 0.5019608f}},
            {ImGuiCol_ButtonHovered, ImVec4{1.0f, 0.0f, 0.0f, 0.74509805f}},
            {ImGuiCol_ButtonActive, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_Header, ImVec4{1.0f, 0.0f, 0.0f, 0.65665233f}},
            {ImGuiCol_HeaderHovered, ImVec4{1.0f, 0.0f, 0.0f, 0.8039216f}},
            {ImGuiCol_HeaderActive, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.5019608f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.6695279f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.95708156f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.101960786f, 0.11372549f, 0.12941177f, 0.2f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.20392157f, 0.20784314f, 0.21568628f, 0.2f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 0.2f}},
            {ImGuiCol_Tab, ImVec4{1.0f, 0.0f, 0.0f, 0.4392157f}},
            {ImGuiCol_TabHovered, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.06666667f, 0.06666667f, 0.06666667f, 0.972549f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.06666667f, 0.06666667f, 0.06666667f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.9490196f, 0.34509805f, 0.34509805f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.9490196f, 0.34509805f, 0.34509805f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.42745098f, 0.36078432f, 0.36078432f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{1.0f, 0.0f, 0.0f, 0.71244633f}},
            {ImGuiCol_TableBorderStrong, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.19607843f, 0.19607843f, 0.19607843f, 0.627451f}},
            {ImGuiCol_TextSelectedBg, ImVec4{1.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.25882354f, 0.27058825f, 0.38039216f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.18039216f, 0.22745098f, 0.2784314f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Everforest"},
        .author{"DestroyerDarkNess"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{6.0f, 3.0f},
        .WindowRounding{6.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{5.0f, 1.0f},
        .FrameRounding{3.0f},
        .FrameBorderSize{1.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{13.0f},
        .ScrollbarRounding{16.0f},
        .GrabMinSize{20.0f},
        .GrabRounding{2.0f},
        .TabRounding{4.0f},
        .TabBorderSize{1.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.8745098f, 0.87058824f, 0.8392157f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.58431375f, 0.57254905f, 0.52156866f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.59607846f, 0.5921569f, 0.101960786f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.59607846f, 0.5921569f, 0.101960786f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.4f, 0.36078432f, 0.32941177f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.4862745f, 0.43529412f, 0.39215687f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 0.972549f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.3137255f, 0.28627452f, 0.27058825f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.8392157f, 0.7490196f, 0.4f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.7411765f, 0.7176471f, 0.41960785f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.8392157f, 0.7490196f, 0.4f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.8392157f, 0.7490196f, 0.4f, 0.60944206f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.8392157f, 0.7490196f, 0.4f, 0.43137255f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.8392157f, 0.7490196f, 0.4f, 0.9019608f}},
            {ImGuiCol_NavCursor, ImVec4{0.23529412f, 0.21960784f, 0.21176471f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Excellency"},
        .author{"gonzaloivan121"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{10.0f, 10.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_None},
        .ChildRounding{6.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{6.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{8.0f, 6.0f},
        .FrameRounding{6.0f},
        .FrameBorderSize{1.0f},
        .ItemSpacing{6.0f, 6.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{11.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{6.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{6.0f},
        .TabRounding{6.0f},
        .TabBorderSize{1.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.5019608f, 0.5019608f, 0.5019608f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.08235294f, 0.08235294f, 0.08235294f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.15686275f, 0.15686275f, 0.15686275f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.19607843f, 0.19607843f, 0.19607843f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.101960786f, 0.101960786f, 0.101960786f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.05882353f, 0.05882353f, 0.05882353f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.09019608f, 0.09019608f, 0.09019608f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.05882353f, 0.05882353f, 0.05882353f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.08235294f, 0.08235294f, 0.08235294f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.08235294f, 0.08235294f, 0.08235294f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.15294118f, 0.15294118f, 0.15294118f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.019607844f, 0.019607844f, 0.019607844f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.30980393f, 0.30980393f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.4117647f, 0.4117647f, 0.4117647f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.7529412f, 0.7529412f, 0.7529412f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 0.7f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.65882355f, 0.65882355f, 0.65882355f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.21960784f, 0.21960784f, 0.21960784f, 0.784f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.27450982f, 0.27450982f, 0.27450982f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.21960784f, 0.21960784f, 0.21960784f, 0.588f}},
            {ImGuiCol_Header, ImVec4{0.18431373f, 0.18431373f, 0.18431373f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.18431373f, 0.18431373f, 0.18431373f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.18431373f, 0.18431373f, 0.18431373f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.101960786f, 0.101960786f, 0.101960786f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.15294118f, 0.7254902f, 0.9490196f, 0.588f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.15294118f, 0.7254902f, 0.9490196f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.9098039f, 0.9098039f, 0.9098039f, 0.25f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.8117647f, 0.8117647f, 0.8117647f, 0.67f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.45882353f, 0.45882353f, 0.45882353f, 0.95f}},
            {ImGuiCol_Tab, ImVec4{0.08235294f, 0.08235294f, 0.08235294f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{1.0f, 0.88235295f, 0.5294118f, 0.118f}},
            {ImGuiCol_TabSelected, ImVec4{1.0f, 0.88235295f, 0.5294118f, 0.235f}},
            {ImGuiCol_TabDimmed, ImVec4{0.08235294f, 0.08235294f, 0.08235294f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{1.0f, 0.88235295f, 0.5294118f, 0.118f}},
            {ImGuiCol_PlotLines, ImVec4{0.6117647f, 0.6117647f, 0.6117647f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.43137255f, 0.34901962f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.9019608f, 0.7019608f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.6f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.18431373f, 0.18431373f, 0.18431373f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.101960786f, 0.101960786f, 0.101960786f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.15294118f, 0.7254902f, 0.9490196f, 0.35f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.15294118f, 0.7254902f, 0.9490196f, 0.8f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Future Dark"},
        .author{"rewrking"},

        .Alpha{1.0f},
        .DisabledAlpha{1.0f},
        .WindowPadding{12.0f, 12.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{0.0f},
        .WindowMinSize{20.0f, 20.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_None},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{6.0f, 6.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{12.0f, 6.0f},
        .ItemInnerSpacing{6.0f, 3.0f},
        .CellPadding{12.0f, 6.0f},
        .IndentSpacing{20.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{12.0f},
        .ScrollbarRounding{0.0f},
        .GrabMinSize{12.0f},
        .GrabRounding{0.0f},
        .TabRounding{0.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.27450982f, 0.31764707f, 0.4509804f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.23529412f, 0.21568628f, 0.59607846f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.5372549f, 0.5529412f, 1.0f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.23529412f, 0.21568628f, 0.59607846f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.23529412f, 0.21568628f, 0.59607846f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.23529412f, 0.21568628f, 0.59607846f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.52156866f, 0.6f, 0.7019608f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.039215688f, 0.98039216f, 0.98039216f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{1.0f, 0.2901961f, 0.59607846f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.99607843f, 0.4745098f, 0.69803923f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.23529412f, 0.21568628f, 0.59607846f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
        }},
    },
    {
        .name{"##Gold"},
        .author{"CookiePLMonster"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{4.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{1.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Right},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{4.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 2.0f},
        .FrameRounding{4.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{10.0f, 2.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{12.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{10.0f},
        .ScrollbarRounding{6.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{4.0f},
        .TabRounding{4.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.91764706f, 0.91764706f, 0.91764706f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.4392157f, 0.4392157f, 0.4392157f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.05882353f, 0.05882353f, 0.05882353f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.94f}},
            {ImGuiCol_Border, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.10980392f, 0.10980392f, 0.10980392f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.51f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.10980392f, 0.10980392f, 0.10980392f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.05882353f, 0.05882353f, 0.05882353f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.20784314f, 0.20784314f, 0.20784314f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.46666667f, 0.46666667f, 0.46666667f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.80784315f, 0.827451f, 0.80784315f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.92941177f, 0.64705884f, 0.13725491f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.20784314f, 0.20784314f, 0.20784314f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.20784314f, 0.20784314f, 0.20784314f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.50980395f, 0.35686275f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.9098039f, 0.6392157f, 0.12941177f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.7764706f, 0.54901963f, 0.20784314f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.06666667f, 0.09803922f, 0.14901961f, 0.97f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.13725491f, 0.25882354f, 0.41960785f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.42745098f, 0.34901962f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.8980392f, 0.69803923f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.6f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.35f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Light"},
        .author{"dougbinks"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{0.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{0.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{14.0f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{0.0f},
        .TabRounding{4.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.6f, 0.6f, 0.6f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.9372549f, 0.9372549f, 0.9372549f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{1.0f, 1.0f, 1.0f, 0.98f}},
            {ImGuiCol_Border, ImVec4{0.0f, 0.0f, 0.0f, 0.3f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.4f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.67f}},
            {ImGuiCol_TitleBg, ImVec4{0.95686275f, 0.95686275f, 0.95686275f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.81960785f, 0.81960785f, 0.81960785f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{1.0f, 1.0f, 1.0f, 0.51f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.85882354f, 0.85882354f, 0.85882354f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.9764706f, 0.9764706f, 0.9764706f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.6862745f, 0.6862745f, 0.6862745f, 0.8f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.4862745f, 0.4862745f, 0.4862745f, 0.8f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.4862745f, 0.4862745f, 0.4862745f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.78f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.45882353f, 0.5372549f, 0.8f, 0.6f}},
            {ImGuiCol_Button, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.4f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.05882353f, 0.5294118f, 0.9764706f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.31f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.8f}},
            {ImGuiCol_HeaderActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.3882353f, 0.3882353f, 0.3882353f, 0.62f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.13725491f, 0.4392157f, 0.8f, 0.78f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.13725491f, 0.4392157f, 0.8f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.34901962f, 0.34901962f, 0.34901962f, 0.17f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.67f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.95f}},
            {ImGuiCol_Tab, ImVec4{0.7607843f, 0.79607844f, 0.8352941f, 0.931f}},
            {ImGuiCol_TabHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.8f}},
            {ImGuiCol_TabSelected, ImVec4{0.5921569f, 0.7254902f, 0.88235295f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.91764706f, 0.9254902f, 0.93333334f, 0.9862f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.7411765f, 0.81960785f, 0.9137255f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.3882353f, 0.3882353f, 0.3882353f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.42745098f, 0.34901962f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.8980392f, 0.69803923f, 0.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.44705883f, 0.0f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.7764706f, 0.8666667f, 0.9764706f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.5686275f, 0.5686275f, 0.6392157f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.6784314f, 0.6784314f, 0.7372549f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.29803923f, 0.29803923f, 0.29803923f, 0.09f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.35f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.95f}},
            {ImGuiCol_NavCursor, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.8f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{0.69803923f, 0.69803923f, 0.69803923f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.2f, 0.2f, 0.2f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.2f, 0.2f, 0.2f, 0.35f}},
        }},
    },
    {
        .name{"##Modern"},
        .author{"LousyBook-01"},

        .Alpha{1.0f},
        .DisabledAlpha{0.3f},
        .WindowPadding{10.1f, 10.1f},
        .WindowRounding{10.3f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{20.0f, 32.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Right},
        .ChildRounding{8.2f},
        .ChildBorderSize{1.0f},
        .PopupRounding{10.7f},
        .PopupBorderSize{1.0f},
        .FramePadding{20.0f, 1.5f},
        .FrameRounding{4.8f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{9.7f, 5.3f},
        .ItemInnerSpacing{5.4f, 9.3f},
        .CellPadding{7.9f, 2.0f},
        .IndentSpacing{10.7f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{12.1f},
        .ScrollbarRounding{20.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{4.6f},
        .TabRounding{4.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{1.0f, 1.0f, 1.0f, 0.3991416f}},
            {ImGuiCol_WindowBg, ImVec4{0.039215688f, 0.039215688f, 0.039215688f, 0.94f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.050980393f, 0.050980393f, 0.050980393f, 0.94f}},
            {ImGuiCol_Border, ImVec4{0.42745098f, 0.42745098f, 0.49803922f, 0.5f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 0.42060083f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.14117648f, 0.14117648f, 0.14117648f, 0.4f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.23137255f, 0.23137255f, 0.23137255f, 0.86266094f}},
            {ImGuiCol_TitleBg, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.09411765f, 0.09411765f, 0.09411765f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.2918455f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.13725491f, 0.13725491f, 0.13725491f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.019607844f, 0.019607844f, 0.019607844f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.30980393f, 0.30980393f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.40784314f, 0.40784314f, 0.40784314f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.98039216f, 0.25882354f, 0.25882354f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.25882354f, 0.5882353f, 0.98039216f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.5793991f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.09803922f, 0.09803922f, 0.09803922f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{1.0f, 0.23137255f, 0.23137255f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.0f, 0.0f, 0.0f, 0.4549356f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.18039216f, 0.18039216f, 0.18039216f, 0.8f}},
            {ImGuiCol_HeaderActive, ImVec4{0.9764706f, 0.25882354f, 0.25882354f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.0f, 0.0f, 0.0f, 0.5f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.09803922f, 0.4f, 0.7490196f, 0.78f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.09803922f, 0.4f, 0.7490196f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.2f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.67f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 0.95f}},
            {ImGuiCol_Tab, ImVec4{0.105882354f, 0.105882354f, 0.105882354f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{1.0f, 0.3647059f, 0.6745098f, 0.8f}},
            {ImGuiCol_TabSelected, ImVec4{1.0f, 0.22352941f, 0.22352941f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.10980392f, 0.16862746f, 0.23921569f, 0.9724f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.13333334f, 0.25882354f, 0.42352942f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{1.0f, 0.42745098f, 0.34901962f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{1.0f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{1.0f, 0.21568628f, 0.69803923f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{1.0f, 0.23529412f, 0.23529412f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{1.0f, 0.31764707f, 0.31764707f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{1.0f, 0.5647059f, 0.5647059f, 0.36909872f}},
            {ImGuiCol_TableRowBg, ImVec4{0.7254902f, 0.3372549f, 1.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 0.27450982f, 0.27450982f, 0.111588f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.9764706f, 0.25882354f, 0.25882354f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.0f, 0.0f, 0.0f, 0.64377683f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.46781117f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.0f, 0.0f, 0.0f, 0.73390555f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.0f, 0.0f, 0.0f, 0.7982833f}},
        }},
    },
    {
        .name{"##Moonlight"},
        .author{"Madam-Herta"},

        .Alpha{1.0f},
        .DisabledAlpha{1.0f},
        .WindowPadding{12.0f, 12.0f},
        .WindowRounding{11.5f},
        .WindowBorderSize{0.0f},
        .WindowMinSize{20.0f, 20.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Right},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{20.0f, 3.4f},
        .FrameRounding{11.9f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{4.3f, 5.5f},
        .ItemInnerSpacing{7.1f, 1.8f},
        .CellPadding{12.1f, 9.2f},
        .IndentSpacing{0.0f},
        .ColumnsMinSpacing{4.9f},
        .ScrollbarSize{11.6f},
        .ScrollbarRounding{15.9f},
        .GrabMinSize{3.7f},
        .GrabRounding{20.0f},
        .TabRounding{0.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.27450982f, 0.31764707f, 0.4509804f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.09411765f, 0.101960786f, 0.11764706f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.11372549f, 0.1254902f, 0.15294118f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.15686275f, 0.16862746f, 0.19215687f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.972549f, 1.0f, 0.49803922f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.972549f, 1.0f, 0.49803922f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{1.0f, 0.79607844f, 0.49803922f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.18039216f, 0.1882353f, 0.19607843f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.15294118f, 0.15294118f, 0.15294118f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.14117648f, 0.16470589f, 0.20784314f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.105882354f, 0.105882354f, 0.105882354f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.12941177f, 0.14901961f, 0.19215687f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.15686275f, 0.18431373f, 0.2509804f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.14509805f, 0.14509805f, 0.14509805f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.972549f, 1.0f, 0.49803922f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.078431375f, 0.08627451f, 0.101960786f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.1254902f, 0.27450982f, 0.57254905f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.52156866f, 0.6f, 0.7019608f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.039215688f, 0.98039216f, 0.98039216f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.88235295f, 0.79607844f, 0.56078434f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.95686275f, 0.95686275f, 0.95686275f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.047058824f, 0.05490196f, 0.07058824f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.11764706f, 0.13333334f, 0.14901961f, 1.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.09803922f, 0.105882354f, 0.12156863f, 1.0f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.9372549f, 0.9372549f, 0.9372549f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.26666668f, 0.2901961f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{0.49803922f, 0.5137255f, 1.0f, 1.0f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f}},
        }},
    },
    {
        .name{"##Purple Comfy"},
        .author{"RegularLunar"},

        .Alpha{1.0f},
        .DisabledAlpha{0.1f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{10.0f},
        .WindowBorderSize{0.0f},
        .WindowMinSize{30.0f, 30.0f},
        .WindowTitleAlign{0.5f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Right},
        .ChildRounding{5.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{10.0f},
        .PopupBorderSize{0.0f},
        .FramePadding{5.0f, 3.5f},
        .FrameRounding{5.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{5.0f, 4.0f},
        .ItemInnerSpacing{5.0f, 5.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{5.0f},
        .ColumnsMinSpacing{5.0f},
        .ScrollbarSize{15.0f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{15.0f},
        .GrabRounding{5.0f},
        .TabRounding{5.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{1.0f, 1.0f, 1.0f, 0.360515f}},
            {ImGuiCol_WindowBg, ImVec4{0.09803922f, 0.09803922f, 0.09803922f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{1.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.09803922f, 0.09803922f, 0.09803922f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.15686275f, 0.15686275f, 0.15686275f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.38039216f, 0.42352942f, 0.57254905f, 0.54901963f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_TitleBg, ImVec4{0.09803922f, 0.09803922f, 0.09803922f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.09803922f, 0.09803922f, 0.09803922f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.25882354f, 0.25882354f, 0.25882354f, 0.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.15686275f, 0.15686275f, 0.15686275f, 0.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.15686275f, 0.15686275f, 0.15686275f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.23529412f, 0.23529412f, 0.23529412f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.29411766f, 0.29411766f, 0.29411766f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_SliderGrab, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_Button, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_ButtonActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_Header, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_HeaderActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_Separator, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_Tab, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_TabHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_TabSelected, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_TabDimmed, ImVec4{0.0f, 0.4509804f, 1.0f, 0.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.13333334f, 0.25882354f, 0.42352942f, 0.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.29411766f, 0.29411766f, 0.29411766f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.2901961f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.03433478f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.5019608f, 0.3019608f, 1.0f, 0.54901963f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.0f, 0.0f, 0.0f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##Windark"},
        .author{"DestroyerDarkNess"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{8.4f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Right},
        .ChildRounding{3.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{3.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{3.0f},
        .FrameBorderSize{1.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{21.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{5.6f},
        .ScrollbarRounding{18.0f},
        .GrabMinSize{10.0f},
        .GrabRounding{3.0f},
        .TabRounding{3.0f},
        .TabBorderSize{0.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.6f, 0.6f, 0.6f, 1.0f}},
            {ImGuiCol_WindowBg, ImVec4{0.1254902f, 0.1254902f, 0.1254902f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.1254902f, 0.1254902f, 0.1254902f, 1.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_Border, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.1254902f, 0.1254902f, 0.1254902f, 1.0f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.1254902f, 0.1254902f, 0.1254902f, 1.0f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.1254902f, 0.1254902f, 0.1254902f, 1.0f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.34901962f, 0.34901962f, 0.34901962f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.0f, 0.47058824f, 0.84313726f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.0f, 0.47058824f, 0.84313726f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{0.0f, 0.32941177f, 0.6f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.3019608f, 0.3019608f, 0.3019608f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.2509804f, 0.2509804f, 0.2509804f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.16862746f, 0.16862746f, 0.16862746f, 1.0f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.21568628f, 0.21568628f, 0.21568628f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.0f, 0.47058824f, 0.84313726f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.0f, 0.32941177f, 0.6f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.0f, 0.47058824f, 0.84313726f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.0f, 0.32941177f, 0.6f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.1882353f, 0.1882353f, 0.2f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.22745098f, 0.22745098f, 0.24705882f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{1.0f, 1.0f, 1.0f, 0.06f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.0f, 0.47058824f, 0.84313726f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{1.0f, 1.0f, 0.0f, 0.9f}},
            {ImGuiCol_NavCursor, ImVec4{0.25882354f, 0.5882353f, 0.9764706f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
    {
        .name{"##ayu-dark"},
        .author{"usrnatc"},

        .Alpha{1.0f},
        .DisabledAlpha{0.6f},
        .WindowPadding{8.0f, 8.0f},
        .WindowRounding{5.0f},
        .WindowBorderSize{1.0f},
        .WindowMinSize{32.0f, 32.0f},
        .WindowTitleAlign{0.0f, 0.5f},
        .WindowMenuButtonPosition{ImGuiDir_Left},
        .ChildRounding{0.0f},
        .ChildBorderSize{1.0f},
        .PopupRounding{0.0f},
        .PopupBorderSize{1.0f},
        .FramePadding{4.0f, 3.0f},
        .FrameRounding{5.0f},
        .FrameBorderSize{0.0f},
        .ItemSpacing{8.0f, 4.0f},
        .ItemInnerSpacing{4.0f, 4.0f},
        .CellPadding{4.0f, 2.0f},
        .IndentSpacing{20.0f},
        .ColumnsMinSpacing{6.0f},
        .ScrollbarSize{12.9f},
        .ScrollbarRounding{9.0f},
        .GrabMinSize{8.0f},
        .GrabRounding{5.0f},
        .TabRounding{4.0f},
        .TabBorderSize{1.0f},
        .TabCloseButtonMinWidthUnselected{0.0f},
        .ColorButtonPosition{ImGuiDir_Right},
        .ButtonTextAlign{0.5f, 0.5f},
        .SelectableTextAlign{0.0f, 0.0f},

        .Colors{{
            {ImGuiCol_Text, ImVec4{0.9019608f, 0.7058824f, 0.3137255f, 1.0f}},
            {ImGuiCol_TextDisabled, ImVec4{0.9019608f, 0.7058824f, 0.3137255f, 0.5019608f}},
            {ImGuiCol_WindowBg, ImVec4{0.039215688f, 0.05490196f, 0.078431375f, 1.0f}},
            {ImGuiCol_ChildBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_PopupBg, ImVec4{0.078431375f, 0.078431375f, 0.078431375f, 0.94f}},
            {ImGuiCol_Border, ImVec4{0.42745098f, 0.42745098f, 0.49803922f, 0.5f}},
            {ImGuiCol_BorderShadow, ImVec4{0.0f, 0.0f, 0.0f, 0.0f}},
            {ImGuiCol_FrameBg, ImVec4{0.07450981f, 0.09019608f, 0.12941177f, 1.0f}},
            {ImGuiCol_FrameBgHovered, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_FrameBgActive, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TitleBg, ImVec4{0.0f, 0.0f, 0.0f, 0.5019608f}},
            {ImGuiCol_TitleBgActive, ImVec4{0.05882353f, 0.07450981f, 0.101960786f, 1.0f}},
            {ImGuiCol_TitleBgCollapsed, ImVec4{0.0f, 0.0f, 0.0f, 0.5019608f}},
            {ImGuiCol_MenuBarBg, ImVec4{0.043137256f, 0.05490196f, 0.078431375f, 1.0f}},
            {ImGuiCol_ScrollbarBg, ImVec4{0.019607844f, 0.019607844f, 0.019607844f, 0.53f}},
            {ImGuiCol_ScrollbarGrab, ImVec4{0.30980393f, 0.30980393f, 0.30980393f, 1.0f}},
            {ImGuiCol_ScrollbarGrabHovered, ImVec4{0.40784314f, 0.40784314f, 0.40784314f, 1.0f}},
            {ImGuiCol_ScrollbarGrabActive, ImVec4{0.50980395f, 0.50980395f, 0.50980395f, 1.0f}},
            {ImGuiCol_CheckMark, ImVec4{0.24705882f, 0.69803923f, 1.0f, 1.0f}},
            {ImGuiCol_SliderGrab, ImVec4{0.9019608f, 0.7058824f, 0.3137255f, 1.0f}},
            {ImGuiCol_SliderGrabActive, ImVec4{1.0f, 0.56078434f, 0.2509804f, 1.0f}},
            {ImGuiCol_Button, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_ButtonHovered, ImVec4{0.30980393f, 0.31764707f, 0.3372549f, 1.0f}},
            {ImGuiCol_ButtonActive, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_Header, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_HeaderHovered, ImVec4{0.30980393f, 0.31764707f, 0.3372549f, 1.0f}},
            {ImGuiCol_HeaderActive, ImVec4{0.30980393f, 0.31764707f, 0.3372549f, 1.0f}},
            {ImGuiCol_Separator, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_SeparatorHovered, ImVec4{0.30980393f, 0.31764707f, 0.3372549f, 1.0f}},
            {ImGuiCol_SeparatorActive, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_ResizeGrip, ImVec4{0.24705882f, 0.69803923f, 1.0f, 1.0f}},
            {ImGuiCol_ResizeGripHovered, ImVec4{0.13333334f, 0.4117647f, 0.54901963f, 1.0f}},
            {ImGuiCol_ResizeGripActive, ImVec4{0.13333334f, 0.4117647f, 0.54901963f, 1.0f}},
            {ImGuiCol_Tab, ImVec4{0.07450981f, 0.09019608f, 0.12941177f, 1.0f}},
            {ImGuiCol_TabHovered, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TabSelected, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TabDimmed, ImVec4{0.06666667f, 0.101960786f, 0.14509805f, 0.9724f}},
            {ImGuiCol_TabDimmedSelected, ImVec4{0.13333334f, 0.25882354f, 0.42352942f, 1.0f}},
            {ImGuiCol_PlotLines, ImVec4{0.60784316f, 0.60784316f, 0.60784316f, 1.0f}},
            {ImGuiCol_PlotLinesHovered, ImVec4{0.24705882f, 0.69803923f, 1.0f, 1.0f}},
            {ImGuiCol_PlotHistogram, ImVec4{0.24705882f, 0.69803923f, 1.0f, 1.0f}},
            {ImGuiCol_PlotHistogramHovered, ImVec4{0.13333334f, 0.4117647f, 0.54901963f, 1.0f}},
            {ImGuiCol_TableHeaderBg, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TableBorderStrong, ImVec4{0.2509804f, 0.25882354f, 0.2784314f, 1.0f}},
            {ImGuiCol_TableBorderLight, ImVec4{0.039215688f, 0.05490196f, 0.078431375f, 1.0f}},
            {ImGuiCol_TableRowBg, ImVec4{0.039215688f, 0.05490196f, 0.078431375f, 1.0f}},
            {ImGuiCol_TableRowBgAlt, ImVec4{0.06666667f, 0.10980392f, 0.16078432f, 1.0f}},
            {ImGuiCol_TextSelectedBg, ImVec4{0.30980393f, 0.30980393f, 0.34901962f, 1.0f}},
            {ImGuiCol_DragDropTarget, ImVec4{0.24705882f, 0.69803923f, 1.0f, 1.0f}},
            {ImGuiCol_NavCursor, ImVec4{0.9764706f, 0.25882354f, 0.25882354f, 1.0f}},
            {ImGuiCol_NavWindowingHighlight, ImVec4{1.0f, 1.0f, 1.0f, 0.7f}},
            {ImGuiCol_NavWindowingDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.2f}},
            {ImGuiCol_ModalWindowDimBg, ImVec4{0.8f, 0.8f, 0.8f, 0.35f}},
        }},
    },
}};

XR_DIAG_POP();

static_assert(std::is_eq(xr_strcmp(themes[4].name + 2, "Comfortable Dark Cyan")));
const theme* active = &themes[4];

void selectable(const ImGuiStyle& style, const theme& theme)
{
    if (ImGui::Selectable(theme.name, active == &theme, ImGuiSelectableFlags_AllowOverlap, ImVec2{0.0f, ImGui::GetFrameHeight() + 17.0f + style.FramePadding.y * 2.0f}))
        active = &theme;

    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::PushFont(nullptr, 17.0f);
    ImGui::TextUnformatted(theme.name + 2);
    ImGui::PopFont();

    ImGui::TextUnformatted(theme.author);
    ImGui::EndGroup();
}
} // namespace

void themes_picker_show(bool& show)
{
    const xr::detail::ImguiWnd wnd{"ImThemes", &show};
    if (wnd.Collapsed)
        return;

    auto& style = ImGui::GetStyle();
    auto last = active;

    for (auto& theme : themes)
        selectable(style, theme);

    if (active != last)
        active->apply(style);
}

namespace
{
// File: 'Terminuz14.ttf' (81308 bytes)
// Exported using binary_to_compressed_c.cpp
constexpr std::string_view Terminuz14_compressed_data_base85{
    "7])#######So85U'/###[),##.f1$#Q6>##%[n420tjXr0.>>#)jNC4aNV=BGQ`5#GLJM'%xmo%X`(*Hlme+M`cSV6<d5&5#-0%J/1jP(mSZ`*hW&]t<.d<BnB47Eqq.>-1@pV-TT$=("
    "%Y#n$-hG<-q*.m/<_[FHUrG%EU^>E6tD/2'n2JuB6<-/>',1,)[[qr$sJ[^IbDL/#r5S>-oU+41+>00Fq$1j**mDA+uOgk0P%S+HGE5i64)&#,+6)Z-g^A_1K/QlJbuciAeI1=#PpNw9"
    "h=+Vdktm#MXn,VdC`j7#_=&&$%C7#M_nv##UE31#N&###f4/>5FVO;->=Ke$Ckl'&(lT#$e#<X(X@(&l,@0:)g>t9)v`O;-eXZV$E2aA+2-cw'MUNF%sG[8#<J_/#*VUV$cvo*%FJ)4#"
    "51i$#eQj(N:.GL<VUHS7[I0AuJ;AwTS&8O+G4GCi^wUJ`GTK_&Zr&@TjEGqV0,ww'5eSF%U04>dCRB.$eh/F.CqC_&Is/kL7wB=Rd9^KNgkR`YZ:d9NbE$mPu&#GNYeHRWJQ7US):S48"
    "jJ:K*xxgt7EQ]F..WB_OP0R^OZ[*vNE11eSgRS780x).HF2_f/$fv>R4etX.TE]':MYLR8S>FW81IKL8%QYF%lWAvP/tne79#HppA`eJ8caQ&Q)coG&#KZ$$pPBA9A`6db#(-A%cXhDN"
    "GX9l'RLsT*.;nRNThbjTEO*_&>T_#C4vg0?:)PqTkwZ.V4B=@[fg@'8@'i=2ksd$#+cso9j6,?e2:(&(FK=<-DKv.)9%@<-w&##1TGiQSis^Q.t$t^P.r//1+hZ*W>KZu7(>[w'b=.l+"
    "s+b2-psbsQu0Sv&<WDD2aerm8+PBe=`%9:ef-bZ%)^u8ReWv.Pg['AUgFhTSc'>5*s/_'#?A3.$[?l6#)(MT.?ww%#-4ip.Qwrr$oFEX(Zl14+;10.$I8YY#0%,DN6&l4S?U^Y%^Q(v>"
    ",]UV$0u68%47no%8ONP&<h/2'@*gi'DBGJ(HZ(,)Ls_c)P5@D*TMw%+XfW]+gSY)N_-4&#c9F&#gEX&#kQk&#o^''#sj9'#wvK'#%-_'#)9q'#-E-(#1Q?(#5^Q(#9jd(#=vv(#A,3)#"
    "E8E)#IDW)#MPj)#Q]&*#Ui8*#YuJ*#^+^*#b7p*#fC,+#jO>+#n[P+#rhc+#vtu+#$+2,#(7D,#,CV,#0Oi,#4[%-#fT_pLJqanM21G`N@BWrQXx-JUhhEcVm9^%XoE#AXwv:YY#-VuY"
    ")QRrZ+^n7[1,k4]6A0P];iGi^@:`+`EO%G`^e35JMMWiBES4GDo##8I3HJ]FrbpxOW:PcDXB-DNdtS`WEOGVZ:wUfC4s]3FuOkA#Pd%T%Nk9PJZ_=]X^-p.LH%h%FctPP8qq[fUbvTlJ"
    "Pc#v#)S/GM#E6,)TWW]Fea48Ij^8;HSZa._+e8`a[t$##0VEJLr>Sw9M@ZY#o`Hg#mlQg#%;w0#15n0#9`W1#A.92#Ax&2#%@AvLb1`2#['Z3#]7/X#-Ri,#DIiG#?R^O0Qb7H#-:D,#"
    "AA8vL_dB(MFr%5%/NA>#nO3MKp]ZiK<**;QVxQfUNLp]+<fe]4*RXV?JIVYGx@Qc`t3Mci_ABv#;_/m&2<@AOW7acVBC5Gr(<PJU'9%vYEI.,`V0ZcruR4)4,aQf`'g^(t2C`D+Ns9v-"
    "e_bJ2&6s]53v4v65Ka`XjhvuZ54Q,*XSZ8/NG0&YF?OMhq/Qv$$di8&>hrD+[u%Q0g7U5B%:F^#EQ9Z-5X(6'>5r#$%bFdE-;Z&PLg%KVt0NvZ,6(Q^:/wJ`H4P&cU0e;e/c:?6Gu-98"
    "3U[mBAZ5HE,XvXu:v:'olD4;-;1&GVV<]Kl6*^KlJ&Pi^r;1)3WTS>?pYx/`r'2GiYvE##08VJMcg_m/[v@1$uU:;$]p@L$/pkM$^<KQ$wo%u,L95n,>_@p,]R5;-:N4>-w3q4.;$c(P"
    "eA7*N:@KVM#[[&M[a:)Ne]Ou-ccXjLTX+'.hFQl.E5@h.bQ2M/vg[p.F0.tL2@2)/RrQl.hY2L/?@q4.IubnMI^28$#'>uusc$4vG213v2W=2vZ4^.vlk*xu2WftufAAtuoV_nuL4Tiu"
    "##egu1Ircu^#*Du7[^Uu3OKUu43JPurY-Kuf5LJuK[.HunabDmZM[0mrCu.m]kNdlKFnclE.Icl/GCbl##cal4<L>lM@5Ul=RvJlr?0IlK[X=l^RG:lEPl2lkCY2laif1lJ2j0lrXw,l"
    "#dxdk1o:nkm%J[k')g5kVB=9k7cl6kq=56k_Dj4kN,E4kB^d3k:E?3k0-q2ks'=1kM&c&k*oP#k__5#kZ=m/$&*,##/L/(MBFHciYTpdmxiY/$D#%##6;=e#Mi?O$vGY##B$(,)k_Y)4"
    "#;GjL3TF:.oeY9C;a4I$FI,bu%kLEdOA&U:TAW`#wPd;#fKb&##$nO(NZ)F3PImO(NIHg)09tM(CG>8.e.Y)4Th^;.O@;=-7ENfLX%[2MhbpC-H2cc-vqHq;9SQ>d4pMBMPvBs.&,###"
    "pN`-QG3BJ1;As?B<,B+4R%XD#Y6J)4_i%RMxO%.MC;Z)4VE,RER[kD#8$JY-7b/I$]66GM>^sfL<VEYNE,sFrZ_f2CtEJEd^;nHH7L:C&Fr+^u?6NQd0P1bRVZA>#r+TV-WD:-m+m`wK"
    "t#bG3TcW;.1:<$gnv@+4_'M['Hpo6*Z$UfLqhAGM^X61MA#>sL,rAGMc2D9.-fIGMAe3>d;'3=(fXlGMYmS9CDfS&#Ei4p756rp/*Z>G2f?-IMRerA.4Djc)v(v<C[_G)4JWoG38<bJ-"
    "A@;=-7eh9MDD4jLt@)aO].k?NQYx7M/C'g-#Z1@'_LHQ(0.2t-FLi%MVX?>#U08G`jwdG;*L]'J2oTEn.X*W-`.[q2F$2'#+W@)$SdmeMvndLMXf*F3A1(gLh]CDNnX>W-4Yg6Epk-YM"
    "H<^;-gt<^-]p?e6f+mM(hg,qi/KCZ6xv//11U$gL0I(]$#vJ['kL,F7mdU3X`B(7#PhFJ(Q,k'8S:Z58a,>)4GU@)4OS7[^5sX?^EtE>dF/n;Si8I:M$GF3NtklgL9T8c$pbX;.PvxTM"
    "GL`-6iW4G`x]5>d`Je9`A2)O#62@8#f8iu5(>abMJkO?-g@dH.t16G`B*@v$.:#Z-?/aZ-(BK?dLSimLp;$##)s/kLchs>.Krgo.G7Rk+MH4PdQiIfLfD2G`%R,)*nf^G3tq-L5KL3g)"
    "SVSmCmrjrM5'8DCe*loLA8'@%H>uu#AYJ&#s_Vj(eqho.08m*4T#NF386fX-uq:X6L/'=:8iYB]3ctM(Z-jG3Wk2b$afv1';Q2Y$7PgIh-A'$$n@)-uJl=$^,hAp.eTA;?vAG_&12Qj("
    "d-LhLp-;hL>/H)4[#HZ6ZDsI37sG%.[k3qLgqj^-R%3R3$x9[Te(+kk=_EM0Aw;O4bo*CSTN`hL>:xiLa:s'N2RFjL/<x9.M6&tu2QlZpmr<Aux+KgL%KJEd9G@2CrMsPhX5fjLAPSF-"
    "?7Gc-Wcg9;V(m3FEJ1W-HlmW_dl.&4e/KC/8BF:.`^K]M+-J6M%ujnLSBiMN7Na+Mnit;.(=8DC#a*^#KTp0)B+$##shL%6@Q3Z6=&)P-jw#1/IMjD#4x)O=C23+@>1T9`FX`78+adpK"
    "l_2G`-E[Y,pIXw5qntM(P];Y$w_PZI0IXD#lErn3)lKHMx%6D-F##k-AveE[-q$1YIH13)ipJ%#RaX59n?^;.ODf48X#h>$2A]'.<&ffLf]5p8j@[0>;6&tu8AJj/n?^6$*jLEdCX1&G"
    "h?+?dksp=GU&>;?Lh=e*WKBD37?sI3o9MP8'XBN(d`MD3$iYhLgTTC-CW<#.t0O$M,CFQdo8tP8U^]2`fjJ%#YIV+8]Fq#$9wZ)4qwMi-F;9eZK[hGM(Um=M((Qt$D8xfL;E)qLKb;;?"
    "B)K?@&rx,&%s8>,QrtcM=(Ig)b9u`#UCsM-N;8F-Kg(P-F&Qt$&EMe$=K-2huZp5L/2o#/o6B2#8d0Wft$3G`.uQS%WKBD3R_Yw5^>(B#Ek=DClFP?pA4NQd%5YY#3[&JUnvE_/^[/2'"
    "6a=T8]0G)4Lv,U8n-F2CNfS(%Av(q'%Q+1#rnZN9MLg;.HDYL#QS=,M]DA+4sFbA#f&9m8-KYb%,dX58PH=kOub./1EU:kOikfi'pIXw5s<6g)BP&d)H'c,/=]a19Z$Xq28xxa#r=:DC"
    ",4(<.Y'3:.LR_GM?UZe%T$%HM=jx(:-fJm_2a<(8I)Z,*HJsQNY]8^#mI_P%3gqZ6$*x9.vCjc)N:Tk$sA[]u7%.XU&;$<-T*1%/gCq=Gmx6kX&xT^#wPd;#9OKC-,:uo0iRG)4d`MD3"
    "P6lD#bUV)4lAZEn35_)l]M[#%rq)tqPIr@C%63gL2;BoL(f?=luXV;:f3)d*O-ap7gI'N(.aeKY]B8p.>IHg)KBAqDQjo;-<;m`-%?n'83KKwMHF_19$tn=GNcW9`2W]B]=MQT.Oke%#"
    "psBe8%FGH386fX-B>xfL9k]V6.G+_.@pvC#Dn,H.=9W=YE.i;-6nVE-'=>l&=Y6>dCl6>dXQ4>dMlC<-A2ZX$PT/i)`?QcM+fNN0]Xj=.XuQ4Cli.[8RtW>dLJ:0CstC)N'%/qLP9M+8"
    "_%Jp&`4Qq;3G-U;<:e8.$>0N(CG(H3'Ec59N^3LYs),eZ(QCk=m5nA#WctJ-xE5w.C.eX-0^k-$FP&d)IKk%M2Wt1'$#bIh4dPW-t1(7jq/T<-a+ex-^Hv(N,@P[$rA5>d(FFe-XKRhL"
    ",m0a#%xw[-Ux$T.U9KZ-D7K$..)@j:FYkA#T&-X$&/GwKo%_acx66&%PE.gL%<?>#xX'^#MF1gL(.AX-QLmO(_0hfLJ1;X6Ea%I#51Gou*2bf-pbW*@1F-iLid,pLv-Rq$f7f:d`le^$"
    "bS###HL3]-Jl*F3m>sI3>?xTM;j8>CBltdO7+J6Mb]uG-p+.A-vo-A-em%n$2ffA$N$fe6>V@2CGE]9MV#nHHRju<:Y;/9/pIXw5*)-X$bYDD3wD9m8;,:wgh$Lh5bB&<-V^(*NUGKoL"
    "H(i%MhZ')Mwp//181R<-<J=?/@V5g)_<$gL>V#I#?+>'$`aGTMVIJEd+X<<--m>q8@5ij;1Ex9.vbGg)Z02N(:lgw%O]iBJ1+c&uj8$C#lHDHuK>6MO.BXfM2Pg$.uxpkL2*^fLP%t^-"
    "$V9L5tjB[9Ef2'M9N&;Qn]5W-%k-1,@Q*58AMtM(KZ:]$9prW/P?H)4WB8p.:8QQqktS9C[1f?8*fkKj'c?=lfB;gL,=`QO&L.IMRbtG%c6Dv$RW>Z6pAPEdSBm]uR&#GrF+0kkKr+A8"
    "3p=R*t8vER>;Dk#wPd;#F*.F']#G:.>aBK-pmc[.s0YD#NTiM%)Vu?9(d?2C>8:<-'k/V-+%hK-OI/j/nj9'#u<p]$cA5W-,KG$eOXj=.ug)Z$QoC)4]$%-M,YS;N]/G@MC1.AM1C;B'"
    "Nmi?BkBd>$54A']=p*H=;`su5w-t`#W<$hcw</9.odT3C#&6<-_9>%MqdUE:h+Uw0]QOp.2t=Snku_$'(?sI3XgfX-)-e#$9s/kL*[;;?Zw@W-(SgB]4.*X-]1#qrDMXw5u+[J(.Cil8"
    "agB#$5fB,MXx=SnrG5>dOxs%uCK'wLF$TfL7d&c$'3<F7mF+F3(0p[6o-e2MrXkD#A2K;-Af:a%c$g2CpI%2'2xN8pWK]d;Kc]2`X;KS8L;mg)NBCD3gwdLMYTbg$uQ;X6P0'`&rJnAu"
    "@c%I#rx;k:ChtA#N9%XSS1_KM2,OF31v6i$&B=p7,ZP6WQ01@@VeH1MN,J,8K?pTV9@h<CB1EF%e#&gLD.vW-S34WJFAtFN=JXD#U@XO)/KF:.CQ7eMsrT)&]gM#$d20oLx/Qt$)Vd<-"
    "S-MM%?qZGM@wgG3Wtko&;6&tu'jqh$w0E9r(26W-[Ytq'Ifl`%bGqp.@$(,)chomCCY<f/_9$,$N/B=l=*hqKQTER8p?GB$>Hi2%bfV,<D4=2C+NZbcEEo(<01oP'W?v<.Q&[J(a0YD#"
    "uC.@#&-/rLxlS9CB%b,2(=2/13Yo.CZ42G`-;ii'D0W)Nm#IF<wM1_ASVe&6ScQX.nC`0$U(C@/]3Xc2xAH/;QctA#O@]3b]<'U.N#G:.sSoo7_ug>$>ALm82+U^#E0`$#:*a'%m<#C-"
    "]w3]-ruQJ(96&tu4p(;Q=Jr@C(x9^u$-ddubr:8CoBoNO6`NrLC<:58Zx.9&5Y###E?\?N;L%CB#=xs`#_q>b#qlme%7Idruhhv1'e*loL*5<+MC82G`hidD+N:CgLZ3RA->9Zl$jXj=."
    "=Q5K:02v*7':'n(aHqx;5*S5's1_*e_?@+4hD]eMo0sFrY7VP-XM)Y$tW-j8:a<>d)DwmL;sZm%gAc/)f_h-4XJlh:ULu)4&0fX-v?CLEfb8;/B.'U#,*loL6r*'MLG#I#WnLW8PCOn-"
    "vG&LPO1i;-N(bj$Te0a#aZaQji5xQ;ac<>dC3+P:Z=^9VZ_7_f+5a`#i7'G.OhFJ(J1rM(U4`29l)[#6sp-/C>PS]uC,gc$:6&tueK%=(fnBi8_YX]uQsEhP?<XW-VAGq;@oWD#Tg;hL"
    "WAuM(=?]<:k2@2h[YLxLPtT=YVcR?R^6E$#5,>>#$e-^#&,-W-@w+F%@w//1+gR]u_?P%MLLuK.eTkNdt'e-6<YZ9`1mCd*Xj6.60YQ<-eeUp$I./d;jf/:`XW&CA)Na09,9tY-*k^&="
    "+O.0Wlnm.'05H>d),4cGD6]Y-xN>kX^m,.muc%Z-XPJ#$]5Vh#%<@3rt6emL5`$6%ZB(7#$3_d%r_b'4cQ3Z6iDTe$/I%;QSne8.extBC;SxA#JiQe$;j5/1(kWFeseiJ)GtR-k5kK&$"
    "+iNB-;dab$7PgIh(g>>dL`]C%@=@<-T$rd-?%%V'>nkD#<XYw5sj$/C'Mf;f>D'tu/Q?p-ej9@'*(:B##(1<d/xeV'ko###3V/d)J=Hg)e^3Z66$Jp7q4J3Mxq7DC;HV]uhe$q$7I(C&"
    "F0>>>d((kbffXkO?,eJ(d>5<-'$%uLTO.Pd]&5W-Z(tQNe2&gL^88W8hZ7_8_#EqM<sMF3o-$,Mux+[$=XsB8TG:>doL2W-GO)nj#TD^=^dfQaqV1.;lpGg)/:rkLm1H)413Ch$-$7tq"
    "aw.W-n%9kO+-Jt]_px-;S4Hg)0a5F<-9kM(Dd0%%OBZ)4EI^c9%o>e6pH,,9wAR,XjE3R%00O$M'r]0:866Z$kF#n_5_.Pd0:Ce#?h8,Ml,BHuB4Fc%cL*d*1cDT.'-p[6Z8ix$Rt;8."
    "1m=o$de(gLH+[6%HYnhsBK`.&v*fG*[KX'8axpkL(nHg)jdu-$Wl(<-,;sm.Bp#2'#O3^-*a34O(RE<-XcVU%4k3<-jfmX$)4/g)`g<dMw47F'qZN0YZn%68EUC_AIIr;-?/]^.qEwY6"
    "c5+G1#xw[-'Yj=.,?[S#5sP)/l:2/1YEV$9obh=Mgl80%UeLgLLYP&#A(qkL88H)4s,db<TOl(-A+Yc-fe'*lk/WQ..[a19CM:m8RWF&#uU'B#*`Uc;p+D)+_&-$6n$1N(J0nO(p/mER"
    "BB3jL`;EJ8iHF]uS5=gLlGHe%D:2Y]:'>Sn`S_$'u4C*eQpCSn(;OxBm@0B#uxxiLYcQLMBXuk-nvL$Bue-lL+*i]$9A@2CUU6HMdR?>#FVO;-AK]G;hM5W-[#7w[sk;i-1dI0Y^TIg)"
    "#3N,MoYmj;m`3>dI'4>d%qmLNrUZL-S)iR=0rXAuraPSd53MM%#I*1#p<pv$7ACFcO?Z)4g>V4DY[-lL>kK&$S=2=-HS4W%=T(_]Zp:8Cbm_d-2<h<L8mb;-^xvj9/hrQN()pu,d-LhL"
    "<j*F3xwaB-v%'g/d$E8.;LKu.NwmGMkU.PdELIb%2>n2C-=MGM-4br$]Q&Q8b:q^#A/s.CZ9V;-?,sV.q`6[92R95:W%pG3:]qpp,BF:.IP&<.b2v[-Do4lM-OA/.^q&,9$&%2heUDW-"
    "/VZ(6peP;-TGO2143:E</BT;.hPXisRwX8Mj'(l%NMUwB?lHI$(vW#$1[,H#_5%>%Y-L#>IaxQEWxQS%`>ru5#0,na&3(B#2@)'5,Kdruwsp=G&5>##&<Dk#0Q,+##?$(#H?EU-5qc[$"
    "0iE&=F#R#6GImhMW$*+M(:#gL<COF3BPg/)]H7g)>d/Co4@?,M5`%3%bHqHHI+]2C4NP6Mb44T=#gE>dwv$F7O/5##vbDs#0Vf5#9YQ/)_1b;.O2o>/7CK?d#E7#MA:$##X^j-$uVO;-"
    "akRP&VrSY,vsai0tc%Z-O$k5/B1Wq.w&pS8k>69KN[j=.&)d9%+GDn<Z3cQuD+M*MO>_190RVA.=Jr@C)BE,M`c+k%8vjGM>A'tu6fS^6he^-#62xH#wh?O$*/`$#.qvIMRSt`#-s$N0"
    "QiL%6b=WD#RmwiLb&lSMX%8DCk=.qLn'k=GJ?%m-eb_3FPpCSnNo4[$@A9e?G)J#$TVo^#fF-(#&m_u7/[c6<.F^L;JbmI40peC>k7T;.*l`5/vl.&4sK.29]rR4;GbFW%VXG_&q[;q7"
    "f9m&6wwSfLc#6L%pSXV-ituM(risFr9ZR^#ISA;?2K['#42xH#eF`G#I+ofLB#hG3@u[w'im[*@iLsM0#@GGMH#[`**Og+Mn/6X$`#Yq.NoWD#,]Q)4i(U#&Ml-YN1.VT.X3cQumgmL-"
    "/a8K%/(a(9o]b@n?C_19W7JVM/TnuLq]<S8h3O2CNJHW8^2),)k3v>$&C])QRl]D$T^BK-u0`g@iYgZ-+rJu.ov@+42i@m/]a)I#qs[*$nH`9.`1d-#ia1I$pb7G`EWCG)?kS#6%UjU8"
    "5,*Z6MS.HFU^ne-'Vl?TT8W=YUx5Pdfv>2Ccg4F7w)loL)V-+#k]83%#Ja_=5.,H3m>sI3L9sP_Iq-E.[?$2'iQD,M5-5k%UYo.C$+fG*@a3QAI'FA@OBZ)4k#juLSVfZ$6`NrLAwMV="
    "/f_$'q3n0#tlIfLO8sFr2e[V$'K5<-Xj=x9l'q>$bsX?^b9DgLov1-MA^Yw5+PafLnB4Z69X5H3ee-lLB6?X$V=(bnkW4G`wgCN`5W/c$Z8Yc;bF0et3Rt`#vBk)+802N(p[3[9^]Gm8"
    ",HwgucNBm/(<0/1gjZK#IUnuL2?9C-2_IU.VO]t#@PAZ$Mm'W-jBv9DaYo.CBA';Q9;:8%(biR8txJfFK.Xj.DAwo#n^87.dJTp7%n0[K%bbB]D-ZGC[3//1ksxs--+Qq7DLGGc@g*P("
    "2WYoLag0h$b*Z^##v@P#S;nmLB;^duDPL6MMY;;?JpxP8Z5h^#gL6(#jiPlLjD%+%e)ahLxeAK%]8d5hU_.f$-ZKd>E/G@MX%=F3i=MrAPT*HX1fG<-lnk4%Z@vP83jx+.24M3XdW`-#"
    "a6;$v-i?O$O`''#8pm(#n)i?BT#>)4lY:K::$wY6T<<O9Q8^;.AI76/sQ3Z6C]b/M8rcW%u=ZqM31GouCl8>C<.#7CVKS>PUd?=l$f#gLxsJfLHF?##YDBc.<&*)#NZFf-o4AB-C==S&"
    "Pm3jLAw'kLYv1x$Fw#'6272X-mE3j*0ot;M=s//1fqT&#oeI#14,3)#N]&*#Ip@+4$$3gL@Z2u7OZtY-)f_s/2IXD#kU#gLQ1M8.XBPdt-PI3'oo9XJ@3ou-Fo()<CHi>n/E=<'s[*w7"
    "?<X2C1:$OA&:)d*ecGp7sN0N(9o/p9UDTZ-e[kD#0_LJF4[>l$`I:0C$2-W-a[.[KaV$Q/UX4G`D7>##$xg^#?Wf5#VCKu?4x9A7;%b'4dxjJ/)IuM(js?8@?N?i3doSj%JT0AMmqP;-"
    "u-B=l(k)gL5gVa-1_^dFNuEE-v[Ag%5@Q1s14fQMtJJEdB,b>$I(R/)Z;8p/[AH)</LY)4Ckp+Mo5aIN]e'KMP,sq%aXr;-OY:H-4a*r$LIBB6N7Fn->*q6*oXm;-lGmIP5w1'#bWs^%"
    "?d9W-]]QLGs^ei9(#tY-_[7>d=5d9Dww(x'?#kCJlcbGMdQsf%AVJ&#gJQd*bOPV-faXW-UHCv[M0H)4]]wY%PcM3bgI$edtZTEn;AK?d[#Y/:pL)UMJ4G^#:%o%M1sx,&7f18.JC$##"
    "/PliakTJf$2,oA#6o&k$CuWD#kJ/J3Ga*QA@+$XhIT4W-_pu9)b?)-u9gJL:_;Pb%VWvTMr@M<-_.7'&:^$N0HgfX-AA0N(./)I;,-Oso0O.PduD?2C,<B<-Rk4c-%R(0<BArP-SsUX%"
    "baLANbcE,<D.0Z-WGs($,nHg)P^+W-cYSwBaWVQ1ZEX,%LeuS@QO@*-4Q&;Q]xWC&XUf5#HXI%#3@gG3%h>w$LAwY-%Ht58)<i^oVRB>QJZ0<-6Fo^$qWm>$?k;3(jWCG)iD'ZJ1s[>/"
    "'vsI3kq?lL(6K4M[s//1d0KJrddYT^2B'oLF)>W%-f*a+l,eA#K1<#6)k(7<kfCT.KprHCV*Aj%R*U9Cj)F@.-eOECqlY9rPl1<-&Xwq$^wr;-gHTZ$YgfX-SI(gL'<Y2C4*2p$Jt'2h"
    "AOQ,Me+B5;nMF&#io+gL]Vj$#=,>>#NREn<R(Rs-dOMn8erTB[f#k=G4BK>d)=>@R;C,-Mm?^-#FUh@Ij6K$.-d0=Q1TY++ij<p7/$ZEnxVH2CChf#%ke$kt;o(;Q0`>X(q3n0#qn9Lj"
    "rg>%.8;hxP9jDv#Lk3r$^TIg)`$@gLlr7DC%UM:VkDJgLt@GGM-H1[MGgA%#CTZ`-$8Np^vB33M#6fQMqOJEdU`v;])($xLV>2G`U'v4Vo#K%#]$(,)Ei-62?'L&$Auq[6g0tG<//w<("
    "*;BgLG=(=%@A@Sn@s'Q8d(=2C6>h;-mvdN9.%Q,*FNQ1%B(r;-lX_p$ue-lL$*x9.lZ'T.o#G:.nX-Q%?-op7d0vAmR8RtL/O&;QY[e8.)h[G;V?>2CA;Q32$9e;@Pw'6;GIuG-]F_L&"
    "@C8n;&Yjj$WR-jB]&X>dLf';Q5.M&OeX&%#uABjCYcWZ6)IuM(wmeM9A7=2C5F%(As)_^u50eQ%h>9XL8cQj9WJ'N(qk0C.?b0a#(m`a-nvL$BsuJgLKqJ*?UV,mqHV@M.(=2/1:JoA#"
    "[DJb-p0.WSdQs$#N:<$Q;0gfLHKXD#DAQ'm&*Fn%T#4jLc/]Q-]b_t%pF%*>uR;GBr2O2CGRa'?v05e9-<O2((tHOW1c>u@6,EeH2`J&$eG5<-YkMI&&N-qitg_S#qcJ,MTFSb-Z9-Ec"
    "?,sFrTPh;-iDQw$rG,W-%A?tJtf%Y%oE&N9H#ZF>?X:aNh&wm%?xiQawWj-$mVO;-(n(&+'wsM9V->)4?Uw;-73cg$7JwC#<pTN(HJjD#jZ*iM0])>&998w.&CGGM>fEto4R7U._R^du"
    "H4K;-bOo)<V%RW&lD>&#q)5C&vXj=.T7K&$VMXw5w1eJ(O#)a<rc(R<x,[P$nGp;-i[pn$VR5C&u9)I#ZDZ=&PUf5#0N'JAOb0^#5ci<AEF6t/U+1p$sPZ2C1:uT:*(EVne#3^#4O,W-"
    "4O/7)xjlw.GR@%#gMYq;aSY++)R_`$bE8W-*M,O2-($xL@)e7:+i&KMv5dS#8se^$.#I2C0]DY&vdFKN;o*F3HtTfL50_o%IjTj9Pe1nW<Bdg*rapC-/XIu/uCpFCKboBd8m#`-q%/kX"
    "/Vlr-/U(`$L]1W-7,-.4g3a*%Qt;8.n1MaELskA#W(tj$T*FW-wP_Kc*AQ29<ZO&#P%qc$Sd(;?[[6BR0KF:..0;hLCckV$t6DgLMA^dunq$p#58+FE8qe&,qW*20,6AgLlsk_$nXj=."
    "I3H#%EFiG3)Rg_-u6Ak=CZJ^?c_Up$Rlg?F(5>x80.6xL?WaIh^^>nEe^&SEZFBq^<LR4Cn8kM(nS-Z6+OmB]C)X,Mn[2>M5tT=YLjVqLBXU68j7,GcsVaIh=Vu]OeX&%#Y%]D=%psZ6"
    "thL%6c=WD##rIxetZgC-k&Bp:p'0*u_PR^$]m*[Kg2^>Pw+^%#ZhFJ(=KrQa7Wi7:_SHF9Bv3>dv@A4)bimF9WAN.d7/Z29+^Z$$t<r$#s&U'#t't`#Fh*N(kLo^-'uYV-XXTV6^LG-G"
    "e)v<%HfnGX'x7DC'HIRN1P9gL:<e4EYUFuBbQ&%#sE9V%#3tG3:PihlC7w8.B7E:.3;6>dKEgHG+j`a-HN5[[lnS9C%91S-:2bV$6[8<-ne'S-K$iF:Ja,g)tM@0lgu&tqDRa:8K#wY6"
    "rM?*M^1,[$$8q;-PoH&/n0Biu;JN6sN6QtLbBQX$MZGth(lw(mKPFjLXX7+%N?5s87:kM(3mGs-^5F%?V1G)4/_9>>ksN>d3g2'MtPXg$G/_/4:@,-Mr?,e>qZ_@I_uqI;9L'N(`';(v"
    "FD4jLvp.B.R0KZ-)jo,M*T+G/B*iIhG(N6M)-IE&22I2Cqx8;-7-I]%q)@D*&bm2VdC96>nxvY62Vob-QO%[@'<m`-Ap]Bdf0e6%@=Kt_ArwZ^(2Xe+<$>SnxJvD-*L3d>=85F@d=mhM"
    "1DjQ.kh-PdUXR/:wrDvo4O.Pd6<ap9_MgS8$*q>$O9a?^E,-W-H,-F%6^/hLamS9Ci4F>Q)Nh)<V4#VDuC#gLVB`oG#cgJ)DOI<-6Y]F-a>S'._3$,MgjFo$KN:uJ/v`E-1Y^t9=_iB6"
    "Ce4w$K0O>d7pPfL;6/,;N4$$$mh1$#c5[a-b&QJ(Fs*QAr_[?TZx)?G==]>etqYO%p2SQA:]JmCfwHm%bjk8.(gY9CW6'9.(p(;Qh[b>>(kK>?bW<p&[3^-Q]Bm(N.1&,;I#E'm19TkM"
    "_L^s$el6T.@4i$#owkR-Rc+o$q@i;-cbY[$wZ8>dr'hhlNRC[%&xO)'6%io.5))A7?V):%n6lD#)w@+4(>tM(8Egw#iqi;-mK3i%%ckA#x>J3%6TBj9Y,q'%/ecgLp-sFr*2Q<-3P3]A"
    "L)wY6/)UW$9a)F3eD'UDHINg)6SHp8aFk2C$QgIh2Ob$%Ef2'M-+vBCs`+gLMp`XFAsl2rcHj$#JI=?.8@>Z%T6&p.CrqM(KP;-;7eC_&BS3?J0^0W$wbapKd?a$#=WvD->K[x%b#U#6"
    "X2',;lCA-ug(F$%JbpT`=-a,MF,F$849kW8_<a$#bni294J#<.]Xj=.h-F#6:/,`%i,Ks7'O:_A?mGg:V5F2CWXsE-FASv$QW>d*k*=qgOdim$iWK9in`0i)`_8%nm8ds%`I:0CatI>d"
    "1KtZgiq%9.k@$2'u=Z<-a?aN&np$)*_.l,4RfTd'Xa*P(3`4(.^'q+M[<4K&s)_^u/eHi$=1@)E3)=2C&bM21;Lhp@[,s<%=wfi'rUkw5uNmG*(umM(-x<2C9L3>d*,#gLH:e<G.3jf4"
    "=W%iL)RFjL7KA+4h4Or9f3eq$s^3Z6A?1a#Rf%&4<7h/)A=:V@iA&7hJ/%Z8GO<h>%9JgL:Z:@%EW8q'0klgLW)HJ(9INh#SDl-$(_mV@V27<]EnT=Yr)Ts-.;k?MFDbWuD0S;-FWo]O"
    "Vq24BUitA#2VYw9*Ams-FMk%MYAg;-s('$.M_C*OTxEB-O=Ji$@xs`#u;XkO,<2Z-Z_h&?q*GE:im<wp]7a-Q1'ZU;R@wT.=(V$#(&b;MYIV=/g,f#64#4jL/HL5%nC]Rjeg9BZMF?##"
    "W@6)%xcsx+%u#gLa$b'4)NP<-v%e*&f0LB=AYGqV;AK?dm3x291Zus06We;#'imL-ufVh$#]^'H46V-MRlS9Ck(nm/ZQddu`#@5$C]c8.PR$##3J;K)Z(TY,I52Z6lHwn(2OXD#B.:jE"
    ")mH*MlO.Pd__QAO&U.Pd&ZZL-N'S%=4M0j(Y-S)4+@;3tSL8j0*IuM(AsvC#X0fX-w=0Z>(6='Gn0Biuq&w<CF%@?90kv9K@CCD.73`-#IYhs-ErVbN3]9&$Jim9.DffG3#]x(d%u7DC"
    "-ITk$u)_^uR$juLTH=4'G#GDOX9SS[M,@D*2alT.?5tM(^c?X$gLMm8A%dAmYg:9&evb-H<S(bH'sCo_NVu%M5OjZ<%U(?[dW/%#:Y8g%U^$Z-LMQm8SVbA#l=_DNFq>pL%6X0;6cJqD"
    "'9(]g?sdNNYr4[$m^B#$MsN1=>pa9B]>sI3Z]JB#=xs`#_uU_?fY-/8FR.ZRoD#f>Ks9B#D6&tuWx_ENfA2G`=a0A=p0jq0/nHg)XusZ-e[kD#3&[J(%a/3%Za3It>(i%M`WaIhB:;Oi"
    "0Gk;07)1G`Fo&&+WN0F,-ZG,%KGhuG7w1Xg-qHg)q%Tp:1nN>dbIxFD9^3>d<5)W-YVj0,AUIO;^P3v%D:bDu)#+`>h*>)4BTEgLMSNn8Hh'gGkD-I%slg;-V)pI@9]j9'JkY9:rLu^]"
    "&8`welP#vG'^bA#j3'Z6<R,Z%WxL<-sr5n>gtW>-Ic8U`N64<-^PWI&]lb,Mn)A@;g^fQaf2+>P1GmDH[C,F@B*o;-g6*i%cL2%&&RC`NJBrP-MnYl$eKUA$cX)+e730@#CQ]58L,^U'"
    "WNt1'7t/kLO]&w'Xp:8CB:hL3P)K4960G)4K#ei$0w`0CNCjT`6*CD3QoQX$m]^GMkx//1(#vJj]<xf$K/gKMwFNM-57Z]%#i]6L5_0a#`lqP8ha3>d.jSgLnr7DCV/Cga;&cr$RB=N9"
    "w8dG*9>,aN_)Bv$Vvf;-wOYQ;aRSA][DqI8B;64aI[]8)I%@<-[ne+%KSHVH4-9e?R$E8.2#E#>V1F2C;?x#/>v_?#`L]?^O1aB]lJUJM4Gk#$?C86Ck[>NDtYHA'asgx=K%R<_ew(x5"
    "4J6X$oj@+4JV/a#$5c4%SQc)<Vx^<h:4NQdZ=SnL$sS9C#*o62i)k=G+gsp':w=Sn2YLMED>6##u[K#$EaDE-UpJ6%n_OF3HWBq`/4jT`LSi=._Aon$w;'dM#gH0FJ8O2C4YB']P[fCM"
    ";`/,MlRm*&XUf5#]9F&#u<p]$s[<B-*:R20EQwY6(Ba1)`IaTB?(i%M<wMV=-(=2C<J6W-mkX/,kkS9C>]xA#)A/3`r^q:@:B5H3Pq(x53V):%C%XD#;.,+@)TU)<(<X2C3bNrLJ6^du"
    "d6+c%I#:t_2pk,$4nx/.$_?iLawtb$ljHgYVJT88gNc^$?,sFr:IV'AVl,;Qm-RW/^*of$a$qMK+&=#-Tu_W-+15OiCT=D<'X(*4NIHg)B<wiLs>=,%8e%;Qi`cs-S1;$<cFNX_rL,gL"
    "t<EJ8?Z0j(tuxfL1=1A&*a?['9WBm8l+=2CLswS@g:QV[3>.6/v$OJ(TZaxFe`'^#Q]BhLsgt$%p<qA#]C?L-83$a$kAPs-2ljKF@#CB#[v(>5r-Ak=8TvY-kt,^#mA#*laHqx;)Z3>d"
    "J'Mi%*VWECbU:,v3lAc&6.[e4`EaiM%&(q9FEg;.sChOjQKn*.lc0-MTC:J=5'Xs83x.qL,Q&;QK#6'$'4ECa8Uai0=sh9.XL3]-sUd;-nHc[$QN0.;kH$XJG+F$8Q@iMOl?3n%W^0K:"
    "&c[Ksl#g%#fZk;-<Ow6%v-P.<]10Z-P9du8)q//1$R)gLIuC[%7#r>G9x1NVihC5%fteE?\?rw],)&^0cQbZV-jpkEIc9u`#ZCrM9%2e#6rHHg)170C-h)^Z$0+=2C_$j;-6>xs:iK(C&"
    "pA7?R0lk0&7+(DNc^9&$f#:#6kEo/)tX]D=XJj5_'M]:&,0e^.BZim$LpU,;p[gHQb%CJ#s,<b$()qm8KFZ9iWj6b@R$E8.bmSPi0+J6MkEaf8#k5`IG0_P'm,/gLPeim$4_n#]t3&v7"
    "J`'^#Y=Us?$d5g)<*'8%)lKHMnFA*Hj<Hpg,dC['[4Yi4;0>Snftho.hIgn$OUkD#36=mANKs+5&t//1j5pZ$7[]T.bJ*-u^p2Y$u(^GM*ZeI-2&G`%CO,+#--W+NEt+D%Bvn^#aXYw5"
    "fInu>#5J'JH_8AeTEt1'XiYa*gkv58#UX2Ct0O$M91&$&RUf5#wEqhLsir`$[Mx8DsGN8A2YdYS5X.PdC`r(LrHH>Sh#k=Gnt-Id=sk;-p#4C&MG@W],*AX-Jax9.:/@k=Sq3XAtL]S#"
    "Fk-L>fn4bR0`7j90tW>dLa4/1&1jj9uJ%a+J[B^=PxoG3=N2_ArtRs-T@oS@bj3>d&-:[9hO/v>]7o%u,EG[$TW:kF)PI<-YWrm$&pg9;B.t`#2P0L>DH3jLjt7DCt-Tt:ekh,M>+#$%"
    "A;U<-u=dH.HL3]-m>Gs-T2WF=K'lA#m]Pd$oOt:C3slM9NCKNEAH*^%C<09.rU72'<*Zf4OFQ>#gj_A=[P^p9$.-)H]B2GYiCP4%@iQOEX2SS[ph](QR^M`$+<<Z$HJH3b5Yd6<7><Q/"
    "mxEHuc(w<CULr;-_pTc$oEs>?btnl&+Xo;-wff3%bKR$9u,A_8^JCp.7q(;QQ4IxeQ*TZ$P5W&#9>ho%:axiF[d[I-(fj/%[#G:.TO-O4Z.[,;[G4EZ&/moLEtE?%IU382Uq)$#d9YS&"
    "$xW,;*62cu+?ZD-<:$L-dXC0%j0$B#.80rT-%5_$s.YJM;N[B&MEtJCj#r/YP9;.Mo_U`NiD]Y%/.U-Mplh_GH:Y)4cr28I2epHQd+d;-h*`X'bg>N0C7YY#0Vf5##L?IMGK=:%:,B+4"
    "s]()nptS9CYJvWB(jkA#W6*/%SO(^8a=2G`tYZY8<+lo7fcC`d$VR0;J[eA,BNv1%#jp[6ZwdLMrLPW%j1EQ/$jPEdQ>f19r^.[KK,iA=X4vG*mVK;KB_Yw5pqu^]tZIM914>)4*b(^="
    "BPnk='d?2CKP$T.S5cQuqDH&%ORJ(MPw=Sn)L.jBf'F#-n4A^$@#LZJ5UkD#qV$Z6]t%b%u&,W-5>e#RKqoZ$=#MX0([ma$dUL=&rLA8#&5>##jSU5%sA<A+oE&?7/0Ed*OqIW-vF2to"
    "erE?%i2iEMtsnu%.8h;-0`de%2D,gLHfDE->1WU%%.Kj4J[kD#T25L#I=Jp.9:k=.0iV@&Kjhu8rfE>ddO<_J]7J&=(8P3`81^gLhl[Q-@CPG-c.PG--R_0%C`f`*6oI_FaK$LGs8P^O"
    "&b&b'48Dv9TL;aO^Fi;]wu*a*Hq<X-A44hEG1S&'cAL9.9Npl&5QPp.XgfX-]ZdHdSD:>d/][5/r%@5$x=Z+:oZkA#0dwlC4=O2(PgdPJ2*_>$NV;Y-GigQ(O3XX>$DTp93w(hLo]nO("
    "KVk/,`GsI36rx(:DqJe$/7-;u_t1P-%t2b$4s?Sn;uq;-hGmX$B(Ws-2eI294=KpTP2#?eTr*.FN=6##tR0^#'`87.&$<fM-dYV-tlL#ndtH6%k3qA#)4=,%<a&dM%,K;%Ur:<-j=9C-"
    "e<OG%Mi$<-oRTs&E'/eFVane-B45<&O14n-?)(+%oPus-i$hdMSk%d),>WD#r#[*7rwGHu5'uZ-R%xNOiM1Ddj5>>dwOA^$p1j2ijA7L%&D=,MNrK%%oOt:CCh3>dGc$9.7DPEd,tTp."
    "2t=Sn%Jl3FlF+F38t@X-Dqj;-JH>d$EC86CeT$Grf5J2KMl#I6w)loLHP0%%On.+-s';8..,?f*[eCgLDlS9CY?a,M$)2m&.]18.A][p.U?1a#U%0SMlL(c$pF/b6t`*vL,]4=:^>Pb%"
    "RpP+<v,F2C%sKH%Jq7o8@]@&,O__v>qm^>$*nVgLZ+h%%ou_?TmvtBCkrwFr*I.ZRVWZd&J.cLY]N=m8$s]G3Y#Us-,d5d;qP%_oPAc`%.;2/1[4q/bh<nq$,lT7)d=2G`#B;_A?#NF3"
    "Hg*P(jWw`Ng[5;d4+V'A3XwKY3LS@/PkOoMh8Go.?Wf5#N,.e%BTGwT+PZhYSnK&$Y3&H3T)ahL1kK&$($rtLn=Q&%B['2hKB@T@kH+_JJ&K0Mkxw$8h._#$gmIO9uFTYA)g.^.aQ3Z6"
    "?:Sn&RcuQ_%UWNM?II*&AGi,;ScmJ;6pr(v386X$f)>m8E9.O=l0@>&AR^#%24lAur[mHd<Jeo7LCT9VpG5l9K[l)4J=Hg)CAg;-)lN*,_ubo$C<I,MA$41^AZ.3KF0L#vrT_n#:aV78"
    "Ss1R<k*e#@[8%^$IOATiFHkpBn:4KjwpNn%PUf5#9(a3r#-5c%a;6W-4qmcZAg+/'8B,W-mFCJb$SVJs%x7DCxrI9rWVE>QpH=t-#FqhL]nHl$8WJ_m^'QJCxkL)-0+J6Ms:4f%(.kFd"
    "SM%TUEJ@2.$Dh%M*rJfLt-gfLY^5[$E=+Ls.Km`-+)_QV:xa'4VO^>>:6F2C$K+,%d*iIh&9hD<ZL4Au`X%*EdF>B$Uhsq%.M>s0-hq%OMq(P-eE6IPj/b^MR'Ls$l'/W-/&3F%r0hB>"
    "E%_>$c*oj(i?*209cO/kP$G[$vdI29.V^$JQ@On-(vI`6M?\?h%ZJoxR<w$=&QJg,M(BGc%3Od;#=OVg=RUC).8em_>*v*Q`:PRJO%%k=G`R@v/)c=Z%459a**A=W?A/)d*LbEW->sn?R"
    "E7+F3]Xj=.XN,<.aK;X6Z_$q$9;u0#M+?a*NFFwGcZ3>dLu69.J].,)i:LmA?%'Z-<'+;KI>tM(FTIl$@F;p7KXQ-dnAogLQJ+Z-*vP'QpD5&#fMh69R3qB[tVLh$(wZ)4wTV8@iJLfY"
    ")PN/%,<:DC[bD<90ETf$:`8,&5/S]ubItH:asBW/b;*M%ov@+4oX%f$Z_Y)4b-k_&Y,/;Ms1]i$ciCj9RRbA#GH4($_u>98htsY-MA66)S`:<-+)dn(]C#I#hiC<E`s0i:H`J'J0XL-m"
    "fQ`du]U)X9[b.E+hV)F3j9F#6[PQb9<`bA#(]Bp@w(Tq^SS4O%nJUQ/MX:;-&a:D3&hCQJjv55NK-H)4B@w;-cRh`$86fp7%ckA##V@b%YoI08WX_KcncT0=Si9wg(gv&=>$sR8t)B+c"
    "G(i%M8e-l$)'Xp7)X3sq$]+$%P`Ls-9u3bF-=LAnv?d]%gG-na7&h;-ukbS&j,?2Cs4;<-frre$#cM>dVJ#?esv;_82XkA#x<2=-?ibl8Y<-$$#[I%#dLkA>2%pG3c0W0G?IHg)PU?S1"
    "ac4c4L@tY-#Sdp7W5O2CeRYUDbL$gL#jD]$x^2gL]^3k<-X]6Eu3AT.KGX&#i=N1A27G-5a<?l.d%G)4beoIB[%Ow9CC6a*556N9uUC_AQ3k;-@DG?%/XdGM6/K->5S9-mn/Up.@Djc)"
    "]E(@K?m(Z65KW.GShkA#SmVE-PFoRM8:Qd$`C%kktJ=a*LkIW-T=m0GhJOdMa^9&$*wRa*7EKdDDL&UD[6?GMs3&Z$8PgIheY(N<S/-f;%Mu3+g*TRE]hHk?o@S49SG^:^07#1%GFi;-"
    ":=wuoB1.AM]w69K]E>[$LBUKE]c>p8TLrs'pxI^$rRx[>7A4K3M<Z)4Iick%)sSa*aG5<-(.08%PfY58c[f'/4BBQ8gL%a+R.Dp7HVBdOpbYV-&FnGDn9lj`$t//1>/QB?ds6UM#h*_$"
    "0QCw*p8'q7][?$$/Od;#:,>>#4U>[..%eX-ZXn'&CEaT'wp//12L*hY#J#gL2x.qLNPkG'*R6r@)s>g)38B*[3=jW-Ue=BeNEuw$x08G`L&h<&G/cwMj$Q;-?/if(YnHeQZPK-QV*-^$"
    "A1Vv=wQX]u<Tj6<g`FO%aK7kO<j=0Jkkv),Z>[u%W8us-5?<jL#NXD#$8eX$8kPZ-hwgA$?7m9''_.F@oOt:C#V*9.aW8G`rQ<p7g6G#[+19gOFST(O.KXD#*Nj-?Auq[6fW'DMP5^du"
    "KDdD-7+*J%e4)'S_'XU%7Od;#R$(,)L8DRqbW?T-E07a$L1Y)4hJu-$#;8I$C_)gL&#>Sn43Jb@Y>#BOG9W$#H*.gL4b*P(Atn;-?$R&%B/Z<-sb>Cd$R<nL$%L`0.`6/1lta1uIg8wJ"
    "URF&#isuN0RhFJ(qEwY6Z[[79_p6bmps>W%>'a^%x08G`3H^,D*;Iw[AcU&&o/[`*_p^#6v-0a#D(w/)B+eX-[2,W-cZ^p9XQt1'33P1K(#OB#-=6<C:rAUBw9q#$hM7%#3F*nLrYt`#"
    "5Ip9VC><,)X@X/%m[e8.$o%2'JrIn&J#CW-pTL?%YFt1B#3k<:K6fX-s9(8M>g*P('A@$Tk]k)c9sfSUo5LH%+G2>Sr:I1KFI/Jd4#o3r5'MHM2inO(J]J&$@r<h>E$.L>Tjgs-4pMBM"
    "q#8DCcE;eYk9aN&8+io.nF-BnYqeb%*@Y'AQXxR8:3t89pOt_$:'b-Zr@%9../B=ll:?p7]/i_Ac7YGMq)$a$^6qfLus)'6vnie.N7V9C<dAq$^X*#AE5Pn%5+_k+uQ))Zq5#&#JG%)Z"
    "4[kD#x;*#9_.(p$e0s;-F&50.4rWH;2.=2CwExP/38C=l7l5/1Qgr>[A^FJ'9<-Y8g&#A#=gUg:i>Y)4V#4jLAM*>NEt^_%0;2hD@p0wg'Sp&mB)4fMew3&+<jNv>6Ng9;O^F68Kf'B#"
    "?`Ek*W_QdM^fP;-GQPm/rnWD#KGWD#+5oq&k*M4B>nd#S^72G`299>d@Mh,=_cBV'd,@Ns26V-MP:[.%?f/c&FUTE#X'9R&Xs#U.sD*hu;IVX->p?IZxLU58+WmK)C.$##VF+F3@h8I$"
    ":Z$W]h3]u'@9Os-umL3Ex?O2CGFGk`Ic:W$6KR#$xbLP/P)SS[^7pu,?:n8.__OF3IOG)'`gx9.e[kD#p5>.>bEJ]@)d3>di8(T.Hl=DCB3dpLRGBw8]=6JrukX30ppB'#d$(,)f8>L#"
    "/2L%6,Jb`>/mP<.Vw(x5CQwY6nO&A>fM69g_&MX%8_KmL`e#Y9dT^3*mqS9C;P*XL_Yfp@-jQLGl<JPFwHqb=<hb]uVH+8NUsVqLqK$'8mxd<C#-Y<-=.%)%9>oI;w)^G3*]#gL48H_F"
    "0NO]u)qwq$$=;e?-k=r:9,5)%SAV2C5JEr9n2#&#sD<;&_gF[BOX<g)S5%JOES&;Q3[;E-AV2E-a>Lx&8Yg?$LMb&#U,>>#HGrDPA=1p7b,>)4e[M)4$<XG';6&tuH]ud$OST-dZSX,M"
    "peP;-`qx7&9:2[.EPY)%^<5)Mv?qKM/p/Rhkin`*N#-0:k#@>Q(^oGM,iTZ$B90K:id@Co/x2<-MMo6%`',/(-$#Q/Hg*P([]kw5N77>dlgl-$(xcIhP<MM%`gmA#97#4iwh)M-^>S`$"
    "fI@j::Rdv$]kY)4g;FgLVRJEd_G``:@lK.;Jl<>dwKgrK%wa7%,l?X-a[ZV-B$p*%risFr-];Y$QlDs-4NP6MIQR%+o,YW&l&g%#2<Zx'2Ps`#'LN*%1V/a#hY6<-T9bb$M&9p@W:xjk"
    ".@lZEqsSq'v%aPD6_YrBY3'Z$g;AIM&_kD#TOG<qxla*7sV=[f5v%'.7G1qLO`(#8gZ3>djGqs-?KNjLtZA+4NoWD#X?Rq7#VYH3h7Y)4HJjD#.>_)5K25%ME@Ad;=UWJMhC/&@bx];0"
    "fKO_&h%$##.vQJ(5nl<%Rt;8.triqLBo:X6Y$x#')gXA<#I4&u1k=DCtJnA#hC/Z$e+G(=[HH?$Jax9._<wiL&OlcMW<Y4/extBCOh>w%Z^Dp*S?=<-1*a2/cB:&$#&Up;8UkD#f$r^o"
    "Q%G)4kUfC##hv1'4gNR%<lG68D6O2CqW>@JrploheEr@Ij]7#%'7B$DP,2($Vk5&#XhFJ(_T/i)HJjD#gC]lJ/BP)4`>nB$JqgG3t%U58:l.NM&*0/1@3x1$q#vl$Yaw[[9L6d*/7'30"
    "p*3G`n5``*KTrKa:I[OC'4Be?SIKL*g)k=G&.(@'?e&K:6)a*INprHC3Yo.CFeCs?XUbA#mNoRAApB#$K-(fD.2=2CD$Eu&uN'['wF_)(s`0i)LifA$cMur$YvKs-h[^1KFV`QjGE+<-"
    "qx8;-%S>s-gKFnGn7+JG1EfX-vl.&4>0wiL2x.qL)<O3.`pQHD)J0hPm_oO9Hb7_8s2[`*c*U-MLXGoh0S`B#Tf%&4nYCou'J,/?.kkA#@#j*%vYnQD,(Ow9v,7X/2N&c$$h-I6R'4T."
    "9:k=.2)-h-wXw<(ac)K:nC6JrB`T4D6fDY$JFU,M1=FPA>h(RW;pMF3A:%oB<t>g)oVP'StGIh5#0S;-hNW2%55c^?SGFk4j7$##?RAZ$()E0E8kOT9J'L^#i>sI3]eX8I0,U-kBP2qL"
    "7&0h=DY@$To7f:dKWvFr4w<8%8abZeYsN+.j>SnLGCh1TOw=SnV?^.-c0vE7RF&H;Rug>$u[Uh$X>D(=?l7p&qRlvKCN;m$7sZh#C#(ijR;l?-M2K$&?&W;.r'NW-sxiKj0:8=.<B>Sn"
    "peP$$WKBD33w//15-P0Yq3n0#4C%;Q)XQ;-`>MfLXQeJ;5>fBJlXj=..Yl)5mkS9CRTV'#gaDs#?Wf5#,C3D<'V4?.-BF:.K9]a*5p&HM^fp.Ck6`99aYWq0e6Sb%J.@>d2wY9%O.7>d"
    "+6a;%.(H]%Zmh$0,`T[%@x^i/&C5n^-r]D$&IS2`3K*>5oY+##&,###G1p0%pjf8.<u6/1F=LZ$,>###lF@K:(B)c<p*f34#0S;-m_Me.6l:$#p3E3&C+XD#7&v;-@sni$Zx>p74RqKG"
    "Ba^MWbjJ%#7ewq$n`0i)V9Fp.4*x9.tA^)Rqwqc$?e;gLdu7DCj'2X%%6Gn<vn,-u01BiuM*<6_PU$39:2UB#FMsFr-a9G`2$B0'UiH9%N1QdF9Z7.Mv:xiL/NuM(6otM(bcsfL6Xg88"
    "w-L^u[)_^uCl5/1cQ_BfY^3a*(Yh&OiDv$%.T=,MU/4Z6`rQ/8H4CVCA(i%MKdGGM$@);Q.f1Pd+ja&?$xSfL(1lr$&If?TC39I#;ekD#A$E8.xkgq^8ZiN8t2ILaFU/Z$=LpetMl#)O"
    "4ht%=C:P)487j5C@<Px7V:^FEpk<>dK;87&0d,W-2?bCF/K$$$MUf5#IBGrB?1OF37`f`*$=`a*f0DjK0QJWpqJWU-ls<_$;G11#?FBDeiF]S#h6cA=D*iP0cBj+u@/0i)b`MD3Kd;[-"
    "DNAX-u6lD#uQ;X6Z^`i%YYW;?v2c3bJJbGMQpT=YQr9a*^PY<-?Nei.Mww%#]IAZ$A/-W-c+.F%d0YD#0Vpx]v=rB#Ic/U`7ZsKc#f9^#HwV'H_72G`%vp;-d(br&SOAm/uB:&$Jax9."
    "NqlZ%l_^V6:<V.&Sudh*Hg.n$jctJjap4KN1iitKSr)$#jjar$]g*P(Wvg%MYF)<dq#.-uk6,R%J)6t-;C3D<*e5g)0=V*1qEwY6^KKu.:=Hg)Gw[:@XH4AuHEet/R];;?:.TV-w`g#7"
    ">EMm$@.as-'ec`*H7Sf*3:?%.;QZ`*`7jrAsO'N(,L+-t-$3d*WS_W-b7<CnU6]8%Wn)b$P$&68IRse++00/1]Fc7u_Mk%MkVFP?p-B+E<QR8/S=I=Acrcj).>vt$Q[kD#5TKN(cQ3Z6"
    "25Ph#xJlW-A;OT'$u7DCK[V:%Y7Hp7;))d*L=^lNj)PT.7xL$#f2Ru%,r)W-Hd?dd?OL?dh-;]$2R8F%Ej5/1p9=SnuCpFCbspMNic]t^t:M*m0ecgLfv`B#H(b'4>bZV->T&;QnXAou"
    "WrCSn?pu9qVx&-u$3mw&l'GaZ=#BUA>f3>dw88a*ren(O#hps7Tw1HG.BF:.XY?#6]5iX%Rt;8.Q)B;-ZFII-CnJG&-[<R%Y1VD?JtKu]5#MD&Xuak1n^B#$7daIDvFX>nj&^%#Fg/0M"
    "cR^=@FYjUIs]aZ6+U1N(iNfP/^;_6;$f^n$.]f&=t8n,ZhkS9CvQj;-F:;m$i*Ma*jQwa->-Xd+:)EB1HdrI3XA+P(H>WD#?c[i$(lA%>Ah3>dC5ihk?iQX$n5FD8(qDf4$rT%%LqAm/"
    "LgfX-'-p[6`j'-%1j4Z6-Tgb%EhTBQ(CGGMaV$&YJ@*?d[iZ=Y$jPEd'`F&#Z1cZ<1iO<BFFMd*h$io.`R.T.k(OF3DetY$t_j=.]Xj=.VH,<.F;QT%`';a4p&;K*p'T9CM2&I#)Y_t]"
    "Cju/Ee6#h8Z^b]u-^muEpe0^#t=58.#KCSn^L5T/bw9hL[PwX$dq(x5lG9I$75we%?FU@%OKV5KHwK=8VHt1'5psj$N&>a*0b)w>JSkxujSo'.)hZ.M[WFo$8he;-cg:D&4`$.P/c?=l"
    "^siD6>@Xf%aieJj$/moLc;N$#CHkL:TXkA#8a9W$1Y<;?ZiYQsBKWB7Y,.#%=O?x%b/1<-uB9C-.DQh-h7eF<XQNIFC=bF3XYFjLam2;=Dfd)GsXAou6K(p#2w@P#U&K0MhJ1h&x.Ya*"
    "#u;W-n3]>eu:q[62^k3&l'@p73$(wemnQ*5nP>VHMT3-v4Rgrg(N0W$5@#-2Ee0a#6IZV-Pf%&48l*F3vM<i%X@vR8c.=2CO;:>dcAx/&0:CB#A#dIh9%N'JdPLtCEX<<-Y'=CnD_0a#"
    "jp@a*9xrW-fBd;0v2beA]47w^7]18.Y'3:.m#0i)u1(p$vNs;-?P[M.,o@Z$nZKZ6S9)I##2qs$)lKHMeq//1COVJt24fQMS+ARMZ'+G%o22pL-7D'#%xF)4(wJW-4'4L#9Hq209,B+4"
    "'wZ)4ig[J=A76;DV1$9KwYIa*kh*<-[f=b$-e_;?<(=2CH3b<-(I/b%'3Lp8CalQWXfd/Xt;Y,%DvPm/Vl*F3JOmO(PWJ(%KZ4W-0)lxetb5o$.?^M#@njr$ZQ&##<34G`tA,^Z*'aB%"
    "uf6DCN=%X$q8V#>;_5s.H-mUB-n<#-8PD#6KW:]$4nU@G]*wY6N=-*R6a](%jvvh%^<5)Mg[afLn1K;-F1o6iG[:6;bXv[/@enBna>Nt%oa[7;bMsI3H#0i)A>^;-wSur$7t3W-JLYn*"
    "5ED2+wp//1IXPZ7mR]S##hv1'0*SgLi&JjLcmaO<b&l,1AhFJ(=5tM(A,[J(14O2Ca3Ue+`=2G`aA1bIrvBBu%w%uK&,q'F-u^>$oR%T.GwgA$Nx,d$OdPg:#0F2Csa%#%Oh.W-T/=r7"
    "vXgY&`eow>45P)40ZgfbKaWR8T7<Z6J9H)49^]GM2L'K-XUV[%k,YN9#JH4C4]o.CFVO;-HJMJ(;l###'vsI3oV;6/*U1N(p<?GMo_v,%ag'v*BbDb*[BMZ>1wBB#eTq]$wIRC@xwSfL"
    "Tf<#&J=fZNNKJEd]vau&^]###,vQJ(%NR>%lXj=.Q>%YK-hma$XRk>>#PF&#n_rJ:5QE-Z5Z)F3]x2Z6(,+`>7=g;.2k^rgiD?e%[ppEILO1gLWMWe$`fv1'6VH,Dro1p/Q#Bx>S_R=H"
    ":`=x>JJO2C<l[gEHrFp7Ej&D76L8j-gfKh3C$`I&Kppp.o6T9C)cO)vlL2G`J5<A+QlRfLuQR*N@ej=.e?.lL*&Tr-YiDvo)N9W$NLuFrBK>;?(r0C&%ld`%P5l;-[^N;&/8L%6b-nQ%"
    "MA-p%-0u(&sE-pIjhbA#)rv&OYt`0Cc`6#6ZcBa*:PRd;of@p&,6pr-bhZ3:K'=2CDGxdm`*4i:Dc#:;sqBK<>l0gL_Os$#rxJ$/,qq`N4=,f*V:`a*V+)<-1buK&c7D,cRE_g%tI$6'"
    "ceLS.,7&>-J&4^%*fm<-5)tV$n=KX?^NO&#o8<PE^*U)&digDRHQNn89_,v]]D4Z6`=tM(QX)P(&fg)P,=U%%=SAY%Ca_m/ksp=GE=(nu%]<P>x62oAvToRA$.eA#D^g8..Nwu,w,g8."
    "NIHg)Rv(N0Auq[6*0x9.OF6eM_;v[$DK(a<?4O2C-n;U)U;(W-w&wmsr-I0upVXS[;w+/(pIXw5'0fX-=%&qLrU]S##(1<dG[ni$4-B=l9x9_8Otn)?2.WDFIobA#AkA#6Y?O(&>L5i:"
    "UTO]uT*)t-UE&b*Joi<-%B5PO?h)QN>c6,4]i>.=WCcOK$G(q'vM8PB)EIad,&?X$G%kD#:=Hg)7w&W$Ufu;-$NTn$fa%I#nipW$^=bH9#-I21l+1l%Iw'=(9PaA=9uG,*9G(gCi@c3)"
    "3_0a#<gk?B/@.lLZ.'k$<QUh>H(r=GVPEEYXQt1'3Av1',66s%lh*T.u7L%6H)+b$^w&Z6mK^_-_]>F>bC:5%X/Z)<K.2oA[+v8/*Z>G2sel:I/w^>$6/iD<7sfRJ]e/q`<Vq-$N=`QA"
    "$jHh,.ggs-VU8MKZF<$6,T/i)>uv9.YJ5F>U/4Z6-;HX$/@.lL/XkD#rjUx$t_RgN7WNvS(C6c*[3fs-mv3h=AnV78hS1_AdRRB&,ioG286fX-'YTV6HXj=.ROmO(#,Cc$f@I?d[(1<d"
    "kQt:Cw%At@hAo%u26b_$@VS]uj2RUMf[o3%/+s^6TL@X$iHi-Yg2/8>J[(pJvDd-.%2D+MAL);?'hT/)qWI6%SQ`LL9[bA#*J=s.4aPg)nV,lL9J-a$n;9KCN7O0P6`NrLkpL]$TE/?@"
    "dZ8%#Ym<^%/Y*?@^B/W$<oWD#*j8>C&&^-9tA(lKstlg+lB^$.GV0AM(Pah?FaP#v;`B#$M9h-MtQ=F3Q&[J(okY)4e^3Z6PK;X6^)ik$K0Y3Ds[Aoum:)I#HSC5$0PK1KR>D2C/)T8@"
    "Q7_W/)W%`4']$##,vQJ(t`MD3>tgXofs,[$(A0N(cxLI$8vsI3TQ4L#jqC-Mp,b]FS@cC-L9x,8H?^5U2/?T%%`dM3K6$-&;YM)FTGfBJWJuf*J:,v7hWZek(`ew'09x9.m(j=.>JWfL"
    "tG$I6sg20C]o,-uuY=@9Pa`.&g.Hs*6iDN9/n]G3s@]U;bg'Q88vsY-dMEmJC04>d^^.gLN>,HMg3=K&gUY`-R3n0#`^$4$A:]jK@GTZ-D*-^$_LAmM;1'^$6L[>#T=2/1$dkg$s#+?R"
    "ddtg$AC86C6=A<--XGl$WMjB-Ul1e$f%2g*+nG*O;?u`b-V<08&oBW?0>D^#9*F#+8+9ZGt4,H3NbZV-E%a5/ojLEd^9_WH'-q#$nw_?#JAI>%U>Lk&VI>A+WG$T.Rf%&4pJY`%R'g20"
    "Jl_;.P6lD#<G-+%`b.<-Bc`&.G-/;Mwu1p$]?D[%K6D_&T/Sk+V]b?KED]W?AFV499l?(#,vQJ(%uw@#PImO(k_Y)4,i$1#5ZeBu`8-;u#:$C#$Fc7ufPB*:H7q#$wN7%#DeAZ$8+Tau"
    "O<H)4MHq:du9wx-3bNrL8Ht1'aNq%uH2^]<q:p&?33DhL:23Y.X0*P(rFCD.QLmO(=;8Au*2GG-t2#>^cPR%+IF7W-FBKFI;%I)4KQAZMAJJEdGj9>d(hL&uOMuFr;_Qt_cUR<-a-%I-"
    "V2+L9x.3Z6OY2;?hXl^ZAgo?Mq&DP-KR=3&GB8a*J?P<-B<bDY3Bx9.BD0N(83lA#/E_78Lnt]uRrrHCe7+<U%r7DCgDwglPFa8.PS&##o?/W-@)gQjEs-b6xL9WM][Flt4N&;QhpuA="
    "2>.&Gu7H<-W&g'/WtYH0]_G)4u9rI3A$,F%KMi=.le<R3C%Bx>Q@O2CsvWq;_C^DQ6o8gLww4A9rr<#-@e]W-MJ=lE)*x9.?`-W-Of^4j&[W3Map(-Mii?=lUGdw'&VG#ne#k=GrG*^#"
    "hllr^G`k/Mb8L%6R^V'8'#-U;HOiG3wl(*%U6&p._]Ol8&66pgc]BK-]l&r%uMC%G0LH9i51Va*9JSh*3>ds-f9rkLvc6g)ihns-A;xfLKWBMBsnN>deK`l*_'qmJkcQM3NtuT:^PO&#"
    "iS6I%.P)k(Z(TY,U_?X-.q7b@6_-L>YuCs-g_-lL<,n.6/'0/1esOEdK2dc%OdmgOmw-2&%G@k=H]8d)IH;#?Ux,@dmp(p#a#HHu#7YY#kSuIUHgCwKv]sx+([kU%:UON0eS7u.:%1N("
    "].Qq74[3>d;mhEcA,sFrgO(gLfc[a$U<L/?7'rS/'Zo:qXFdd$=IHg)A5Hg)]$UfL9'P58Q`Pb>?:^,M/]Ls$@=kh#Ze_gL2+<d*@%`a*oMDQ/c0YD#Ul.&4@mO,D4]$'R+x_5/kxcIh"
    "gNX%FwSLp1_EwR83]W-?MSu;-B&qK-?bvxL*BGGM)*i%M682G`l8Q2Cw=tO`N7qY&a=&[>]E-tf7u%;%v,Q&'pv(.$XwbW%q;wA#^U5o:F='[8TbK-Qr(g`-kMvE@ZS###k'4]-SRx+/"
    "Z0x9.(=_TrkKB$AfnN>de*loL$8sFr,))n8c.%KW4N9f$r9Fg:VC`'8O#Y`%a][^$ivtBCBL/W-(w8?7hqoTC:e$iL1X'N(cfOt92UkD#Bp#5&^<(nu##CBu5[ml$P;ka*.0YA?1CV/)"
    "6`jEepPwl*BjZMMqUQ;-F8mi'ESXV-WV/d)aQ3Z6R)_^uRv4qLE8x3%@TdD<YJm92mb^)4Q,###)Y6G`^S@D*2uOl;/R)m(>E,<.XXb'4R[C7J4%vQ:KZlU<-4ww'K`Um8`h?#6v)CCe"
    "I>P_/XfP;-IOBYo7e0a#u_=_-tNRm%dgZ=Y3BZ=Yu$8W-F^vbN[l?dFoa=Y$,^R<.cxF)4E.^S#NC>B>9qN>d=w%'HH_nVoQ),L:L7,H3SQhA$lUl,MAO&;Q^V=,Mgp,L'<_n`%,PhG3"
    "p-@a$34lWfrn6mL0=L`$/''MuYWWEClS_1&-jFp&Vf?63616g)NIHg)Xgx9.>.OF3H_@e*GK,9.t28G`RX$gL_%].'[Z5RC]4e]-cO/bI]l*F3C_0a#'>jbM;mS9CNSLs--Un%M7c/,M"
    "fWeI-ks>T%a&Pj(d-LhL0JlrJ$=KZ-Bw(x5t66]%'YkgL0mS9CQ1pA#s)S/&9w@9MH:w[-tR#Obel.&40^a$%#6jrrkkS9Cj6S>d652,>39WFN)*)R<gWo@@=l*F3e/mfXZndLM4lGp8"
    "S(k<:kjP*Rq>$2'1geH4t<rp$KTgj9#+#H34t</:#g#<.sG#J*xo%Z-[D3<f'%8DCZ?bH9uap82gm2;=Q`#cE5W06it*WhbWES/&k.$##%vsI3jotG'uQ;X68Gh.X18dX$@1.9pH$_0%"
    "tq-5?+xq[G)2=6D/0Ed*R-],M-%7g)X0fX-e[kD#OM*Z-o_iIlC,sFrGH0W-xXAF%fI:0Co;2/1>4Q4VbxFK3D];Q&V%2W-wAY)YKI3]-'fg/Cj=*Z6-M_WAn*]i*Nd9b*oSm/:Zh[&,"
    "bw9hLW^.d)5af(5u0dx51-g8.hOd;#a.K&8rT+^&5cp:DuJI*&a3PW%n'2^mT#xY6e@k_$PhdV-(1AT%7&Dp.7>[9CURLvnm$T9Ck?x1'jIbb/h=2G`tT;3V_/QJ(#YieNZGuk%]]=,M"
    "3;I]MPQ)nuCCK?d8&Ne$B['2h4$:N'p8g>eaLXMEhw=Sn]a+W-4<'_S/:rkL7DqIXG=Tc&a$o<-/Llb$Ym@j%?Oc]uB$cn*B.JW-b`^Xog1u6`nn6&%,]LS.Ggx_%fRpU@HcI1CK?xD$"
    ",j#v*`(5gLhZ:*:M'=2CVa5d;+kK.;3a;'Q1Rt`#D5MH%St;8.O1cOKGo2voXhx(:=%?Z]bDSS[1M###4NAX-2`*Q/W'rFrk)1<dYsL3CV]fIhu03/1x_@e*^4'n/oxw%#stWD#Vutl8"
    "se?#6_oE?%(j(&=%E<#/3BGO-UC'k._9$,$/Cai%i&)`=m8)d*p]AZ$C9V-Mq-;hL`o1]$/qL]uYHQ]uS-D9.s5edu%;hAOHJ2G`4V###d1Q5V*0fX-/KV;.98Aq`%,>,MhN&;QwcuNO"
    "(Zi05B94<-:N>g$spaG3@r-W-uS0b$xHJEdhNKB?r?W/Np^u%M`+v.#t$tw9]5hG*0H2Z-aF4_JWjBp.v?1a#?vo(mk5Ch$6LN>dOZJ'SK5X5A_uF^A9%OJ%@a_7&5A;<-%Rg_-@h8I$"
    "*28GP+/jt%=g0W->ZiHZ(xcIhA%(gL$daIh(p(;Q;_sS%Wu<f4465O-Q_)>&.+8Q8m->)4`&A4KIB%4=@16a*4Cb)<@h?dF9Z/e*J4^v>*(BU^6]F_.Zf'x5[VSa$RbZV-ie-lL87&F-"
    "GJ>3b,j#v*H-sMKbJIOtQ7]Wo.1P$%A^xAIA(OF3<U3W-?id$e.#%&4b9u`#g_=0*A>?tKfAC[eOo()/Sd$Y%1Od;#N:J)</44K3FD4jLc'IY%Xe=kF[_Y)4xv'oSpC.Jlsc?k*x;>-M"
    "r,G[$$$At@]-UB#[v(>5Z42G`2>s+&+g5X-@4lBAM(j6EcQ3Z6'p5X$*]Q<-r6iP2%F(b:r*eXfjm9-%R&3f<a8kM(%6CW-D`:jV@x3KOG?5HMSN_`.$XWECGa*u*Nvq>GflfQa*N@X/"
    "?(OF3^_;p77xmW_39k-$a02B-#k5`-5[+[^^d`.&1p[u'uqP;-6APP/:avY-:r-r7VRZpKtpKp92jAiF=7F2CR.,d$f#E3`qeP;-pGSsB0#WE-l2YPVr;nL%-BKgC]'9*>6+Un$F=r(?"
    "O&TO91C;G>[iGp8aA`BSx_W2C>g&?@2n-a$ha%I#q3NEE$*dG*b%%p%av+W-ukh>9j*7Q%oo,W-A1#RNbnvcMLxvt$o^B#$X2jFN&k*P(&nsn8U*$Z$pWGkF9;u0#1Idru^lq)..MgsL"
    "+nu9q_6of$lT'^#&0*F5a,QJ(0+M4Bf+wY6q-RmLb[<B-]=Tc$P8W=Y4oDk9iG@*->dD($U$Yk4QF0k.mg*P(Imdi$KLLd=Mag/Hi(a7u]hG<-cjJZ$9/XXSX8To8pl5g)nXwnV1(Yv%"
    "R4Fb&ngO'mtmfB&:>Vb<L&L&FWpmA#vOlo.2t=Sn0>S6i5qp&vD1OF3D6;hLb4L%6kJQ[$)mc;-o/=,%11F2CLZ2<`mnS9C9vjI-DW1>8=ddUR:r;r7ir,g)]j%gL+qr&.]<3jL&#r)&"
    "X7@]@ikS9CS_7&Os?Z>9hYE/2F(CL>vqd;-8<BoL2oHg)RPX$Kg@C:D1b*HDxNAFeCf)v*C6sDNor@Q-')lo$nv@+4Z>n;fs%w.%Y>XDN15;p$xJ5N9J+l3F:(H%>GMF]uo$JF%)];j9"
    "(0)d*]e%N(kx8^$'$(Z6(7(%%qd?s?Pm0B#<PK1K+WE[%,:[58/VgJ)@@.H3>UkD#K4+'=xqI$7<1sY$Ir'gLI/h#_[Ap49%T'N(=<$MNINwq7PngdO?(i%MPB8DCpI%2'7l5/1VrT%+"
    "lqG*nR$f<-;s4]9K-wY6a%O$%)r@X1TVZ0PnBuF&3G29.v$OJ(09>sh^bdM&p#vWBC[k]uXgv1'L+oA=i'S5'2fqG3b3^OT9gkA#5[h[-A*JO)f&k=GflIY%]]f$gWZVwBh7RL'>ZLV%"
    "I>2]-/OZE7^r_1&DfD0>H5YY#b:am$)Np;-PD:w0lqkj*DBi0.'ec`*9a4i<N@S495Ab>nhpJ%##o1d*c%i/CnP6_#=f?X-Tklw5oK2d*Ph8<-6ssj$D6ASU^6:A;tkE>dp#rU@q9[w'"
    "Gu@P#;T3>dTpf^J5C+YH-`$iLo=T;.Q@i;-XBR'OdM&,9H/'Y]Yq=D:2`AUDWs*g&K%g6'TUf5#'?$(#20r]$Eff`*@pKW-p>1b?tw:8.74lpW:=#_$BDjc)f%`a%rIsO<QIOR-*_on$"
    "R(T]u^-KgLxfOECL02^#Wo6C%C3>Snw&'/1_7tQNRk`G3t6f'&e;1g)qKj;-a9Nb$F]>&O9hjn'9JrNtsp^r(eH3jP;GH`F5ZH?$JAO&#&hrW@#M&wo$'6i;9s9B#-^xY6IP=o$M/Qs."
    "R_T;.xrCW-ZOIk=?MsX%uORs-'t(d*d_.<-1Ut_$3N<UrBAfs9pPG&#5O.e*G;V<-Yl[W]BCCD.vl.&4G1Ot:-r/E<Jd3>d7[^#>akZ-Hpe?i+k2#&#[AY`%g*A4COD_6rBs*P(ZYkw5"
    "`:Qgb-V<f%T1@w#(l?T%08n2C]a)I#V'KV-%df4VA;5?IfgA%#D`*A[cDsI3kFhwS#1qb*CF[Q8^'=2CeUW;o&CGGM0M-W-8:p3+we^>$$Z/A%J-[a*Jq9g*K'xW9ZN(lK&NI&#svB#G"
    "Kj*KEa=2G`nn=`Q)X<C9+A`B#Tf%&4D>pV-b3O.;jBSN2(-'+%$6V9C7r0PdB-s]F8[9N([75;-fP-<-$a]J&e0/>Gf3*Z6%5-uQ:xYR]8HRf5I9T+I6H_k*i,MQ8Z=8FI8Od;#JR@%#"
    "t[lc*G)m29F>$$$*0fX-E60I$/t'r%Vu(W-3g'LGs)_^uTQGn*AO.PdK@/q'b8SS[bOPV-VL>3(p`0i)-mun]ASK/)xN<a*eR&39UlE>dC7,w%ER^@8`C#I#J-9;HdJQC@HoqA&/al;-"
    "u;Kc$4MjD#a0YD#X^3b*hd9dM+i+$%-5$n8>MUamLK&Rj>ev.:FWgJ)R9KW-q.'/P0L*N0/j4Z66fv[-emwiLpX%.FW2F2CZ_0*9t/2hbRh6n8h^c6<]QZ`*u];i*[b=t-EmZo@WCi=K"
    "I=w[-T4W63:&ZC&4_@e*YI=Q/<>MGM$A1PdZgk;-`;ukO#xDi-O<C1L9bAg)^k9fFm^:#?-@mbWr4D2OV(A5DNm6XL305k-k#+^dh:AouF7wi$dwXQrqF4b-7R:[%?;2<-$:Rq$@xs`#"
    "F@T*@/1Ss-x,@%He&l42NXj=.u+:p7<o3>dp`@v>=w:=0WK1k$/sI&#_Mta*j:VdMA`0a#hiP<-#PTC-dZjZ':P#gLT]ud$dP$6/thni'/tn(<LqQ#6m<GPAx],6:F_auN'q<MIWdt]u"
    "#$HHuG_&l9m?.KshjJ%#*H?e%?V[/1JH/i)4ImO(m_b'4ePvE&N5s]ubOiTd/oYe?xB*hu1L6`%$1I0u@p(;QOl](%JdgB&qD]j27u>>@K$wY6)&V)&=jT/:7]h?BZ`Um818O2CEWb#S"
    "jL2G`Q8?SnlAgf8o>WVn(Z[oDQtt0(pf_5/Z0x9.;)^b*_n>^=2;ksS'SGs-OmxuG5'CA7VNee-7A_w'/g9QN;'>Snl'n&6dW9Z$fFW:.B[ZV-_&J/&vt#gLuxvl$qi;o8Da8U)hBrZ<"
    "swq[GWR'N(SIZYS,cPW-TJWEe5o;dcuc2a09];w*>1DgL3*JV%EB,I8XLl)4H0nO(6Jkv%3j4c4TiG2C'ZeduWN$1%l+)S%f;&gLT6[C@n#K%#[hFJ(9qv]8dl5d.DQwY6E]'Y&Y1?$@"
    "BcB=S/W0o%*Eqd%FatKGj9gV%l*_`*HsL6/[Kb&#V;p0Mw5L%6fwu/@%^'^#LQ8;'Uc8'f0ptUA`F=1U$$X72g:5k%7PgIh%VF]uvNBNF<BFQd;5?6sb[V6N;j8>CWGN&#Prhs-/-<JM"
    "c9xiL87(-%v_j=.T6KZ-XXTV6J%Xh5I.8Jrn=1?p1KToLu(a7u<p:xKwh^/)atrk^G32R<Gr9x5fPF2CG/$T%#SO]up,>w$SKX>doY+##q[MA&:n]f1B.vu&^cY%&]66GMtI1N(q=.H("
    "EHH^=d4=2C*N26<ijKZ$>`(q7*^P8/?\?X3&#:&T.b>2]-v(TF-[I.m->v#ZAABMm$+C]T%ORJ(Mk(@$A=6O2C`4n0#U$QFWYpBn+^Wd`*=M4>-AOrY-g?J6abW@)$k[u##F$(,)>uv9."
    "D7Wq.Fo0N(4ZCp.7q.Pd5D*YB_2_^=XBWpBekAv.9(V$#+<ph_EUP4ish=5$=a=U$2sM(Z@w=SnBu*mScunHdQ]A>#Y[t%+6W&8/)>uu#1B0[M`:N$##,BP8WuPg))QA+4(t]*4q#G:."
    "7CNEdksp=G<c^GM5DR6%@/o^f$0S;-^#wE7>X1?%2QF:.oDsI3b#pm%bpP2Ck`a'8_aoBd`WH&#&.$_#kSuIUUml:dTu-A-l*)m$g0#huuDrk4GsC2CN)D2Cb%ns-lG#)<''Zg)cQ3Z6"
    "&kt;j5U.Pd^7EWB1m?l*<Evs-Ww%>G5?^;.]RBk.D[kD#dSt&?Td3>dv:5qVY`gZp4(lEI_<7_8=j5/1XZ)eZj^7_8AKXe6gRP>dUvUh#4>h;-_`%f$?0niL#bH##9rC$#VUvf*VEx;-"
    "VWww?I:X2CD$j^M5WY8.'G:;$/U6(#2t=Snj*io.$6BJ1U_?X-HofS:L$)x5/awiLvFF:.CZ`]$K>mA#a+x;%3.5F%(vTa*8+Sm8)3I;q]UY*$owAgL.ULH%m-7gLM6$##KL9k./c=U$"
    "E[*+.k6GPAA#b>d8Pl=G-NL@iROQ##('BN(T0so.$Mm)4p@cS#qUKv*:J&<-4_)t%a34G`Yu8>,Q54a*[aFW-E-=L#wwdLM%^CD3c]8&(n+$L-0rs3M=x8Fe-DOb*pwV<-C41@27C/aF"
    "]'$`IYe'S-5;1%%=R?gL,#.>%CvVEej^CG)@>,edD&RJ(Zax9.+hv]%H&&B#SKGvn1I[m/Q>f19o;2/1atFp7V1L'#(G:;$2h?O$Y;GCbE@h>$*DUk*^i7<-';EtBWk0B#Z_Y)42dZh#"
    "bGcG;%jj7MuB)t%#_B#$'tn%#jWt&#s*9&$5OMp$].OF3Wv7n$;,B+4?(fAG#W^/),rL4ja7^duDHL-Ze(T#$3L1q7>[b]u96QtL4WaIhLlBgL1R+1#>)#H7Tej=.<6fX-XY?#6oG.a*"
    "KV_p7ug2eHAdk-$XN-;FJugXS($uj*?xQa*uY[n8^YO&#FAxn$k85l9,hmTK7]?O@N(gG3YW;a-r6[R]7`>mq'FGGMg>O@85]c6<uQj-$])1G`'ec`*U;:WIDH2hbc+`b2GCJFI$Ew%+"
    "bOPV-oE_EQ7ekD#sQ3Z6#fx^$+Zh$'^$aQjwNLU&(EEk8CA6$$DXWECuKX%+^:%T.pi_S#7Guj$c#c8.KaR;-CUA+vM<Z)4R*,g$HHd'AZ_Y)4R:2=-d:FmAZ^@vn)FGGM$>xi*$U1<-"
    "?&FNNm-g`=AJH+<6.jB#A4f:dZ9V;-ch[k=o`0i)>/K;-2BlOJ<l0B##Dm=Gj66^#WM)E%W*8<-_NXR-S)OWWXFOR-VbnT%r%%^FmmN>dx_@e*j#CgLH:MhLFSt`#hTaG3Nhs8.cxF)4"
    "r,(9.BK>;?/X]Q8J7O2(I0+N(aF.w@I90]:s-=2C.5F^#:@$h*%v$9.e^V;-TB7-tSOqkLnx<:%vST29T]TsL?E;N8V(=2C`INq%FBa@>;s,g)oi+dMB<L%6&lo`*gcvDNS]K%%EKFW-"
    "pR*r['LBJ`pjf;-j4q(%l_OF3YZLnjE49Z-DII1McdYK&MqI)E;JGF@Tr&h%l80Q8]]g/)d-LhL+.;hL*TtfM%WkD#O:7>d>7x-QmM1Dd[^A=l:wtJW(Q+1#cYtk%CJsc)T4+S&'EsI3"
    "2Zlo.kb2/13Sk$KPPWw;UAXA[R`:S%=I7m8x[97D*n$&OA@)p*eZ#30=9W=Y7CNEdOQYt(<g#9%*18G`n5``*U_?X-o&X_%xBSp.NbZV-B9)/Y%daIh$tn=G]B]<-48:x&kKaQj@vQJ("
    "Yrx`*ZM_p7K@c)49,B+4sf<?%k1a2CK=5)MS=8BLEZgO0PR$##%####<2niL`,(XSc/8q*3/igLmIr=/mxLZ-k,',;g7L^#_.kFdmM,E%dfL1*2Od;#/a.^$o9p8p<#0i)c4qJC=aC$^"
    "^S-<.[P:Y_=c=B#)l@e#RvpG&H'O>d9s+A8`MgKM5Y###DtuM(b9u`#'l0B#NbZV-oi9a*J#29.$ErFr9p?a<LYD(=DZT/)G7?xLZrHg)rh.ZT3#VD3Rf%&40T`3X9JeC#s3oB&r+Cr7"
    "^:=2C.&@2C(w7a*n2.<-'j)M-`fbv-Om^$+oc$X-W9k,O?f*F3-n($5p/5W-)Jd3=a,8w%1tHg)T-C0c(R)x5;_G)41j4Z6Je$&IhFcO%tCM*?)Mt2Cksp=GkLm*%xjo/:E#CB#2Vo.C"
    "XfCKN;u<F3B1gv.<6fX-H[(G@PGpkL6)i%M]*E-Gc`3>dwB@p&`#9m_0x(t-(oPfLZ]d^,Gj5/1t,C;?&,###4n@X--(u]uQ8Dcul'eT%AO,+#k6_S-A`;Y$9%1N(A&&hl5Lv]%;NL6M"
    "WSmWKvh<>dbXrh*u[Na*oT(<-QmfV4D]-<.P6lD#J]c8.^LuFrANv*%oD3^#Oc-n8iBF]ulND(=)>_WJ+_uGMHY_7D=F_19QBt1'>3U&'*p<A+;1G?75UkD#6R4)&dYDD3ttS,t6S]F-"
    "/BpV-3OJ0s@1.AMwO,k%eP#<-9]*j$7vY<-*wKrBwPF2Cw@9C-3%4j&AXSJMKuh;-L'>c%ObZV-3X]s-FMk%Mc?CG-#q//1(_pI%^wCgLYR+1#%iuk%Ck:JFU/4Z6Rs_x$hI:0C38C=l"
    "$`?=lHiJ&#k%)t-^-)s@gV4l9_t>g),)7xGmeGs.uj*RU$^aIhegrZ.vVaIh8C=2CjeFt%vCTCX]q$L*:KihLRN+JI8-7g)e^3Z6@f%&4C,:s7*g'B#]).w>#d0^##K<3&rBU&#iZsn%"
    "XLjl&`t-q$pR2N((m:A=I8Fk4Nl)W-Wl+w@>B6'$wiA6s;j8>C.Pr29QXbA#E-0<->9[8D_j*J-dt(o*Ni$66<[.Pd[hX=YH`6t-l?U.RMI(]$+,0IO+bE20wGq,4dpK-%ug_S#wFoKR"
    "*`U`NNKs$#C8kGM9_nO(^9-,M[(Tq$#3',;=E4Auaw,D-0Y?.%4v,T.kxcIhK]]C%LE>d%BJXw5&0fX-YpCwK9@x1'-2O$M^(c(.&W3nKK:$90;w8.EcI0N()`vlLkGJn$XQa'?O#41^"
    "@^iA#8['6/#UZS[_@f+M=)OF3pWoJ:c]3>d1/@2CI<%?$:E`pFfxBB#eh<SnM=n92QGm92-EA#6H)T88FO=&u,jRMd@jK8/Z9V;-<'<VQ@CGGMAbu&#VLbA#.0jiLThwA%9<'F7bfA`A"
    "NNZq)tlIfLG3I*.-E@[>:b,g)%cis-V#4jL8NZ)4AY5g):vC_&C>(W-Oalk;.#/'.,m,:9h^3>dHRe8.kc:DC:4IQ'9f###u$9?.LT/i)uYW#$)HdEn&Uj;-=xsnLKrS9Cah6M3'[JEd"
    "avj=GY:9xT7XEs-3gJb*;KgW-x[<wTgh3[T7pJkj4N&;QiFd;-#9]QFmFY'AXj`RS+WO&#k]Jn.Kww%#UA@^$Ji$*4>[ZV-9e&7LvG6*Mc$/qL<Umh-FK&R_1TgoLbj%f$@*&REq3n0#"
    "xZj-$4Rjj(rgai0u^L9ru@n0>G^X)4LBwms[?@+4l7tA#c@Vt-g_-lL@T-<.u&k*./h:+=fsbZe=qC[%Fj1qLj^xrL/%QmW+=Qo$TUIW-2aHW/dQ&%#D^^4&Cao%F*^,g)B$O/)cMQ9i"
    "Eff`*5D6<-f3ON.p>bG;R]@<%$+TBo%q*JGow9B#.V'^#'F'p(iM1Dd%u#gLwM)7#@FU`.TtuM(N#Sr3=]C=la+gIh%2G>#3,ED#xWf5#5$'k063(B#:[_P$WMW1#*lO&#->Tl9r[-1,"
    "M/###t16G`@iF#$Ln2T&hTP'APtn;-WxS,MK,&n$I6%gLq;I]MC4r*%sm7K`/xQS%A9GJ(lC2N(#;GjLG/gfLN',V/tODf6*qB2Cw$M,MghNB-,tfN-v=G0.eb'-Mj]<j$;=Hg)G^SRf"
    "1k3>d*-5wgwrapuwVWEC<]mHd@[Ld-^;VF7ceJM'6]###BBrHM?>Bi)A(eX-sws^-5%#*-2@^k$F+l>$'msjM%Ghl8i8HpK9O-29M2T;.2l7W-:DE6s,cd+8]w^#$/0O$M5C?>#J-EtC"
    "ZScJ,jOxo#(d6F.e2[4vwKx>-ATR2.DCmhMmFuM(uuW`:3(72hssEU;aACq8oZ2:2ba95&h0F#6#geu/k_b'47l8>CxJQL-MVMA.7H/Jd>*+W-?-uQW_+VN(V*x9._B</MkbZ(?9/F2C"
    "uFPS.$&>uu%eF?-=@ZX$EH=_/'-F2CwQjQsW*G6u$54g$s.rR<ZD###-&[J(S0W8..(a^]'YWj$*Na>$Cwx?9Ef2'MgggC-6sY@.7p&;Q@-x;-<;iP-#ZuD%s>$2')tY=YM'%Z$)D_G3"
    "e(9B#Ej5/1<FP]uOP(S%s'0x9$^tw9X$o%MWpT=Y.OP9`_DuM(u$Uo-LQ2F.ivdA#iE$?Q#UZS[t+>REmLtu5;$UfLjFt1'3Kn>$]/LS-+A?a$Q*D^#&+T@QtAC<.A$(,)).#Z-oAwA#"
    "[fM*I#fXR<'x&OF[HoM(.4J#6<]Qt1RR+3C?)%[#Y4iEM31t88q[lQW9,`$'wdJN(7k3?#E4?&%d`<Au:XFl8F'(T3Zxh]P/Lt1'2vqUI30=2Ck&Gs-V0niL<x(hL5HuM(NI$.MH1V8."
    "9#h8.$<>DC/PNfb$:Y(NWklgLjPqkLpjbk1>T$&4dW0/1n,dduS?fZNhFg^$Y)fGMGhWb$Oj</M.]-lLa<Y0.8'K0MdA8b-'UE0lW7+S3qw@_S>gM*e)G0N(@mTK-=qYpNN-J6Mgr?]-"
    "$_x2DT/eu%t3Gk4U6d>%uKB2C7mW9VYqVBOqkHm8,G[0>S)SU2b=>gLPG,d0-&[J(sQ3Z6n?xR9/._^#&i,Jt%?TQN;L6M9('4m')C*W-f&-=1C/4gLvE2G`Gc/%O$7n]OM*G-;VEg;."
    "D1>b@>._GM;UV$%sr)J%M6Bm/4NAX-p*3Z6RQDp*246gL=[1N<A$K&veId*Oi8ULM+]aIhuIHb9[dl2;$@c(Ni/<$#/gwiLNMx(<C[3>d.2k>$bS6T-(]wm%r/pM(S-Y*R^lG*R.X)dM"
    "b-HF[9)aa-r9Xq2ak:X6[x9LM<4*/8'I1_A<x;sKbY,)/KV;;?UF;4^VLp;.MMRAG__3>d6n)dMK3Y[M:A:@-ckQX.sq(x5HG+o$$A#I#du4qLBrdX.aP(SuLW.b%QFu]8PD(I;hjK#$"
    "0gmEM5#/qL%I:@-fV?iMK]P5Ov`K`$3I?6sUTV#$:@h<C>_?=CEf2'MESN08>3:kb+MqG36C.eQW$-^$Z%#Gra-/[Bbk&j*_+Zp7L=:kbj@dER]?ZcMi(xY6pUHq;Pd+W-72CkbxWB2C"
    "=TEJ`Zpf;-Fa*;MP4I@d^3bU2242S360IwB]sI?$KG/JdCqMBMnH1[MS-F$8t0CWf?=8v$?2Cs-NE1qL3>a7uaP%d#@(iLduK*^O,'QJ(7_OZ6po0PdS>Xw0MVxFr,K%=1F&###Y6D^#"
    "c_-lLp+)mMQFt1'-%V)NRecgL5Tt`#8a]Q:9&QPM:[T@NLh1f9o?EZ6HXTV6O7P2<aug>$uxv'89^cF@(*[^#0Djc)#aX0M%8u8OYcwe-BGoNOXtTfL=A];NwY&d#$bo[9w&h#$MUf5#"
    "VuX-?[1l-?s7`0lMVxFr+[hxuJEf;-j$(S-.1-a$k,b581f0^#:L8$M_sBh$HDpk4;M'gL0p@X-GD6?Ixp//1GJQvIEnT=Y&$tP8(CT9V[c###whg<MSldM9Lg]mCZUAG9CBm&Q^]=^%"
    "oN^_]QK7eM&q'TOlV*_&MX:gLO7V-M^$TfLIvv.%@6wiLYc&k$7PgIhdK_GV.,fN%*B'<-F$ar8=xW-ZI4Zi9g^,g),ckZ7-v$&4G)P6Mr_aIh@INauhdA3Mg5H*&VWf29V(Jn3.X^h#"
    "#.EW-$lBitq8Uh$keDMM_*^fLOu3$%Is/kLmGXk%hR>,M]G9@%XsgG*b:l,Mvm0a#IS/a#^brDlxHJEdhDgOKkjf2D9?VhLtWnO(4.uSMSoYV-9f./17gc5/lgcS#4_3vMQ,N;8nG4K3"
    "6N1h.2_0a#&UU=%m=`J6&D6<-Rj[j%9.P.;5XF$K77GB$-'<b$jSc;-mVWU-+smi$n-vo7R%L^#^Hq:d'T)4)Eh&;OQ##.;M@iRs]#5W-H(N9`bI2<-fJ_5%JhdDNK=Tb%PUf5#8tfN-"
    ">M?.N;*QJ(ebUd-g/VL53*7B#'Y`-#sL'^#7Y531jbP;-n:'58CvsY-EFag)D1eX-atL]$Qe+`MjUa7uEV3>dP+*##4,x)v8*loLk%<$#Hak8.ZTSp._1b;.mi3$%:E1qL[B(%OwT&ns"
    "$r`>dV>?;?eI^'Z;l+G-K[Ld-Z?5%7'@uM(:f2i-Qo=Oj@:I]McIt1'qx8;-YXgX9Cp#0)E@E:.HJ2qLoxN;MWj;;?>w@P#h^sfLLY^WM$3<$#N$(,)c0+T.Ae0a#%VI'M1NYZeZNt1'"
    "G(K0M368rLL4lr*JopGMj>?uu)*M*MqC9s$Y_Y)4,fIs-Kg?k*GgLT.[MR%+*P8ib5;*>5-U=;?a^f;-?:cG-=1Yc-SI,G#]Y(b?BSE-Z-.wgt&/moL;%VdMu6?l$G0fX-o#T)4SbDnE"
    "(sp=G@^:RE?BPEd=]a19F-(3CQe./1C*@29*U3X]I$)x5H2pS8j-=2CI>+?$qaNrL3D%'MVKJR0H$(,)NZ)F3K.U-M.9fX-M5eQMf=&SupFJPuaAmY9'[3>d]e?)N(WZL-f(3u.B+eX-"
    "e@*.HCX;.VIXC_&6`NrLNk&3M3NsFr[/D8#%,>>#5W&L9pl7R3QUkD#;do&]JM^(%Gum8.Q4N1KIJX,MeV$j#[?r]$e:QLM7VnO(8%%H3Is-qLjsT=Y>K]b-3@kWh5N0<-C4SRM.x2$#"
    "hHJH;7m9^#a7SbIi40B#TF1qLYx7DCGDh*IOnaE[kY]d=FsK]=:hC_]K>c;-u$@q$p6j;-2'eMM#b2'%dI:0CUDLEdu(KgL94N$#JnYl$eR$]If,#A#<o-eQW[kD#&30@#B]u5D*sPT%"
    "-%lA#=Dd-.HDD:M+F#I#V_%i*7g9gLIlm##Z3i4BE;G)4`g.n$s*IXJ4_0a#HTTm8,N@&GSRvt:u>$2'f*n=G7G53rSJo3%i&K'd;1uo7pWdURs%1^urMbA#*.T=:(KF&#tdo;-V'b+."
    "*0jFNOD`8.PImO(jZK)+XpHB6w;0W$f.mHk3FF3N&=&#+5-^,M?J?##GvTWfnc(B#dJ*<.DVg/)u,0W-Qdm^7Ix<N.$XWECG,S^#@#/Bdul&$$^Jr@C=9/lKM,k>-O)x?9@+9&$5Y0%%"
    "IQ4lVK'.*.`g2'Mg:8DClFa7unOiTdB#$emo5UWJTf;;?7`###(0V;.^/g;-PveXCDlBwKwBDe#8:xKGn#Me$Gf/a3&,###)XQ;-O*oo%K2w%+ESXV-3J_p$xQ6(>gZ'^#d#3^#VTWb$"
    ",sp=GFrrHCHRhGMvQl%MU2BHu,_GJ8.IF&#cH/<-qvpd%GJsc)uW)x5;sbn%(MdW-7=&L>kjK?nJ<[q7lsfG3,jtO%c,p[6F7ie$q<0-4L2J3%xn>,Mc#7a.5rC$#X.^c0ZH/i)3fSA$"
    "ZWER8kl]G3_CmGD&#Xw9.UdGMucRf$GM_<-R#iL.%$nO(%?TW-_:WpBG[Ld-2x6Qq$aC-%CT/,N(p9h$aLr<-pIOn-0q2<J'XZa$9o0N(n$3Z6oj[*@fp%U:eg'B#r#Aq:8*w<(C2+M?"
    "bpE>d$SS2`T/SS[muFk/[?O&#ZU5g)o.n,4ln`:8Mps)#7bkD#FH*;QkYg;-a`@[=L3O2Cf@6<Aj:Aou7S2b&D7es-J#=2Cm&[#6#n@X-g8NB#14;S$^>#q9e)7AIwm'-%k/@8#`Wp%4"
    "Y9</MN/T9C(fW9Ct&HouA@G-&kKbA#mf3j.3`v[-^,/b-f-m--s)_^uC1Ar%?<&=#:hFJ(T+pA#G^Re-0;'.-RRt`#-q//1n3C&uk,G6uD)WI.^OVS%a&&Z$5AR2CTsn<Lu'.ktKJS>-"
    "SUZL-^274>,O=&uA0hutm$e'8RWbA#>s5?d]@'-u_-NSt&K%f$X#juLNqD#M=#AX-N,.qLFjPl$?tH,MtQ&;QA]>d$EquA#xSG_/hUVfQ9?#&#^4a`#^?qb>(VT/)//dg$uic&#v&U'#"
    "0WH(#@2<)#Pc/*#a=#+#qnl+#+I`,#;$S-#KTF.#xh)Q.:?(B#Hen;-^Y#<-3q,[>h[k]u%kLEd#H,W-Kx4oA*f]q)vL-?$/h?%=733mAE4=x>ViX$KF+G&#'Li8&9(HpK%v>&#ma.-#"
    ":_,3#=PC7#v6t9#&;Bv&2f)<-?s&n$7^,g)+7Yc&r$xL,Hj8a*C7G&bFZvD-oj-mN<n$&OGF;=-7Lm<-]>=b*4mUa*@4:Ea0ZT8%2'oa*iY;HM&_Xg$Te$La%m<^-A`V%'C4ic>:'?-P"
    "`AmD%C7dq7G_?T.:vv(#JPj)#Z+^*#k[P+#%7D,#5h7-#EB+.#Ust.#fMh/#*>Ph8l/Z<%kAY)4TmwiLWaZ)418nJ(Z9k=.cMi=./(;T._.OF3n^R^O3;l0MF%)u&)N;X%C7iID9A5B?"
    "Q3(B[;k^KO=##)%>nL(&ZMS&GKMUS[:4NO9Dh'B#?>NgR<x0`$_&###BA';Q#`;5&pIXw5?EwY6d;:DC3tuFr6Y;W-<5QpgFTx>-C^=Z-j.-)/F,>>#jAHJr$efM*vaT_-WnV:;Ke$HM"
    ":AGGM9@[m%D[>Au<'v&Qe0]F%t],N(/+BtUDYBqV`$/W-E.G+lRaH2(-w8W-UXlZg@`9X6;TxVNBlS9CL=7#PrKtJ-[obkLG].Pd$tn=GGO9DtW_Ma-&I<_S-?^M#wh#gLRJ@3'_DN&#"
    "<;v5/5*XS%9/`r?`5<Z6gRs`#UL,^#XTHn<38R2Cge3IQ^:>nCH`(A&7H%B#,=k63<hxY6Dt6E.<-B=l>oXs?/9q3CFRA;?Lerr$rUkw51j</M+JAouP3+Aua7ENj>,@./;hFJ(YXd--"
    "lH&o(F3C(D#$n--r%>/MOE#I#;@,w.Q8?Sn2^a*.slRfL`bY*$vH*1#<,/b-l[s[G`99Z-mGpkLKVO4%rsx)[8AM-%m[eG*jM&gLD/42BpQSCO'7F2C:.TV-?ui,t_,i9&HW,M7[KLwK"
    "rvYk*-EF:.sI,$%<H+Hk)CGGMwu1W-gZwwpt4+wpbf%&4*R2NNU[-lLhIXD#&A#dM382G`fEP<%nf9dMA/TA-'*q/+?&p(%6RwM9RKbqCCg]$M8DGK(S5YY#2Vo.CjbP;-g3el/7:8N9"
    "(^LZ%rQBC-IZDa--TdEnF[+n&&gnA#(^,EQbbcW%V]o.C_%r;-@lb_.Ip@+4^YQl&CTWe?Z$UfL0O&;Q%Pq$&am2^#u6kZ%>hv9D-Hqj0?L7%#;3@#6Y&lA#nHTw9%Ca8.Lq#Grg2)3C"
    "7,NSt7pJe?P?0etZ>SS[Cd?D*efo?9^<-gLw37g)JH/i)?uq[6+#iD3KTZ[$UK<gLqo?=lBul=G+6*gL34)=-BV2a-E3I.DB'3Osg+($&ia$)*+&+XCsF$##a1L%6$cCP/mX5H3J*hfL"
    "86)B#d9u`#>HF:.<#G:.cVjfL7fcgL=#V3CFE^r*vY(39x,jEIN8V2C5gQ_Q#xSfL)D#I#3J#d$Z]6<-amCx$P55&S(*x9.tf6Z6waLhL3JsI3#07`G%`Th5NF7W-QJ+Xh8.l$BS%ka%"
    ",BxeDHE-$$vH*1#I64f*i2cX->-M$B^kY)4Csb([CG>f*[oAW-/T$@fF:I]M4s7DCHwX3FPY0_EP$&oSqrq%.dLchS$rJfL:K2G`+3%#,t2LFach%njk^UhGSAG`%KU1N(n_TIVw<Kt:"
    "^98W-8_&<8D3,2$xVZ[$j3.lBP-tY-t3gE&5Tw>$UFgBfgDt2;-gCD3Vl*F3:u-lX=(i;-;=2Bq?kFrLEZ`X-UJFhGRdgI;W@#1OOWUX%Rn.W-OnAk=ue-lLrHA+4b_Y1%YI;o8B5h'@"
    "to`t%'s_>G'qhP00APu&_<MgL%kKcGJVW-?THh_G)Q)c<=W(C&VgaGM5>?E&<*2GMcK>e#,Xf29H=nXHr-^*@]BsZ^[<l8I4U,qr;M_m8p4M4BhjHdb,i=j$<3$DecAdJ(8=Hg)rD8n$"
    "ew(x55=$f=F+F$8&)EVnu8xcMWOZY#kSuIU+A_VngPU7)Qg@#6aq8Z.v?1a#9[5[.Rm`'4^?:U9^c<>d=so(Na/I1KI*F>d3QV9r:LI`%]Yo.Cp*3G`O4b`*_]@f*=u6<-c(#o$,o?X-"
    "<XYw5JWV%%gHDHu]6&tutf4s1$UY%+38C=l(fW9CVxIfLF.Xj.-Mc##YgJ?0bEX&##$nO(xQj-$@$/i)&<5p82F5s.=SDs-e?.lLJt.PdUx5Pd[47DCBL#GrYfo$'IKk%MlSAoua>(nu"
    "$1BiuKgMY$Yic;-HA^2`:$>Snd$$>FmoM*&lHBn)])QJ(gSvw*sXTW-2JDtUx^W1#Q5Y%#Kea1#S$V?#,PsD#^LEL#o>]P#U=[S#*0F]#%5Uf#=U3p#>mT#$?(w,$1JU3$OOCV$.7>B$"
    ".?MK$=uXq$0+5^$Ofdc$9=*l$`h$8%L[1$%&q(,%i8;O%h[f=%jq]E%A$BM%DS2W%E@s`%e%Lf%xGVk%ntQ4&<[Lv%rWu'&UTG/&4096&Z)b=&HpwD&l;VK&H;)S&uY^Y&RV0b&p`@h&"
    "]R+q&Kexx&w=?('bY5)M)s8=umhJ<?/o%*a)w6b*-#uaE`_`aj+/u#uocT0:.oB*N`G*td#5H[#L:tB47tg<?5[pZcRc7[>nLlNg&Zm*3_-2[Y7'mh(M:.CFIh]*aGdVI)9TenA32u6]"
    "I8XC+h&27J:8<Un],>1:%-W[YsYg$u#7P=?#?xtZ?'-P'jDeOKFi87oc>Y1:o-C+W;e^o&/CT%>s-lIV9FOV%IGg1Cn@:of$%r%,v,$>H[HY+aQvri(G)boAS$,J`xm'p/>Iv+NJ8`%l"
    "g>YP9qe+2U5l.^#W4VJDj5@Dbn[Q&,&dI8Jx.B>dpU$E+$QrVIV7^Vn3OP&>M73p]pbm&,(Em>Hx`eDb$+w&,d(UpAS1.W[ArD&um9Zd3P6JdNoBDpoEH7?\?GpNdWun=^lH/%q/,_JKD"
    "K&XdW-Z0Kr/]wj:59adW;/:Kr-ppW7/L:3UQ-Uw$4,oWIDqwpfekY-3#5[?QGxMEtr9X-ET2D-jw2'@6U7*X[Kx)esI$Qe<=wh3UGl2es52QX71xAXR;#CkqKhKL;;XcqSKxDeslX74C"
    "<(2@d_k-f3)mleN]W4:f]('S0ms]LMREtqfq9V.3][U:JgPvkhJ<0G+,.ErAtn%`YUAK:oB%hr&C8C`5DwNrAC@k@QNloL`IZ<xmVNXf*U%.A6RJ*GF8*PxZf3G(uYHv.<p<p:](^3#%"
    "lghl:GL0AQ1+GfjuRF;/oTNSKZ-/Ad@7V5(faZM;YQ;;SIA$5qbH-m:Yv<5Ut>W#%<koSB4N`S^Dhj/*N>jGFg8dSgR<t/*.r2Z@&%J)Yj3YGtOD1H43)`/NmW/acD7VT'LQu5C<Nm;]"
    "2&0$%24/<A:TVs]:JpZ%(TNN;xnn)Y^fUsoOu.$7I.FHOlwo/sn<P<8[K`ZRKbGNi##N*,9;@0N?n)*lQVQ*57N9tJgR(n_BJ0C$j$5[7,C'bYbATBm3.C[.q[Y*GTfX6_X6kn(Tjq6C"
    "DhAhW(LpNr0/Yh<nDXtSfA9bl7xFC-_WT[@4+s0WS0i0j^=JC69$ACHXM/=]0e5OrdCsb5[q,+PlFEUp$</o:$j'uSWHehjKQ]71=Ht[IG1^Ug?:uC-c&DuA:oQ7UlSpbl<lZ+>a4U7_"
    "/f9D-=H9]IKU:oh1;ji*ejG]@V#wCZFvnItPk8D?%wb+cit;&%JfPP;fF/8LtRWuoDxQJ=ZGS][iN'W'sV.vA5j?Dd.Z4p:s982a+Nq8(%&xVB5^qccA.l,,n^.dPDpWJtP.i2<P6:jW"
    "w/69(wV%?A1->ja^8:91MTOdGpxQ&n$V333FU=QVFLO,iOp`*B81..B$QmLB.-+7BwCN:BrfX?B9WDEBUv,hBd-`PB^NiXBq2m`B`P).Ch9%mBx'exBtbh)CZae2CYMO;C-KMACGETcC"
    ".vgOC&AqWC^Uh`CJ$rhCWrBsCW0e&DkG00DMB.6DBLj:DnU$ADF$.IDu6PODq>`XDWu@'E_t4kDr4-pD#'DtDd=hwDl[OCE]*P0EqIX;EiSiAERb$EEDi`IErLdPEA>OVE:<#[E>=L`E"
    "8,ddE1hhhE$AZlEo&`pEl$3uEpCF?F*jG)F;Nw.FTKu4FYIH9FdunYFn8^CF#[hHF.(sMF;V9SFH#DXFVTa^FahXcFdr>hFi27mFn<sqFxtT<GD7o'Gd4m-G/J94GSiwUG&8MAGKMpGG"
    "soNNG<mLTGVa8ZGmsZaG4X4gGMIvlGjh^8H0P.#H[42*HI$s2H(h;UHaN6AH:K_HHha+OHP;GWHY']bHi>*fHWb`iHB#.mH/FdpHwxUtH`ZENIIMX_Iw1^cInmbgI*hi2J.]wqIAMcwI"
    "4e0%J/%)*JVRp0J#`*7JML@>JY#1HJNx-QJ(WZ]J`Zr.Kqr4sJ#%D&K^*2IKJBv5K0>s>KxbV8#-tw5'rkC>#6%Ke$,<,F%/1L^#4=35&=_h#$JTGJ(.+_>$W0mS/3LHZ$bZXc20=?v$"
    "q[3v6<T0j(ua$3:'5>>#o=<X(VE0A=01L^#B=PfC1:h#$ol#;H2C-?$-]%GV3LHZ$G#Fl]0=?v$nRS:fh4v1F@-HAJ@AwRC#*N=B+V?1<Vl-)OdhEA-<9RL2=`$_>->hoDRB*g2<b/FO"
    "h+J/22^7JCXUh%FPC-AFi)u4Ji_)d3&qpMCp5(@'i,w1B2^Eg;l(q(<Z]-^5ZB0Z1hPT,3es(L5'J_/-*]'@BERl?BCXLwBcJ>_/wU7X1N5l-$Ur4^#vC58.ac$)3oAHpK:LuDNK>@4$"
    "'->>#(8P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#eEbA#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#/QHC#3^ZC#7jmC#;v)D#"
    "?,<D#C8ND#GDaD#KPsD#O]/E#SiAE#WuSE#[+gE#`7#F#dC5F#hOGF#l[YF#phlF#tt(G#x*;G#&7MG#*C`G#.OrG#2[.H#6h@H#:tRH#>*fH#B6xH#FB4I#JNFI#NZXI#RgkI#Vs'J#"
    "Z):J#_5LJ#cA_J#gMqJ#kY-K#of?K#srQK#w(eK#%5wK#)A3L#-MEL#1YWL#5fjL#9r&M#=(9M#A4KM#E@^M#ILpM#MX,N#Qe>N#UqPN#Y'dN#^3vN#b?2O#fKDO#jWVO#ndiO#rp%P#"
    "v&8P#$3JP#(?]P#,KoP#0W+Q#4d=Q#8pOQ#<&cQ#@2uQ#D>1R#HJCR#LVUR#PchR#To$S#X%7S#]1IS#a=[S#eInS#iU*T#mb<T#qnNT#u$bT##1tT#'=0U#+IBU#/UTU#3bgU#7n#V#"
    ";$6V#?0HV#C<ZV#GHmV#KT)W#Oa;W#SmMW#W#aW#[/sW#`;/X#dGAX#hSSX#l`fX#plxX#tx4Y#x.GY#'>YY#*GlY#.S(Z#2`:Z#6lLZ#:x_Z#>.rZ#B:.[#FF@[#JRR[#N_e[#Rkw[#"
    "Vw3]#Z-F]#_9X]#cEk]#gQ'^#k^9^#ojK^#sv^^#w,q^#%9-_#)E?_#-QQ_#1^d_#5jv_#9v2`#=,E`#A8W`#EDj`#IP&a#M]8a#QiJa#Uu]a#Y+pa#^7,b#bC>b#fOPb#j[cb#nhub#"
    "rt1c#v*Dc#$7Vc#(Cic#,O%d#0[7d#4hId#8t[d#<*od#@6+e#DB=e#HNOe#LZbe#Pgte#Ts0f#X)Cf#]5Uf#aAhf#eM$g#iY6g#mfHg#qrZg#u(ng##5*h#'A<h#+MNh#/Yah#3fsh#"
    "7r/i#;(Bi#?4Ti#C@gi#GL#j#KX5j#OeGj#SqYj#W'mj#[3)k#`?;k#dKMk#hW`k#ldrk#pp.l#t&Al#x2Sl#&?fl#*Kxl#.W4m#2dFm#6pXm#:&lm#>2(n#B>:n#FJLn#JV_n#Ncqn#"
    "Ro-o#V%@o#Z1Ro#_=eo#cIwo#gU3p#kbEp#onWp#s$kp#w0'q#%=9q#)IKq#-U^q#1bpq#5n,r#9$?r#=0Qr#A<dr#EHvr#IT2s#MaDs#QmVs#U#js#Y/&t#^;8t#bGJt#fS]t#j`ot#"
    "nl+u#rx=u#v.Pu#$;cu#)Juu#,S1v#0`Cv#4lUv#8xhv#<.%w#@:7w#DFIw#HR[w#L_nw#Pk*x#Tw<x#X-Ox#]9bx#aEtx#eQ0#$i^B#$mjT#$qvg#$u,$$$#96$$'EH$$+QZ$$/^m$$"
    "3j)%$7v;%$;,N%$?8a%$CDs%$GP/&$K]A&$OiS&$Suf&$W+#'$[75'$`CG'$dOY'$h[l'$lh(($pt:($t*M($x6`($&Cr($*O.)$.[@)$2hR)$6te)$:*x)$>64*$BBF*$FNX*$JZk*$"
    "Ng'+$Rs9+$V)L+$Z5_+$_Aq+$cM-,$gY?,$kfQ,$ord,$s(w,$w43-$%AE-$)MW-$-Yj-$1f&.$5r8.$9(K.$=4^.$A@p.$EL,/$IX>/$MeP/$Qqc/$U'v/$Y320$^?D0$bKV0$fWi0$"
    "jd%1$np71$r&J1$v2]1$$?o1$(K+2$,W=2$0dO2$4pb2$8&u2$<213$@>C3$DJU3$HVh3$Lc$4$Po64$T%I4$X1[4$]=n4$aI*5$eU<5$ibN5$mna5$q$t5$u006$#=B6$'IT6$+Ug6$"
    "/b#7$3n57$7$H7$;0Z7$?<m7$CH)8$GT;8$KaM8$Om`8$S#s8$W//9$[;A9$`GS9$dSf9$h`x9$ll4:$pxF:$t.Y:$x:l:$&G(;$+V:;$.`L;$2l_;$6xq;$:..<$>:@<$BFR<$FRe<$"
    "J_w<$Nk3=$RwE=$V-X=$Z9k=$_E'>$cQ9>$g^K>$kj^>$ovp>$s,-?$w8?\?$%EQ?$)Qd?$-^v?$1j2@$5vD@$9,W@$=8j@$AD&A$EP8A$I]JA$Mi]A$QuoA$U+,B$Y7>B$^CPB$bOcB$"
    "f[uB$jh1C$ntCC$r*VC$v6iC$$C%D$(O7D$,[ID$r+xEF#6hC%KGRR2<Cn-NXn).NYt2.NZ$<.N[*E.N]0N.N^6W.N_<a.N`Bj.Nhs]/Ni#g/Nj)p/Nk/#0Nl5,0Nl2pjMYmdL2Wa&g2"
    "b4hwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK.@RhMw'3i2b4hwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK"
    "(giwK(giwK/CRhMx-<i2b4hwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK0FRhM#4Ei2b4hwK*miwK*miwK*miwK*miwK*miwK*miwK*miwK"
    "*miwK*miwK*miwK*miwK*miwK*miwK1IRhM$:Ni2b4hwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK;hRhM&wSj2YA8FI-E:FI-E:FI-E:FI-E:FI"
    "-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI.KCFIZ+]88rX:d-$vQF%6fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN888usS8rX:d-%vQF%7ojS8"
    "7ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS89(9p8rX:d-&vQF%8x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p8"
    ":1T59rX:d-'vQF%9+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K59;:pP9rX:d-(vQF%:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9"
    ":4gP9:4gP9:4gP9:4gP9%'4g2?Fwq$-?eh2$'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-5ZiwK+7RhMtkmh2b4hwK"
    "%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK,NTs-=@[hM&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N"
    "&p).N<V<7%0Q*i2&3bG3hZ;Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g23^9W-9giwKSAY-v2Tt;-CU<7%^n8Ks"
    "Z/je3=3@Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9te/g2O0sZ9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%"
    "^n8Ksl$SR9s_&g22Tt;-CU<7%2eN.N<V<7%^n8Ks]AJF4[o(N9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ks^Jfb4hZ;Ksl$SR9s_&g2"
    "2Tt;-3JU5%M20?nl$SR9c_&g2x#t;-3JU5%M20?nl$SR9de/g2hjf(%(qvJao*&;8_G:&5q4`A5pl1e$-viwKo7n>[_Bj.N[vf(%(;SdFje`U8@gA,3Qdo;-dmf(%x^vJai[D:8;hA,3"
    "3cq;-e$22%grd>[hR)u7+hA,32cq;-IWWU-&.iH-bX>1%&wDKam*]R9;hA,3e>p;-w+M*%f^_&vlw@79&iA,31Tt;-CR<7%^k/Kslt.r8eq]G3v#t;-3DU5%M,t>nXmdL2KSc;-PGW/%"
    "4M2Wfa`=(57p0W-%X:&vhX2u7hufG3Mqjr$HnAV8'ufG3Ntjr$HnAV8'ufG3FCwq$u>h2DiX2u79p]G3O>Hm/s6L=%KGRR2C0*q-BPSdFaiXC5s8YA5s8YA5E]ZA5s8YA5s8YA5s8YA5"
    "s8YA5Rg+W-8-:FIJUr;-^I>1%Gbju7dn]G3,<t;-?+I6%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%?W^/N&QU5%QDB?nkeM:8"
    "GEf;-7PU5%QDB?npBF4:eq]G3$$t;-lDY)%QDB?npBF4:eq]G3$$t;-qb+w$5WMW/jbM:8No]G3$$t;-7PU5%QDB?npBF4:fwfG3`BS<:eq]G3$$t;-7PU5%vDq2DlniU8*;Z/:eq]G3"
    "$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%0s''do9+o9E$#d3iVp;-'P@+%Aj,KNkhV:8[),d3Dnjr$?.nt7'xxc3$;6W-?G9RN?.nt7((,d3Dnjr$?.nt7'xxc3Vm4W-t?:&vo?4o9"
    "C),d3[F#l$?.nt7d](e3VV)WH`Yce3iuQF%<,(W-JnRpKY&3i2%w*g2%w*g2aA,W-9^iwK7mt;-K'08%fWL&vZ&3i2aA,W-9^iwKM6B?7Y&3i2%w*g25=%eFp?4o9B##d3Udo;-j#g(%"
    ".YxdFp?4o9B##d3Udo;-j#g(%.YxdFlqrU8s##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3=%r;-QGW/%.YxdFp?4o9C),d3wxt19E$#d3Udo;-j#g(%.YxdF"
    "p?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9C),d3trL*%Udo;-j#g(%.YxdFp?4o9B##d3"
    "Udo;-j#g(%.YxdFn-S79%$#d3Udo;-j#g(%.YxdFn-S79=$#d3NUr;-j#g(%/jk&dp?4o9B##d3Udo;-j#g(%.YxdFm$8r8(%#d38mt;-L*08%In/;8B)T&vm$8r8(%#d39v9W-R0SpK"
    "t)08%g^U&vm$8r8(%#d38mt;-L*08%g^U&vm$8r8n$#d3)E9W-v=;pplN<7%_k&Ksm$8r8v$#d3fHs;-$9o3%.db&dm$8r8E$#d3Unr;-j-22%.db&dm$8r8E$#d3E=r;-YxJ0%t'Yp^"
    "m$8r85$#d3E=r;-YxJ0%o8I'vumgL;)+,d3/(9X;(%#d3@mt;-TB08%o8I'vn'8r8kCvM9(%#d3:mt;-TB08%qVj?%p?4o9/xxc3B)l;-TB08%UdYW8thF59U[BqpumgL;n$#d30<t;-"
    "D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;o*,d33&M*%:GjwKa4m;-vtF<%MdfpK[+c4%Tk[E5x':oD''o2V6+pGM/u779KN%gLlA^P9luZw'"
    "<h4:2&5-W-w?QWSkt779,d.F%*>L,3-+of$AL/79A.,F%l$1F%9d*F7V&#d3OBLkFaPYD4fYu`4_J:&523VR<o<#x',@n-NWhv-N(<QD-R<QD-R<QD-R<QD-R<QD-R<QD-R<QD-R<QD-"
    "S?QD-l?TOM1jZL2B-f;-gOK0%Sa&g2e5-'?@nC*NgFwq$@Cjq8L/,F%=`e&d@nC*N6J#l$p-G59rUJe4VgZL2ODxfLtjv^8<cA,3tQ2gLk)TNM*iv-Nnjv-N9s779;kJ,3?Cwq$'Pp9V"
    "Z[sJs?e(eM?aVU8]xZw'C;c;-gsJc$En44:5iJ,3o[fZ$sFDF7kO#gL;=/W86vZw'+h-na4ZF,3-C*W-l>7q9HxOp7vTZHdoS'g2.M'gLs`Cq7,cA,37qn;-Xu/k$Db]R91.,F%Y[xpK"
    "@nC*N4[bg$JBUL;V-,F%MMG,36C$gLD7ev:K#^,3<.7LM-;+o95iJ,3],6X$@[93i(M#<-LqE884xZw'KA:j9thA,3@v3gLtxlm9^%[w'`t6q9[>8+4las;-+db4%0c+eFX-7r7`(>)4"
    "Bt$gL60Wb$f@pK<?e(eMnOX4:_vZw'Uw@)4Bt$gLib-q$rIf-d2Qnh2W+D)4/Xn;-3>U5%M&b>nln`:8g->)4Url;-3>U5%M&b>nln`:8g->)4u,9W-:^iwK&2JdFZ,<i2W-/WSkeDu7"
    "l,>)4u'n;-4E[#%21$?IrNX4:viJ,3RQ#9%Bt$gL4v5X$4GvVfrNX4:gi4R*AOwK<@nC*N?^'`$fLhwKBt$gLQoXl8+,>)46[*W--_JWArNX4:_vZw'tsj;-J]'`$fLhwKBt$gLtI'`$"
    "Cs5KNrNX4:gi4R*QFh&QrNX4:gi4R*#,h;-J]'`$I0Y4:V-,F%2SZ/:_vZw'=v^/:_vZw'`8g;-J]'`$a:,3Dkn`:86->)4Bt$gLQN`m$f@pK<[>8+4xh#gLX+09:>vZw'+J`32@nC*N"
    "TFY[$E+V_Axh#gL.[)'%xh#gL60iF:>vZw'hGY/:>vZw'cp&Q8#,>)48M-W-$?04V8=P)NKc)'%h>04VX-7r7x[.4VPRCq7x[.4V@nC*NLVUh$V;@WAp3AV8e5P)4ANO&dp<]r8@.G)4"
    "B=r3<-iJ,3^gBW$E[/:DUDxfLn3Vn;vw],3GORhMJG:p$5EMR<0?*gL@kb5:<xZw'_.:3i@nC*NE=Sn$8bRkXvd)gL,K^s$oK/W-fJB?n8=P)N5?q-%ajE3V[>8+45M'gLL,%v8+,>)4"
    "X](W-,<I'v8=P)NA$Go$Hl3dX3BWb$I0Y4:8jJ,3Igd.%g?^Ks?e(eM3vi:8e5YD4iVp;-aI>1%GXNu7q1YD42cq;-LG4_$S2W+=6-,F%wFoP9d40k$%@;*>l/;3Vk'fn9$BpK2b78(#"
    "$,Guu(SUV$D.W&*E%###"};
static_assert(Terminuz14_compressed_data_base85.size() == 53080);

// File: 'Terminuz20.ttf' (89584 bytes)
// Exported using binary_to_compressed_c.cpp
constexpr std::string_view Terminuz20_compressed_data_base85{
    "7])#######s'J8q'/###[),##.f1$#Q6>##%[n42Q0M0t^.>>#)jNC4aNV=BGQ`5#GLJM'%xmo%X`(*Hlme+M*/fo@*vgS/#-0%Jjv0iCpEKe*2t`]+;/d<B3.:4DNs.>-1@pV-TT$=("
    "*m)[$-hG<-q*.m/<_[FH0<m3@i_>E6tD/2'n2JuBavQY1xU^`3YYqr$sJ[^I#(0M#w4S>-oU+41+>00FdlDo(<8VY5#)DI2P%S+HGE5i6TJ7;6WNOV-hg]$2)PUV$:*QlAeI1=#1cZ&M"
    "HU,VdFZwVM[p;E4(uxXu01]5%nCi9../5##(%_B#j4'T7=#'##JV(AMAq:eM?]Gs-VpLfL`lD;?<E#)3gt&/-3FU#$YL5L,RV:;$eauG)&D.d*,.h#$C4xH#s0)`#3%VZ#%/5##tn_Y#"
    "WGUoI@N<1#7U?1<^Pj)#Q%O(NdD(T7kA<'#%4GCiU4FGMi#R&#S7,HMjW#<-61^gL<(i%M5k^KO,',k-Xl<qM]$hw0[D':)#$5F%4GMx0lJMx9+fbA#BL7.6r2M.$S0ex'M3]9MS-SU3"
    "7mV`/>S]R<vV'G@3Z(xgu%,x'm6c`]0EnA#ktX@&YYXp9>-jYMKE^4Mrxhq7uOF&#H_5F%*T4:)ql'l4KuGp76&IW&,9,LNb&p,8,O2F7R5Kp7^Q9xTc-:=D/8(:)wn8H7JvZ#:_ep-V"
    "N4s.id),p'3&T)Nf5Za&h7W&dR^_=%BsnJsLm3g&kg+KaUxo7QGn-V+bN$bPM>UE)39A?.(H@GW1,80)Ge<9.<B>Sn:B]p75*e>Qj^B;?rb,gL'?Os7.kf2`Sn*G%tF-Q8uE@`&9xep'"
    ")M]('qM?G7.'wwBpO#l=od3`JlMY%(=3F;)o&cpTI(iE%N;v9Rf(^<8TWf>eQ[?'8q&OxB4pi`]jkT.vc<wx'9(EwPZ(&8VlNo<[&j*(Q6*-(Q69K9[2V0]*Z3hpp[:?i&fDuJ:9P-db"
    "WB+d8_d:?@'k@V'p4;'6=.xR&KNT0:/gj#1Hfj&69`G$6wrPvP1O9h:eqb_/s'*<-iM#l$:T0ZPa;ij:ZnO?I7TYHOInMr*1a*LN;B=P9m5s&-5GY]9p@XU*>*sA#h1e.%wS^;)'R_x0"
    "W?Aq7P).x0pm+L<x`5M:X>[w',6B2#`L4.$[?l6#)(MT.?ww%#-4ip.Qwrr$oFEX(Zl14+;10.$+[^X8N=6##*KBL5d<<v#)<G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#"
    "NR@%#R_R%#Vke%#P#$c%:6TY,cL5;-gelr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4A1OY5EI0;6Ibgr6M$HS7Q<)58UT`l8Ym@M9^/x.:bGXf:f`9G;"
    "jxp(<n:Q`<rR2A=vkix=$.JY>(F+;?,_br?0wBS@49$5A8QZlANdc)E/M<1#6fa1#M_,3#dK;4#iQD4#nWM4#spr4#xv%5#&3A5#*9J5#.Kf5#5d46#7j=6#<pF6#D2l6#JVhn#e#(/#"
    "@*]-#`l60#;?)4#Tate#%<L/#:285#F'vj#<<hf#,ES5#-/'sL69gV-I55I$0wH.#.('2#/,c6#[tI-#7a`O#k=fl#.[O.#ur?0#R-e0#?b>N#M[e[#]5Uf#mfHg#eAL/##t8*#rtm(#"
    "Yj&i#V(?tL=RF&#o)>SIm/loI'Aj+M1`NfL9Lc%OA3[uPA'%>PM&ToR9C6=R[%i.U[,7fq.qKS@CKPV?lVVq)%6io@.X4;?j&OSR_Ll%ln>wYl4g7-#E?O&#oGhJ#+YWL#HLpM#'51n#"
    "vo)`#^6Vc#1b@d#,X5j#WCno#5'Q7$2lUv#_>?$$@@p.$D$t5$*>FE$o8oL$58CN$rA)S$5v.u,N?>n,@eIp,h9;<-FZF>-V5:?-r.[@-/fWA-wNeH-c`SM-#d.X-H)1Z-w4Hg-oLlj-"
    "@'2s-vB`s-.*ft-K#1v-/qP$./)uB.&*o9.IG[h.HcZs.gPiw.cm>(/8ZM)/fF1,/$lh,/4F[-/@k<./N9t./j`;L/Aqu;/h1C?/9V$@/D4_3vNoe+M5I$##fEX&#k>9sL6xWuLuT*O#"
    "L#4]#1x9S-bqi.0T=eo#2q>7$V':S-jCWl%+PADWtoO##]@NGV/dwrZ^fjlf]TV`sLKp1_>F?VeZ'O&#:h@&,u>lKl&*WA5kbvP'<OG>Q%=kofWf+Z$t2q)+WV)T/`S?pAfbnKlE?(j_"
    "`BXKlwJMW%>%h2L:[Y>QSMDMlf]qJiCP;Ll;Np,EwW?vl&$&##sPl@bGI;f_2/b4]Z/o:QlC4A+3;k[tfjZCsoDc@bLgErQ#/wIL1*Lo@_?u:67ZO=l3Bo[k4)oR[r]qLKfjxRIKAwLB"
    "10_4A[DT%Wr*PoQ^t*JBKr1P@EGp7?/t_%<#+g+:5.(5%M(t[j=`>iJr>m@ELItqu^wG:lE,fITk9-iSa@S7QJs^@Nrd4fB$:5c:1po_Vn90=t($7.TVvZb_78>@WqjDFU_:87QNGvtO"
    "BT'%N:$fbL0CMIKsY`XFM>+i&+)(Ip_a3Lo4mopR)3A>#WPg@k@mJa#M0gfLZD48RG1&)#C:f:dA<R;-aG#s$/G###Hax9.+rJu.=5mg)<k=DCF$juL2.``j[]V9C$XWECZWG2CoK.q$"
    ">bEM05Rf#6P]p[6&`c8.Jl*F3KhY<.AA0N(Rt;8.jSCI$Qt`8.Pf%&4pmPh#*cq0#;KLh>9[&K)+Ps6*60sE@?b<>d:22X-:]Zt(SF@>d=n`U1$-#JU(dlSI+dYc2N-sKG@iL%6i/Oh#"
    "haA)46_/g)3S2g)KHZ;.lcpG3FKC_&d`MD3W2XJ-VG#W-YPQhGIDB-dG:e<CKprHCO`_).lUnYMfH1tL61TgL'RafLafcgL>92G`5:c&#V(f+MuI`>$Cme%#hQk&#x,_'#79@+4fG:g)"
    "Rcs+M$=2=-&qP11-&[J(x1tM(92B+4M(m<-$AX%8b,ge$Eif]uD'K0M2T#I#*u[*$[UC1#MOHwKCf%lu0NR>MxPR%+AVJ&#wh^#$CDUhLL>4)#`jG9%Ts+H3M'q+Ma.;hL-dj=.%iDu7"
    ",(>)4j96C&RbTfL;G`.MVekD#6o0N(or[NNdT]F--g^%NwP&;Q7(IW-965B-K)([-Xvk=1tWCou$21[^218G`LQ*##Qe./1j^B;?fuUP&_D,H3,vQJ(OdTg)SW0/1#gj>$qu9)M>'0/1"
    "7)1G`QwJ)+/[HgL*S8$M&/-^$)-+)8wZZ-Hl<J>dnLj[KV9]kFxn.g),BV;.HNfG3MY=gLQ)#A#Ln4a8;j5/1#WR&#Z_f70/e&.$;A$(#[:q)55E`hLpB`2%DJsc)-P`3=6he;-P;iP-"
    "sQkr.a0YD#68#4O$MO]u?umi$?b(C&wK`*$V^ksP5wM#MsA#I#JkrP'^(=Z-HtTfL,?fX-@[kD#d9M.MgmaIhm6Bq#:u/kL&1PD3C4G>#C;Dk#S0_'#KMOV-V$nO(VU5B#6<:DChgKe-"
    "*)k^#Z[ah#kB=gL3ct`#@Djc)uE(tL'/'-u'#uBCti8k2a6YY#D0@8#Krgo.:?(B#T<u0#bw5'#Jg^>$0jVr7n7XKl[98<-x+?-.n-q+M&Q&;Q@GTBox/S;-GRh,M^`we-0]md+ejA%#"
    "e>(:8Bug>$>0l,//j4Z6ht2t_:hWN(JV/a#?9V-M@a+p7G7P'ARIr@Ch0?XC_0h?8476?$C8dgt[,[n8.a*wpdQ`duRwlW-;Ow<1CExNFCDO?g8gEZ$&L?IM(kg%%$0UZ6:(j=.MtRpK"
    "QDL[-x.,R<kQbOM7,J6MJdJN-+nAN-PU8j-%16b[djbp.6P&d)UCPhY9^BN(aQ3Z68Tx;N.);9../B=lwG[]uD#1XC`fv1'k13acZC#I#9cZ6P]g9gM:AGGMQG]v$t`0i)Qp`'4Xc_e$"
    "U,O/Mr+ImLgWAG9,-F2C[_;6M;P.PdL^q-$B8sBA3lZW-tKh?B[9@+4<g]E[QnvY6=#)P-a_wb%9m`'4S-L0YB8sBA%Ej,MBpt0pU<+X-1.5[B`Z%32,/_e$*_?iLGsTA$6n@X-g=W/1"
    "/j4Z68gfX->c?X-9PdxLMZq[-uDsE@m9hAuI5%1C#Z1@'ArZ>d(k5/1HL=a+ow`3=?*dZ-<BF:._6;586*_#$2<wiLaeH1MUq//1W?xE7#^3^#-&mW-ePn3+(xcIh+dnA#pkUH-DH_x&"
    "?'Q'AUfsu5)V2T.BnK&$cwiYM7rO(.K'Gt76`3>di,n]uvSxG)?eE>dCF-TK/x.qLT:EJ8T#f&,rgai0hvIhLWGA+4*,FB#_EwY6q37w^,f?w^Ea8'?EnT=Y=hb?^n5FD8L8*XCv6;)N"
    "'Gt1'/1L#$Ni/gLQ;:58c`eA,JW:]$YIXD#Jax9.<6fX-vl.&4Zm$Z-=ik>$YlF(.sI.qLV;j#Mmo8gL@[4<%lqX9C:QCwKO'6F%h8W=YReZY#o_t.CZ>IGM.uQS%WKBD3D6;hLug(B#"
    "f7u,/<]mHdiwC0ukWTdOGdF?-[:C5%)WT)4NTij;&^IY/7l5/1k5Z9Cf7sZ^fX;<-av,Q)mwIFO0R#l$Xm]G3Sm+W-6FRjM@H>k-x$gMr]>SJMZF$##52xH#i7QtLZ/[##E$(,)AJsc)"
    "?4TA$RmwiLG[oW8U%CB#0+v7$DtX?^msJ<-*KL@-:oK`$NBcQ(@5o09-_jt_6]<uUv/&HM5.SS[MAGU)3,'p7JaIa+upai0snb;.%@t`92n5g)i1Me$d6lD#x(fHF8kw6/2Djc)=j%s9"
    "$KYb%L%G]80Yb?KTdpmU]&=J-_ck;.fSD8#tR0^#':*/1o/vG)uv&/1hV)F3HcK`$kXj=.cLI6/HJjD#NawiL`S7U.:>_8.@I`t-7t/kLZ1HA%Is/kL$m?=lt0O$M95C%/f+W9CbdGwK"
    "PbhGMu)i%M2V0A8*B`D+:Mf&#e?.lLjAQl.Jax9.K`4(.mG/iLuIm1/ZnHg)LSc3F'@js-3bNrL8fLH-cOUTM==a7u#:$C#9uDc8)0l3+dW`-#7Wlm/I_R%#T$(,)eBS?&sl6$6w+[J("
    "T`v[-Vw8OMM9BHul6DcuCufN-0K,W-<)PdXc=2G`M<Bh>+X(WfH2oiLZHA+4SZuk-[?g0>1MjD#_<ug./j4Z6UYxn$4<L2CGLWfL(vD#M`'41^D28v$Eq3>dP^(3)hv]%#<tP%.eRvq7"
    "lo5[-YEKZ-Pf%&4>]:8.#[op.krwFrdF'F7%7P3X#OK1KiK`gLUa;;?Z>O&d>Z7.MrxSfLeGT_-lA0I$,V5g)FGSV6vPS@93Z3>d2Ij>d]n''uqM:u8)f'B#d#juLx@kB-o$kB-B(bB-"
    "p?wmL%:sFr[)iLdtR;>dU3?Au8$_WJ?j[IM:+B+4qEwY6[DsI38=Hg)/j4Z6[YT88F8^;.3MIBM670/1=Jr@CBul=G'uP)/Z5]AC<33H49$vl$$A'617=LP81+)d*a3oN(JV/a#sQ3Z6"
    "2j5pLE65GM[FF3NJ0?-.VAc)Mex.qLLs*3.:&Aq:/ggJ)HWXs-cxpkLSKq[6v-0a#(WW_8oOt:Cb;GW-e`sTVg(k-6T7>B#]H7g)#J%'Mc`XD#Ts`'4f)L6M<Ft1'uhdQ_fr*?%s<HW8"
    "X=UkLQYnO(<v3_$Be7m8>T_K*X,4Z6Ula;.UMYS.Pq:X67%&r%K^'t-2EUhLlsg-'YTc2CQ(P;-Xue>e0_uGMP`9&$+jQ.%+=Vo$v);r9U#>3)ux1'#A5WUMNoh)4k_Y)4Kg'qrd]MeQ"
    "$6tf:sAF2CqWaRN>qt0p,bvaMk$T9C.f1Pd_#kW9*q<#-&Q)w@7_kD#HtTfL+hv&NZ2%'&/t%F@9;u0#=o(;Q0dOq;/uf7u_I,;QoD)?IcQ&%#:0Z`-Dd=*cM6H)4x^%Q8MD'N(.%9tU"
    "tl>>:Jj<>dp<ql%6]m0,<0u&::>rR<hWCG)1S0gLuTuM($2tM(^?QcM3)^$0Niwo#A&CJ#V@T8%&C7#M#^bG14,x)v=w]e$U-4&#f_SZ$=)NF3DR@H3__j=.@U]?Tg[[?T7vsI3loaZ%"
    "Z[S2C7$%t&E-^9<Fp3>dlBQ#$NjO[&7Z54M4g#pLWL9%%?nnm8'.aa,7Vob-bIos/Emo?MRS_tNk4^duKAVP-gZsn%2`%Xf8E`hL+>uM(M866/RQ)x5>9-,MYFAF-Fl#l$ZsH$8+_3>d"
    "+oX]u4(>$%,$<b$utV#IwHJEd_T_d28,>>#cB:&$XgfX-F<d>%>ub*[.dT3CJ'em(sfYOM%MJEdcI'h&_`>3)e>dJ(PZ)F3J%;m8$a'3C3U?b$[?+oLa5b?8MXL_&68YAOf3oiL-nnO("
    "NoWD#v<@+43BtM(C;^V--#B9rSv=)4G&139$JO]uB56=MwVaIhFt3>d_j=>dOn_Y8kHp;.x<2R8Ln>g)GC9e?5FnM(LZGd;sn3so):8J.a]H1KG0B*>]6:A;@QwK>_rG,3atE#6*K^oL"
    "ji0D&Y*`GMGOO/Me@<2MK%pcM+)W''Yk-@Ko$uG)B[5<-a0_i9(<Pb%1+<>dRsjHMdS&Z$pXR<hjn@+O89?X$n'VDN5D_19Fu/kL$tok&O4aJslbYV-wm`GMuxc@.B+eX-'bN90bC2G`"
    "Lb<DC/Nn>[M7i&&_3]AC-8q293AYB]@RW`<*jbq&ESXV-J%1N(PbM)+QpQn*Is/kLJ5k=GGDagt)GDA.7>[9C^xU<-1MA('Qt;8.-/rp'G&o/&V_l>$9RHX$LuuAOLx(hLnFZ)4=f?X-"
    "1AHs-eSp%MF*k=G#k=$^eT8W-XTZ-ZaMtE$Qw]e$mZI%#[^wcMhJ1d-S5VLN:0a#MB+Nb$'XkSI8-NdMCi?uuGR)8vsTl##?\?@#6_2lA#'cXv-.H-iL_'KG&RA###^l:A=[.G)4N^>PM"
    "su_`jBKo%M9Hq-%%I*1#*eHiLq3H)42>lKP[/w;.mGpkLwfH)4PXYw5gNnY#OK:0C(=8DC`J,9Bd#k=GfA/$$_?)-u_Mk%MD1.AM5<rQ;+489&5Y###Kp,%7w$ZV-DtuM(9>IEd2v'[R"
    "$w//1O0_-6#pa#$^RimLdppw%uxCg)^GA_8l;oO(k:57_J-H)4vf5N(wGxiL9-OF3riN;&En=nCs0[mLd@08%)PZ9r(Q3Ks_9W$#<Zi].XSXw5qG?EMWO-T-L]kO%Y[5^#b_ZP.Ghx0p"
    "-,?>dj&A`<1olQWsa]s-OF_hLUN^_-(Xv90+Y<j$'^VwK.eY9CH^@ns,nK2C%^mL-0c2B%KW+W-i1$3M3k8g1o/vG);l###shL%6[*YA>$CQZ$c0YD#Cg.^$g,>>d-5C-%*VWEC9?K_&"
    "n;v/13#]du-g&.$jYI%#,$<Q.=&[J(B<#Q/cxF)4V</p.fNj;-hf['M<#U=Yhw8><>/+gjO#41^>66I$5=<>de8Y<---Z`-*2bGk9tK&$4*x9.AxxZTC$H=Ak`*vLkO#I#<g#;#jcve%"
    "fEcf(d@d;-e2L-%JhstA=4&v$m-0/8CJvTM(TkNd%5YY#:h^Ee-WrG)PAw%+4sX*e*b)gLp1#[$u?L]uaoc;-*M<3MlBCp$XgbP9rs1p/<S@l2BxDU%?l6j9/2T;.fiP290Gs&-ontl$"
    "6nMB%.cZW-?jA3)A;g?P2ev>#@MQl.8rfgtKdq@-5g3r$'K,9.pG@A+6b1FYV24Z6Tr1`d6tgA$=:?a$sLZN)-RGGM.xfRd5Okr$eN8W-)d+e4q<9C-v<9C-J_,@.?hdV-&j5>di2P>d"
    "5/L]M%-sFrXb-O4neIK:8`6#6B_S,;2U9N(KffA$_UnJ:[$L#$GlP<-e2BV-%i><-5v2C8*q.T&@&<qMd21g)H9+W-h4dTi;o(;Q(lE2CVaI,MRZwuL[^aIhQOi*%BNUx9m#@D*FX@H3"
    "5RhG3X'Z/a,CuM(uL^-6i7h5_2N&;QX7X`tsKvD-@XY8.=21/1gR.t-B]+58t>)h)uB:&$<BF:./:?a$fsEhP*ZV9C$&%uLP,BHuFM:=%n<pA,N5`wKY$tp.TDjc)s>k0Pn#m&Q'j#N="
    "hO[g<T+`&Mx&>2C)GiJ::4=2C:3ks-6U6IMhNXD#1SWj$WlEZ-hwgA$+HaGMil6mLwO&;Q7.V58L)LWfP8RtL-G`hLXZ<^--ji2i3_0a#Ms0T.xjZ*$jJSv$NDGC?R&37'vZ$.65+b_="
    "v'vE7k=20*l^A0'9bia#)LGGMCT4AMTXRF%:jEgLHfP[.-^p[6x=^;-l?0C-VqjnLJr,/Cq_)W-N@;$gN6QtLaPJEd(5w9)J=cg<1O.PdBWJ/2?):<-Ftb(&V4_hLg9Z)4G[q;]o+F$%"
    "Rg@,M[5XC&dKc3)MEuw$Oo:kFWNAX-Bw+RC5kTA$^[rZ^%JIEdj../8;uT3C@lX*$1p>V#Agbat]/^p7O.9KM&`15/'E1;?j*0Z-Z(nsAFD4jL]?W0:S(#b[ocsFrgXL)NSr//1Hqjs&"
    "NY;;?L2>7*/xXd+h=A8#_P(W-'eQwBQBF:.J.OF30Djc)k']5/'YTV6RPBA9+%_-4X>,e>wLVR:rQKC-td.>%gU.%#(,>>#>uv9.i>)3CU-mBti>lu><Lj9D]UC,MJ,<Y$2S9?$Aj4Z6"
    ",b?x%GW,sQM)i%M;lY9:ej7e*jc;F3m(OF3--XMMBiJN-s/m%.Hp-qLl-6-.3E5qLnF:sLvHJEdI'?v>[-_^#`+(##Zm`E[_tfi'e*'Z6r4'WM^hH=.$ZL1K1ld8.F9C8#sL'^#F@Ep."
    "=wqu,M#v?K^&,<-%r5]%u.5jLOb8v5-WtG3#YSb,pjD?9T4O2C6gTb%JDbGMcL&,9b.iJsbDtJ-MsTW$PX%nj'x)N(9Q&(me<O(MF:Z)4Oc*Zex_F_$_0h?8Bvg>$oOs6.>Z+e>M6w<("
    "L/5##5DX*$Y1G0.2bQ(>9p?#6wY5T%,apM9musY-pHbp.d`MD3#ohi<L17#'s*ddudsc)'@A@Sn9_K*5GgIj%Q3Q]upE<r&moYs-]'mo7:>tM(]1cc-@$^I$V[kD#c0YD#efsfLOQNn8"
    "G37P)D(i%Mb&m`/Yf@ouCCK?d?63=(SeBZ>Sp>&mPO?##,,>>#j's8Tft8Z.8FT=Yp[4,%ED`0$l[h/#8,3)#_-'l:=1G)4isqn=FcZ)ceuP)/]XTV6g,:tUTnK&$Wcl8.:l*F39LiNX"
    "<j5/1h<0c.Lgv1'T6]n$^'`GMM[rG)(`O]uc)A>-n8L5%q[Hh5E8MEdC15##m#e,$4cj)#1`[i$-7E:._#G:.iDu>dFI2Pd`3j%MwX$##)####xt%T$*/`$#OZLh.B8E)#+$p$M0iI(>"
    "_XbA#[hN#6>fwHd](j=.ag;hLVB4p@FatA#ngv3+RBMhLL=PcM=@W^%PIr@C_0?s?f7F2C?Srt-g<LPMc*^0%mq,v?'R'XS@,sFrS<G2CA2#HM2j.jLHQvu#_'>80(i_`jK&@D*n1fKY"
    "df%&4DZPwToUMs-6%:hL/xSfLJ&0i)#=$p-ZNr?9B1w6<00Biu$ZCou=en6/Z[a19&o*nMr)i%M@PafL*;>GMD*b8$vLb&#d?BW$IPDK:vil)49F9u.0Nff&02H)4cj+`%dC'<?LD1ga"
    "`72G`6*g39Dl?L,k0hA#*(1E3.,>>#b9u`#Vax9.F3cQu^kLEdfAl8.[3//1oS(?dKaR;-=<xr$Q.CgL?[FZ-gt?kF5;K58pj[a+1P3>5/T;dk/k6g)B3a8AwSl)4g^3Z65c+c-7g-em"
    "Z_Y)40Wps$<bqv%?+OF3WTM&.H6JVM0-TgL90B8N1c?=l:L<>d`+V/:D0[Q2jun^$S8W=YV./;M/oS9CukX_$W'/W-WBTSa>@BFCC:f:d2jW;-Ipf5'+8%W]m2:d%Vo5QBX]eZpoCm=G"
    "*60W-v`NV0i:Aou'c+k%4Od;#wEqhLhRuM(Oxs`#B0;hLjNuM(B]5g)OR51%HnP]uotld$,l<iA5T,U;aM]-#+4fJMA1#RNwJ<A+0A&2BA'e#6,T/i)DVuo.m_G)4EG8U)O*$FuG4b?8"
    "=%g*%O-#7C[=Vs-G(N6MNZ=5$.[O)+t?-a$/A6GcEVkJMFIH)4o`0i)ct;8.W#Z5M6q(T&,:-Z$PL]V%p/l?IWhQ>#3pHg#&ej)#bbBdMsFF:.@]<^-^nYn3T_w*%k(,@[X*E$#_Hk5/"
    "6otM(em;hL&aCD3I(*C&>IxK>v)_^ub'#GrUW`#$^SJ(M5G#I#nTR0PV(%[#i>6<C1N^<-8-b7%3?QJ(@&RJ(Axv9.*76Z$aI&@'(>0N(sb/qLjQ&;Q@vjnLoiaJ%Ck3>d0iS`t+m%V%"
    "IhDp&Mn,T1b>^M#uP%d#87>##qJQ-d#Ewx;EPO&#Cjkv-*,loLued##IohU8gL#:;9;u0#J+q--'[ah#IY418A24kOBHN/MB.8DCX/EHuOS7K3:5c%&m#@D*<K9W-AG(91UUH>#_EwY6"
    "'ajjL)qlP9Wn'^u4LuFrPF+XL)1//1m)7Au*RCu8KN&U`5o=_A35^l83R,W.=1J0+<p]c$?-9<-9pr$;iH5s.<&l;-WsXs7[Q_gE^BbG%UBwGMpeP;-wBxWQfwvu#kSuIU7<pSI0SNY5"
    "7*8E57%Iv:j$xKW3UkD#f0/<-@%29.XL3]-`..g)LcDp.Fo0N(intDu$q`V:#^R&G(l=DCm4Q29q=[w'U(3T.r,IHup]I6%b'^v6ecTc;Y,0HM'Tt`#(S>HMhewS=p6Bu]QfG@&P[TfL"
    "85x9._B3jLZMKs7D)F>d4v.d#(s6i$Tn:gLNVJg'muV2CAC?$^1(G&uiS&##sh6WA7I=)#g--#82B)1N)nXG/o9k=.&+il8PLYn*^/0i)tOlhL87G)%(xcIhtop;-Mc0c$si%NMXl:A&"
    "h$oJ:64*YJvcl&#n%=N.B7E:.6^^N(R%XD#u6lD#_C`$%wwSfLCDi-MFuZ1%R%[9.t28G`OT#LP(_EQ8G9>sgAU(T7`Olr-6v5)6>Ck/%?&RJ(Va9T.(.Y)4^r&,&=BPEd/I6;T[6x+%"
    "R/X,MOrT=Y[eUx$o>$2'QgKs-9XL;?ebxQE)55;-=C)4FBVHs-u^4p8$lH=fS$9Z$>Z:J+?,sFrNc@2C#Vv;-9vWb$OoI2Cn/YsLf1$##Yp,d*dbGp/cilKlM*7gL$9E#0,IuM(IS/a#"
    "W-o+%eFi;-BbuO-^4PDNUpG%.G(K0Mqc00%DioA#mX:H-+KNR%fvAFc&J,W-=iM)mk,E6.gd<MCBPYH3k_OF3Hg*P(uO(H-FXCd-twj^oADjc)JMh8.]TIg)&G:p7k<F2CX[DqL0Y.Pd"
    "GNL$8a_kA#m0MIMm$c?-jwQ6%PLjRs1f18.DksJ:V2T;.=wxP/alkw5Z8L#$eCb;.qnD,;^3F0lMs)SL0N&;Qo>?0uA/E0OQ2k=GUw-;QH,c;-a=Kw%?eEM0H#Q#626d'$kOK@%iK@+4"
    "mBi/)<c?X-n+FF&Qd.gL0x.qLlmFrLpntl$U_c8.8g?%=r).IH+QTFG6:_19;KtM9V;?Z$16</Mo$+F3[oSd)M5<*eOn@X-DhK&$X_Y)4B`Iu.eS>)4l3=9.$<>DCTZ%gLC,sFr`P?d;"
    "wb3>dXPsA#1rn'M/t//1qx8;-[*&f$.0uwICA=/M,_-lL5l_%%l*Uq^PBZ)4=L5T@+iE>du4pB]RC.W-pZYq)nR/oC&VjJ%lk5*n.d>9.*V&)*-2h;-@8`P-$P>s-K2pS8<n3>d'bY$J"
    "aR<^-75J9TQhsU%u#A@<$@b2CU4FGM'x2j(*dg;-Oq8r%M[ZV-mb13if3w6%%A`8.(p(;Q;]HWS)W@Y/jvK'#xK5#%2/?N(Tm;F37xVs-W,O/Mi86L-1X`A.[Dm=G*%b2C?f+`M#LJEd"
    "6,,,%bI:0C5.&@'BO*W-vcap9l,g%#=BN/M0>5,M3]CDN02&F-0AW/%It4qLbqqp$(.kFdI,5_AjIM;Q.E+O8<S*Ma;pCl-A,g:'pa1^>i=d;%b%(5%PBamA8$.[B(BJjiJ4$##8DhW$"
    "&.sJ)wKA/.vDN/Mpm,%.halI<NI$=/F+F$81r4N`.Z]#MQxU*%qY,W-xr1(Zw2)A.W?O&#rJE#$[YZ5//L;X6%`'p7g?%OX[NQ]u0'OvLCnfiA%_kA#bn7Y-_;OF.q3n0#,OcF@#gsx+"
    "Y'jG3L)/B.GkTD3FHI,MEFe6%;eVE#RwSv$<BPEdhb#]%Z1aKMQs_c)JC$##r:`*<R<mg)eQwY69FLP8kGc)4g6a8.ugLQqOY;;?q7Vp.UX4G`L#$Yf=*SY-BO4O+B[-gL.CLD&cO;mL"
    "'8V'>H6dg)1jp[6:^5+dL9Z)4.m_5/s0YD##@.lLwu849K8@k=dL?o;/=8DCnXAou*8wl$,%KoMxl)Vt*H+v-Y#8m8$(=2C2:UpgcW/%#,a%#%[9F9.C6&p.RMJ^#CI0p9QNU6r84fQM"
    "4Wj/%%ptVoV,Ni%x/S;-KxG&OD:MhLZR/^$>;Tl`)=PcMTB=/M]nJk9>=ETBI6q0%A0E,MjM71F*AmTiqi5<-`:4R-BOnm%9=Hg):)QW-eaAq'JX+FCml$@'GnjaEq4k>dV^OEdNihdt"
    "xKJEd+J`Q&Sm*X&31ngD=`HLGaGG++Ujni9d,lA#m:4ONoldj9<bfBJPn6*>P#41^/2A>Ql,Xp7LAqWJg4Mm9,LUaQ6:rRB?%'Z-)@[*.T6:+=.(l?I`@X%8d&]p&FI,;QDJdW-;?Z*I"
    "<2Hs-k9-[8dZbA#3Os>d`RdG;I/4RU.j1i'h:nK:L/v6:qjL#?/BV($s]$_=Dpdt/FIpgLP/gfLO[vu#%X.%#,KWU->l$m%oPman3^*gLuAq[6.qF+F3_>)4:;I&=/]x-u>U(I&.dT3C"
    "?Z=[084NQdHu,l$YNQ&#s2Bo%wf8>,Nke8.Mr+^$/P[,;2jh?^)0*gLqrY<-b$.X$L<$:)qdbat(_MW9$t^C-8q#`-:Y_QV-6fX-9quv.NX#X9N4P-5vp#D-q0(p$2L6l=k%keFB_Yw5"
    "8<^4=<]ms-6WxPBRi;6L0XvxL$WmxL_[QY<p*-Z$2?3K8fqTj(TYW]+UucG36O*N(;ukOiLg&49,gWh>J'P6M=_7j%Ce:c#cjLEd=9WM-uQ'LO*$tj$=vQJ(rS<B#(0p[6og#D-)(Sb/"
    "Pq:X6K_j=.LrHW&(R<nLB/u%;N7'?djSxG)vwgIhFkYb%ZWGoF8'+<o]RMA&k#[`*33.^#cw.W-w%#q_#.gfL+$S`CWuKB['a@>/<J]duxXYq;*jvNsk/#&#%&6<-U$BkMJ-H)4kH'Z6"
    "(geu/S.t`#2jkV-kJ<m$aU^]uoM7U(+^iA.ZMSS[MD>p75(=2COFi;-sZ+SM(r=D:`v2@$<B%%#cQk&#%xF)4ZiM*I#/[Z6X'4]-i9OL:mM'HOa72G`>imZg(kUN;QHO]ulctR-vEFR-"
    "5.]:&j:GJ`kdrgLA@VhLSa0gM9(IAOjEA+412xA#5-M#RDcDqLpJSb-VSHtC.q[AOH)i%MiuFo$@6wGMr?5&#ti7m-l^.th-)F#$69>m/-T/i)_TIg)SJSb-X&HgtC<ml$SiC%@6RV=-"
    "i9=3.JCJ3NHO51%NkqGMT<kFN+dYV-O;S^#5xw[-Z;Re$gpwHMK#/qLPoK4.7t/kLn`E((4nU*I.(NnN?ee294K+_Js/[`*cXlr-do/^#CniZp4b8W-nNS'dQ[x7MSMLD.3Lr=GX*jMB"
    "QhrFnQaPSd#t3.meQPa<sq]G3PJD$6]jH)4ZJ)B#s%J$pm:?R(^cC'@a3.K<0X(8DsfE>dm?5edORJ(M1S)M/H<j=GH%%C:U[b&#(H(v>N&%Q0],Oqi.eQEln$@d$>IHg):?UZ$C#Z#6"
    "1r$(FeTCa463(B#]kc;?AgE>dc=lYT?]MqL-wwm-T$o-+E0MIMrBOk%Iej[JULBiBm'9v-6]79.if*F3nDa%7TBZ)4J#5&F#ktA#RVf$BdgdQ_=&s38c=KpTk)p%#_j*gL,]%d)tV7Q-"
    "V/U4&;9KZ-t:Eg84@L0cj>=n;LuxmCNY;;?KJ+gL(fr(C#@n.k8I$29[B6?$Dsn%#TBAQ%?]d-H,[+W-M>iKcH=9&$@<MhLV,p+M:^#W%QObc9Bc'[K@i0gL_F5G-hBVe$O;LlFo2:-m"
    "dH8m8RJ0N(D9&]?#@Y)4J$UfL(=w[-NoM)<eu61*UotO%lpXp._*_^uq9Ne$LZZ5_9qq[%Is/kLnYaIhM6nGMW^rq%3c18.>-<J:d%$Z$h%0^#ZS9aNR&xC#QZ@gLn>gV->[Bwp:1VHF"
    "8rSCN<M3n%0UJp.;aPM'aGQ0+KNoc;Ye%4VI(*Q&*W&L9&Jde4ebEB-obEB-(xWB-QXr*%sxKL3fGnU&3/s&=3c442F/Wk;vR^X:@8f8:0qHg)Km^s$oOt:C,x?p7Fqr,un7H-.9aUc;"
    "[34kkp,@D*aP8^#bN>[$JL*7<[FOq;KG,;Q*pmP.o;2/1eMBp7T[vZ$W9F&#v&U'#FUKC-SRbf%iSl8.PYDD3vY?m/$2tM(PU)P(?oHt$v:MT.=Jr@CCt-I/A/kFd2aXx:Ov4R*wu&gL"
    "0K^c.YjETuUg3vLL.,o$qS/gLD=Z$GwaBN(B7K&$CSGs-Tq^D=Bhs9DIx0XCe@xKG8PgIhZQ'(%U7A['2-B=l_*=Q8W3Qg)^CPV-qZGq.nhtM(9Rj+5[kI+%S>[4)vn7DC%fFAueC0[M"
    "?og-'wf8>,8En;?;-$Z$i5p;-3;$i&@5@99NY;;?Bl?>d>?6X$CJR2CkGJ9iJO@Y/[2SS[w&gQat$JL**8a9&/'*XCRhK&$FLj:%fu:W-Rr0rD%O8j9biX_fIw=2CFcs*%sXJ#>1;:kF"
    "GV:kFJi<pIQ2>)4CeS/:Le?#6$a^W$;jb;-6*lc%#jp;-Y$3:%YRsqBkASJ0(kJEdLg/Pdw_fb#:8$9%wD)`?]DA+4d>XdMvjmi9VfYw/;T&;Q,T@@%o,*2/U&>;?q6MkkI<MGM'Zpo%"
    "BBcf(J'TZ$jQhfYi+v`/./B=l7q.PdoJE^#:Mk%M1A<>.(AP##)CQ$$>K*1#Mke%#twTB>]L@4XC%kD##(4]-L62pI-%Qg)@83,)ANlD<XXOAu&NuFr,2^GMIO&;Q1sr%ufGxi$[26tf"
    "Eh&;OY1.AM3$#i:7&h>$285O-Ns$./sws`#2@=@A4_0a#UIWK:Vl0B#6<:DClR@wg;o(;Qa7.W-r:k0,57HgLHWCI;FdbA#V,cQi<XHM97p#al_mhtLj.HKCLch$'m]3WfNs4-;KY%*+"
    "4NAX-qWiaQM9Z)4Bi'gL*L5d$YCNEd4UQL-(A,:<a4(Kab?W$#%FqhLV9K$.ua;)N$*qs$LRUd=/x.qLqN&;Q_VaWfdf^g-O6c'Qv]aIhkAUEn&,pu,as^$9L6fX-%nhVI`Q*HXA(i%M"
    "I`q5%>()n8P<VBo1@E)ERYGqVEu,N<ifrG)Fg$)*q+it-Z80,;H)=2C8dYs-mB$aEJT3RMq:km$[BwLao[Me.Z-4&#x)iX%1GWD#Gel_$)kH)4K.78Bf]tlVHnT=YEScn:$&9r-ofoCd"
    "EcX19WLQZ$h<r$#fMRZ8nxgN(aQ3Z6Z1tM([LSJ.xO&d)eBk;-=t=0&0n7p&?)?\?d)sWS[8g?%=ZX>FljI]S#8kUd-.Ab?ILa-+#W3=&#]j(P-ulhsE:sU*4d`MD3DMXw5gER>#OL'Z-"
    "3XdL->d:>8R(=2C,i0&G47=2C3xFAu&?MG>DD.0s_72G`?HEW?p9eM1^9jG3kF'0%+oV<-xPTW$a&/gLuq//1WR39.$up=GX-/W-@]4[gv-el?^n[[@9'?-P&R%f$d0kEIM[ZV-.9N_J"
    "kXj=.b[bR-tRL).vY[[>&vsY-4?%B#:'iXgrd'DMnZ.Pd)8,PNfP&;QRjS2E1e_4EhFsVnZoSY,:a4J'GD>nEs.H++$/7I%vP&gL[;Wb$7G6>d`]=,MZpt0p?.H-d+=uHH)27DComX,;"
    "UIem0ESXV-Q7pLEAp4[MY`94.(Kgc;aEkM(YbL,M`@e6%c#+@f-u8k$r[=BAFwE>d1[mrTl+4v%/iho.tU&0%>xs`#.S,gLM:d>%5.%q$ocCm9RkPb#t,9.>WJ*#J5eYl$[X7>d8mU;H"
    "()h>$_6Yo$8bo_6SOqkLaGI/;60wY6c33n%ic_E[TI7>d3mTs-feW];>A4Auw&HpL>O&;QvBJ9.07ux+#5m8.k#0i)I/wY-5;m5/e[kD#cNrl8S]k2C(fT3CHWDqL4;/F-0BV,?(vW>d"
    "fl*NdfusA#^+pGM`sc%&QXe/)l$e&68]X$/B7E:.B2FtC-<uT.e1L%6:;Ls$]tnJ:Zi+na#6XpKMou0G5qc'?OP+o$SEX<-RILD.[$(,)VUrM(wY3^ZTuIS@v1I21fS-W-PgEh3[6qo-"
    "j[*TMC,6w*GG1<-4cMt%I>uu#x54R-C`JY%Q:B>]5G.e%w(o+M&.F$8c/Z2VXoW-CDGZv$VU_lKck)pDjl@$()7vcMU5b?8N>^5UZp9*9,Z_$'gDDF<Qsp/)mQEM08-5d;F.wY6(bQa$"
    "3D'Z$>NSp7-$b>d=Jr@Cc,e&/kEm=G1uh&6vtcQ+'g%kkpP8>,xo7Z-uv$ZpqALh$imd>%mL%f$:9&NBlB+AuDc/E<IGoBJw5Us-+5V:?A'SoUbV3v%J*Jm'XOs2ik]mJ(iRG)4m_b'4"
    "<?QJ(qNsM(>#4gLu_tg$&;#C8,jLEdRDQg:>ZJU)gC,$$4c*Pt8]cBS?XlA#YTA(%BC/h4DIR>DG_#H)qT+W-65)*6_p<$3u9@+4IMjD#x1tM(HJjD#.B:qV=BU/1n,ddu$,_^u)]_P$"
    "PT8.%Hn=2CPL>s?_5-Z$]?p0%=1001>_G)42DE,)@Djc):0'W$r7.&Ys$ck%3-B=l&c#`-WZ<Q2mI]S#C))n9s$;3;IZ]G;[75;-Tfu;-(X-a$rq9LjFQMm$iVXV-G&x[7D`Z=K/Tt2C"
    "3s:;?fXbO;2?E[<jJF&#`=.9.+`AD*n>C#6dSihEb['^##6oJjdn?q$48@2Cq6:Au2,)m$60)&ugvZa>d#k=G%.o2<hos;&RGVv[PZj393Zl)44]JD*N:2=-BO,M88XO@Ra-&f$0Cj*%"
    "H(i;-QarX-Nkd:ikOsR8cwrr9n&T%#wBp]$H,:s$$RhG3f(XaGtHq><m%C^u4p(;Qgo>V#j]mg$0aGs-.#4;=);)d*$<qD<,YaHZLgum/3&[J(JN/i)9Whh$g#w8I'`3>d>t&=-?K&N-"
    "PeqZAd.NVnpShh$B<Z8hii7d)Q5)$[c[Y*$mXLxLlF3&C-272hwl&6:[0+g$5V8*<[l5g)dq;#MX-Tj%dAb?S(W)j;pk0B#F%>;?j/nA#_S/N-@XJ^$op<p76Mp&vk#Os7t.FW8lIF0&"
    "54IP/M+`29Q[><.Vn@X-um;hL:TnO(2Mh;-87A;%[#G:.(]:NBJXkA#^YvX$RIr@Cao'?@qH5YB<rN>dXJk#DqU-L,Br6<-Rvqt%QK`?%aMA+4[KsfM`;w[-(fRW-wNG$K2p@v>`CJRL"
    ":MCiuC$>;?qN$fkf'@q$1_V;-I#>D%g'T2C_/P*IPi-<-ZjcW%5@*20Q%7<-Z%Pc-$CqTr)qS/)[2xgLTDAouCH/JdO.3j9k%u%p$Y#AOFa8%#D0N'%NbdV-gT&H3ishVIO*van8m;3M"
    "vDdANuO1DNJ,U-%74P]u4:qDMJJ[uL$OL?dkD#N9a<vG*uMeekWkmhMlv25%UwgA$pK3b._T/i)sv$>%@MO5A>i?>QTLwJ:JZ9Ll:A(1#9R:^#YMrkkcHsg=aMA+4UNmi;(CG)4Qm73%"
    "ie%fNL9pc$;k=DCPk%<-gOXg$_`r2C(<0/1m_X_$S(Ee+lJ8n.lV.%#60uG%.xvW_U`1E%ZEsR8^t'B#t@RY>Nq3>d>0Fs-FMk%M^(+(M$p)J%a6dgt,A#c%Be3>d%M?p73bQ-HTL3eH"
    "baRd-@@C58M76?$aXYw5j#Ol<@2>)4=fDe=E=FkNZ`?:9pi;+&+i$1#R3e19uD4P<U`xn$Xa^B@Wu6+<I_KN(LifA$UJhamj>Q&'*_Bqi&juB8NH4Pd8FT=Y>_X2CIRCaMFnT=YG8vJ:"
    "4vG,*lT&<-7D&_1Jax9.F$vM(uQ;X61MZ3WIO]Z$*.kFdtS0UUnorG)Ig)-uE2O$Mc5PI;-=U0G8c:J:75WVn't/q`pF$##7NJg#2IXD#fM(W-SmX3F[Bs8.s%[J(?6W)N7s//1:Mo]u"
    "WboBdCtN>duF-iLp?U];i(w<((xcIh)vg;-Qw&V%Mp%@BxQ)x5q<6g)Kw*W-HO:_ABv)?8,LKI<>9X2Cj*4qLr[AN-*JvKCk.-TKJhAZ$X4is-v6`d*[@i<-fZKW(Y,/;Mnst^I_$T*["
    "3+r2B6:tx%Aii6M8LfB.9Npl&#sl;-E7:%%4MLjB:RguAd#k=G#O+b[.K_,;^hf7L4Pe.%'cK:#)<>W->w8$JZ3sv#G?Njt(+?=MjN]S#/TNu&`XL(BMLqKGr8;:;m<SnLSVs$#'.Y)4"
    "vTd#$ZBWJMfq//1#dm2Cm''Mua_kJ-KYg?&wx4;-0wj<-k$Mp$eGKgCxU@KsUA8n$)t8>d_=Sbc5X'kNR3SS[JPH$A2YBA?.R*L>+&QL:i5afO3x.qL^8gV-ig57WhBB?Ao_@B0$#bIh"
    "P.xG)sCJ4XH]`/:l45H3Rm`'4MN(Y8(u$2hUqX49C]:_]?j5/1`0_'#%5'l=83Skt3VO<SdtQm%*.Ck=CJE/4>NQN;(,=2CHms,M6H;&.)1VcKmMmv$xF.%#4e[r'mMGq;14AT.[Dm=G"
    "78:<&J1^>Qr&>e:2uU3X`^$4$Jd#q7*jbA#J+Y5hF-(X%()2wg1Oj;-0P8_Q)Pdmq<eXD#a4Z++([;8.Vgr/<dMsI3$91G`6Y$a$nrV$KdS<fI6PO1:FpeZpIs/kL80K;-h<IX/]qvu#"
    "LH`Iho/vG)k3.5/Rg90)d9u`#T)ahLb,#A#M<&UD;=Hg)Xi-F%3DO#$^S^v&EkGn<S.n8.USA;?rX`s-U%juL+C[6%ee7@0#TkNd@b4v%wG/T73I3eHmC$##$&YDN_]CD3jD$Z6.,?t$"
    "L>4gLrt]f$Cjk;-`T&Z$/%H=&cx5_AtlxfL/?A8#n%a5/lWf5#4EaJM.=tV$Q[kD#$'($6v6K&$)n]79O:Y)4V#4jL^++g$x5>XCWoP,>O#41^%dHTKRcbk%%:1_AdlM'-']7(#LmW*'"
    "V_>>/9n.i)s/K)4rg.n$<pAgLtQ<r$Is/kLPf?k*g4eY-tas<1.UHp/a1mL:lr3,Q?*x9.Vw-HM&+qd%d$dLN*;X)>/WaIh'`I7;OC$##2oJGM@^Q^$C3Ab+cQ3Z6Z5nL%]L^b%@<MGM"
    "P+im88q,eQi4sK;bL38V7_XD#tcic);OGp@pc5g)trvt.(<0/1^/u92id3a*q2#<-bW+,%mmA<-El<p^S(fo@'n_*mLR&7&YboV%WCpA#Rt)b$_rQQ/<4i$#%*nO(Iq<p7OnkA#V_KE#"
    "h`rG)4[Ri=)o/k$w&5G`Mt'L5P<I#SCR_58Sk=)ma+P0&_%cGMLs7DC@_<j9DT2W%>jZ9MAvQJ(@Djc)nIV29)ei63g&5RNIk<Up*)c=:6-EOWf]w>%FSLs-`ewR9capIXd*Ej&eEd<U"
    "fp$>%SE&8rGr1*%Ve>-mVCNP&A>uG3$n]?Tfw8tU@OJEd/]-W-60w?9Zec#$9nPfLILQ>#>lS&v26V-M:';8.q-DpTR_v>#d5&gLx(k=GW4C0%S+<i*#2HW-fX^'8tUI?7>peIM$8_L2"
    "R_Yw5DQwY6[DsI3v?1a##ULp$qn,-uGBK_-au,U;_:_Hbn?9C-a7C?&GWSXL6_(-kp?K$.H#juLb'(P%j&xE$^cE=:=)c;-]_GP&*2pu,]nq-Qe)ahLY6wlLM,x9.l*xF%B,kNjXh?lL"
    "C0[#'L8,hM,A]F/p@cS#fH(EMx2BHuwOs'M:+J6Ml8s'%5$0<-;ek,%IpSGN*CF:.,<9W-W3vB8:$*gL'CHxL8)(s$4cO3r8X&eEB.1B#P0YD#WimW-NA.kr$%.t*SAPQ8,]3>d.vtP8"
    "@evHHLAds-G)P6M'n$R8<qK#$8v2e$Cws9`SEuH?3N*Z$W-J)4vv7n$NJHq`#gN#$UeKW$]vx?@X7^-#T%VW-3INx^TSpkLu)D8.T8^V-b>CYAvssr$TUk>>2F_,Fm](/;[`Z)GH/]n$"
    "Y`HQ/Lqn%#stWD#*s9HMGgLa%'6)X_m*FWJ6H>O-On`&&1OoQaM####7Ac`jK,[`*f2ZZ6'*x9.Bg;hLalsn8#sNw9(.kFd/,6>dwPH2CRx-./aqYS[m^uX?4bmpg`x5t7tbQ?$>uv9."
    "oDsI3YBuM0)`j=.Ae0a#cQ.2989[7;8JSb-EgwU7]>O2Cf2R/aWVeS=8bsJMjgGG)DeR_A'8tM(-]U0u4*#b[5o.d*tr/gLG00jDdnScjJh@;^.$3d*wB^ZGr#wY6jLdh-PSb7UFB8)?"
    "_wGJCPa7p&&o.g:1l^tSY%bgt[i_S#-/%I-UX-&'PVef($^r5i_XQ>#>+9&$L$ewBSVK/CQ6F2C8n,P&_RimL-EDA.Zw.Bd>#m>ne^8%#wx=4%ccs5i#OIp.%j*B#&#k9@6+&NiqmdKF"
    "tOF]uKA:^OA6UKFc[pdm1+$itZn>h/`WVS[M><A+A.?a*oG5<-x%v+?r<,GGPY;;?MTJOi^1qc$&UMT@$>?v$iHFT.U'+&#Zx=G-ZBWb$Ikg(o/t1xf;UKk$rf0i)4S)`df_8tBPGsP_"
    "ZUAG9F@[Ku6rj2C-VxFrqD5>dYQd&#XItcMG_87.o=N:I<05HlFg*P(U&RW]n1o#%Cg:TKEY1x&;[-gLZ2H`%^u(N9'j%$n>/5@&5lho.hV)F38EioM<37g)/j4Z6)aKZ-Y_^V6)Z9W$"
    "7PgIhX^h2huOI1KU*M*Mwc?k*&AggL-#/qLIV5$%VixfL7'>SncHW3k[Lu50(0nO(rDeA#o.Wm/VH,<.XXTV6))@[$//Y*;ZV4&uOq(;QDN6I$uRV<-H'P7%9@P]u:)C`&>cj)#mpB'#"
    "h$(,)%=s;-.5pj%6c:-4Obj=.2=5GM,q,/CaFmq9@x=G-mTEM%xUN2CaA?l$l=*9.8ERS[C^9Q8BZvV[K&@D*%S>a*RxgdM9sAk$-l^gs+Kbr$L,D2CHYDU%nEo3M%Rvu#ZNOG%dR4x["
    "2SM:%57*H32fwB=J<O2C?k3Y'fC06>OF.&GolUDfoZFEt@ZNr2%)###p4JGMfB0/(<o###jqbIdgB/<-JVfZ$LL;jBnpJe$cHDHujZk_$GJ0fO;j(P-dn4X8KMLOb5vZE&$2tM(wNdl$"
    "1]IF<>>O2C,ZULs[+D]M)[(b?Hd%`=3[T5qvt#]%&C7#M]rIiL[JXD#DjrA#AF$UTs3iQ(K/gKM6+O?%Sknj9v3hW/$/D'#&EFu^O^M`$#`oHd@cDW-^)SBf5cJ&$c.uJkv29s$25I2C"
    "WXDqLP.K->JXX]uYk9X/+PL=oskS9Ch.9^#H<S;%:dDZ81I#l$6d;F35Md;-nnVe$adS49:o'^#HeV]K2v(W-HsHXg2c&gL_<63=,OX]u9VAW0A(i%M(3%^$k2lp.jpB'#1H<B.fW^n&"
    "i7B^#5kC9r8FclAQs@OVOR+SM,qT=Y@VsCdkW9`MN<wwA<[d--)lKHMfr//1SaH6q_Y*Z%_w:?$G/4&#vVb:D+OqKGZ06GM-XkD##):6A$Ub0PbOM->unP7Av'_<L+aAh9XugI;_<`3X"
    "?1KZe/Qp'&B^N4B_v&?Jt'q%%N/i5/e.Y)4q-mo7'6=2C-nM2BM]GF#$*HD-J?W7q[qF8M>KTW$CM&q^?bahDE0Zg)X$vM(QwjV$MU:?>CwK^#'wZ)4dYe)<-uK.;,l<>dmlB&8rSD]f"
    "I4b?8i5b#-0Gnx4odaG3R,l;-D#vlM^ZkD#$qAo%U6*gL>`Bh&aGOgLFODo($IJEd?<1eQ_]s]uW4]AC`@@K:vas`up%d`/LOuu,2GR]4h;uTr2c8&vW<p;%_#G:.w(4a*(]g8@];M4B"
    "Ol0i:KWx8U1;5E8l$%[^^Hj58JNO&#rM5p%CJ`$'(v3B#.%eX-jmY/1u6lD#ED*K(>0BiuhI.m-IUKs9/x.qL5ragtXTIY%UL#B#0w8:;+WrG)D_1wg=vQJ(5Z5g)Ro8gL9sHg)3o7i*"
    "TU7<-$4Yg$-,H>d<Ki<ComDHuF.fb%AeDsgoA%l$l?\?^#PP[6WDxj%@VOqkLITtfM%$/qLIe?k*@?-E<%gsHZAhxS1jue^$X@(T7lRTeZE'.qr>16g)X02N(64d;@qSX2CU&o%MWAk4<"
    "kQO]u9R>ZDlh9^#sj$1#cIpU@pA`D+2_e;-iR9`$iQ3Z6f7L%6<3$,MkV^C-We)cYK<<39+Yic>l0kB-N%X/%Sj:T9=#P7%RRsY6NdoG3L_gC-Q@?a$@6]58`cTX8UEt1'66OjB(>D)+"
    "t[r8.N%G#%P<@,MV2>]#,#Hs-Ok-/Cad5g)_FKQ_&8EF&Z30W-fO0<&[F#I#%^wX?5_kL:eD,r'Y8SS[Pfou,HlZ5/sQ;X6Ag;hL'%qBF&*lA#Aadd$Tk:c-+c?=l9x-W-V2lI;0VF&#"
    "O%.9.kL3/(nRb8Is6tY-QQ$Z-J+i;-_$qa9S+-Z$-rDi-2-&T;:do$&L;ii']MIx.hGDLETJK/)iPlq06:_19LNB?dQ3LGMv5dS#W91nMPrZp--Ck^o#,j^o5t*H3d4wR9+H^p9)K0s$"
    "O2+gL83*/88MO]u%k^g-#0:T970&2&(UWW-MoB+$2[9&$#=Jm8dH=gNTCaf8SvN>d%jj7M'g2#MXYNv%80kW-4mLjr.n6g)NIHg)3l=<.]Xj=.GBc'&l_^V63+D9.dPi=.^#Ym8moAe?"
    "R@7>dWL.W-v59H+1R.PdgB168PbpYAZ,SS[dJkgLF8%oB;T(*4VF+F3`E?,M2,p+M`m_xMGGsb$1[_P$gt5D-DtU1.mQtQBc(6DePk(T7PMW]+j*io.jc;F3><>O-C4RB&w3Gt]Z9b_&"
    "1V#dM:eGo$IKk%MqYrG);T7B#Tk8>C%JX,Oh:Ts&L+2Q'^>qDPLCw[-a<;583N'Xo4IXD#vu[;.1X+d;;c3>dT.@&FW&rI5iLU_%lc><-eq=w$F8nJ(kax9.<HPt-M6qfL`U:-'J)U>d"
    "Z(Gt7R.j*.U8JwBd^Dt9NHjVnsc<>,W-&H3_9MP8DZ/0ORXj=.0.lA#xfYo$1xcIhLgv1'b1I0u$,Jn$YYe@>h^M2i-)4v/Fo+T7xA>G2$5TGl=$-^$]i9X]Epf$Mg(&f&pW^%';S6>d"
    "Ro>a*4ab/CsYpU@C)7A.GD,D%.iho.50G$'@,[J(MdOs-*+xbOn#>k%nXLxLhm3AFI+mane.s2C%%E<uvp4[$3WAhLl.fiL%4$-NIm*F3=RWh#I-vSJ1.G)4gCDW$WgA-M5EHl.7H,;Q"
    "#qep%;;&'&%Wps$gKFm8G9:kFHN>G2g@d;-iKE7%TN`hL?bYw5xt7&Ga%Lgc>TS2&S/2iC;h3>d_Ojq%Vh4K::#[w'?^`cmVkn'&B$'/1:qD%>7,6r@]Vo'J)r-r'd=2G`Hee3=jqUa*"
    "b04<-@T]F-bwW+.nr(d*x(V<-k5h?&VQ&###fX9rsR$##;9S0W,w@X-Ylx`*gr6t-L96,Mo'_b*f'&<-*@_V%o/,9.u-B=l,h'_%aHO(&mFp>$pisn%PUf5#/-t:Kl.#42P=w[-vgw=J"
    "<l/v5^>(B#l5AnE@0'T.ah_S#cMvk%$hc@>,7:^#a6d>e+;2'#Y,>>##xw[-pE=ve%2`B#X/p/<+Y>W-,Rq@.ofZ.%%82G`SDsA#Ph2^%V@$dMJ&k=GOJ<Z>S%[w'CSsL:32jAdXHYs%"
    "QDnx44v9H3?2L%6tSEq-5Sptq7DEJ%m?DgL$qT=YP#FdM$3*/8/kjtA6O`X-A66,uJZV.&VJ2N0a5B2#Zv]e$_V8b*ZIns-K(5a*#=Og:J3O2C&QlZp&LMgLDHJ1C.P2,u:eXD#RSY++"
    ":H7g)rfe$8N=Pb%Etn=G6sAgLrN&;Q$;Nt:s(;9.?,>>#)wsM(%xp>$@kJe$4MjD#3BZ=Y511bIORJ(M9&T9Ck:MEdf6kf-X8m8^)j#v*v*4t-Ad8KM#:xiLb_J(%p_9v5Em;hL]hI(>"
    "[c0^#,Gr5244fQM@j:wegWEh=v4u3+Aj5/1[hv1'hr_$'#1?gLUPM]$;vsI31Jsc)/+s=':Z)F3mg*P(B)s9&o@w^]@o&cEJ]DmL^?U];dfV6j7Kcc%$R4C&VPr&,H=$##/&Rl*J-06/"
    "1jp[6lbSfLx>q[6(Q7sC(gI(>:s&*>5U#l$N2J3bZ6wv9=eb*ZP6@e-gXKpKf:5O%:9JAI;`/v5,,wddU:Kv>6.G)4Tx1e$%'&HM=/m`%p3,VthWO0cDK+OF[S[)<QDO2(GK+n%>&t`>"
    "erN>dpjho>><f`t@iurB)@WpB+AM'#%xF)4dp43)+a,P&FDjc)HJjD#fXleuc=2G`GX7R<m6B2Csx'gL_Kq;B+>Wt(vVp;-,eU-%x&@W?>i:p/bnh5%PKkL:&P)c<LO*iEbf4c'>Q*gL"
    "_b:x&(L=9.[K:0C%MeLCY.,8h_I:p$VAl;-ardt$<CLZ>^4>)4_f/^#(;f;.HsIu%LTFm8/angs0n(-M9^ea$$S5oCJ6v_$uDg;-v9E6.lOYc*YiZp.8*x9.vY#v&$2*f%7PgIh$[,nj"
    "b3]ACV@+oL[,O_$O4>s?X%k>-HGR-F$:H=&L0fX-hwgA$^vtJ'ai2L3=Z]m%^US3'#O*?MB0#?I)bL:#ZE0W-@q3L#cYDD3_rss98wS*@-;b^#C:TA$Jmx0W.ls+&9Ca8.f/aIhZuwKG"
    "],Tq^`C#I#>tOJ%r$aG;0uO5;)*'K4M=w[-]Y<UFU8UV_6MCiuNa%I#:VNt::1$9'J-ZGCB7f:dp/W;-mXL$&5JmX$Ltf5/Z`9X6Bk&j*Fh*-DvsA*[-dT3C/UpS%ut7g))2HX$e&UQ/"
    "TDjc)k_b'4)f`8.QMp=GM>;9.[hX=Y,W9W-NC#=C#j*O=9t6gLZkJ%#^6-m'$a0i)HJjD#D]JD*<c?X-Z.;<-Z0Ai$(sp=GAZKW-4To?B;j5/1fU?gL/LihLPQ^88=#7Ae$1mV-T$Ai)"
    "T`v[-CQJ,M8f7)>b,UB#&+ddu%<2/1D@V'A94NQdR+h?^gGAgLo-p%#tsTE9jXbA#6Ubp.Z9k=.M4@:1:,ZpLo$k=GXc)9.Bul=GSC7>d6_sRApLCB#LH`Ih;BMwB^i###;dDE-H+%Q/"
    "V0<W-:HF:.VD(W-ofcT_HK+=Aq3)d*FqQ,M+vBKMC=w[-Bn/u-[R@r8XIO2CeeOkXKI(gL=,ZpLW$oF%5RW/:%BhB##TvG)(n-T7jAa9De'2gL=(8DC2BBFCbLDs-)s/kLMF^cPMoU3C"
    "d=hVI#b$6%qYxV%OQ#$6,T/i)#AlJM2,8DCA$9DC8a'c*T)(<-[00F,'.Ho$I[Is-H7O]M?G/j--56x0aUb05T,G68kYb?KUw;S;*blF@+H_E[B;&,Mg*a(Zxh)M-wX15/(6Dcug>5)M"
    "qOta*ZFCgLp8H)4[#VSC;8rNXR/5##^xd,$Cdj)#nhOkKuTaeFWSpS8@XkA#7(i]$:PgIhGNL$8j8EX(%=tdOBL*>N7fm##9>mX$)7E:.x)Ys-SCI]M9I9)&coG`%x];[$:ga;%XY9@f"
    "[+kuLJO[^$.$V&#P:sx*=V;s7qs*WS5S=38<2__uRA`(vn6>##q#/i)KG/Jdo`C8#4wjN0j];[$ZCP##DQZ`*pX;N9l-6Z$aGmAdeu*a*f$FT./Sl##x_:c$2lRs-g?.lLMi?lL<(i%M"
    "cd55M(PP8.jT3Pda[=WH5=eWAI9J72AbdV-Jax9.>bZV-F<d>%(USF%EuQ4Cm8H2Ckf?m%627a<j.S`t$0?X$92'UVDbP]uZuiQ0%2G>#uU_n#qxbn*l;C9.tiD8#rCbA#vOX&#ofm^o"
    ")Aa'opr-j:QK-C#PXYw5DQwY6uS$&4q'c8M[,BHuSFJa$rEc7ug_MbIA<'9.FC7/1;t`,M@ppcN3[nO(]88w8qx*2%23g;-;7pr-68hxM4rT=YkQkn-:=^VCpc?k*1WX,Mm;>GM15P$M"
    "lG3f4wQaF5Nvb)<Oak*$k>a6.Bov7;UFtWotGUd%Hq=2Cq$S<-inju$^#G:.aAo>S@xa'4fM8gLCI>f*nVCT.=[X9C^3w>'bTN2B2Ix`tkAcS#G;<j$2AO#$epx/.h/Yt7@mK.;5Lb<8"
    "cDet/7b0a#?P4Z%A4JNV'[JEde1=b$Z_=0*Wj*J-b.4_$(^2F&UTuNF^B8<-ml_a%`v(K1uQwY6CHL&$Df*F3^v=A@Cn:X6Mmm%E4hu,,j>$I%*$2gjn.I1KMKks-4pop;E9&+IKPUV$"
    "[THp-WD:-mkN(,)Y%TY,$f1gLk<C`.gnK&$+hJb%WG53)HR/Z$$OrE@T0]0G1#qC$Zs:x$iW4G`=K(*<F7?Z$kP(`Fr*>)4(Y.Q/A;tM(ZTSp.2t]b%jl:DIkRxo#Md&p:(_kKNo,6&N"
    "QEt1'+lF&#$QFgLftjD%Ulm##mVN@?A$_3C(BK?d[)iLdGDagt;ATn*^reQ/I,>>#NZ)F3?isY-BU$gLHAb1)6o0N(JV/a#0@I>#doD2C?pb;-14f^$I9+W-S#_,YnqS9C$g_w'VKP6M"
    "[jt:B0raID)DnXHAXR8/<j)Z-kp4?0B5pc$)G%:.3BZ=Y8m,t-ZZ9k<,P*RMD@jY$ZJD2Cv2fa*a@BA-j,eT-9I-x%(Dd;--3Vk$B0KZ-Ql.&4vK[>#QeDd*0-6q.kc:DC;cEglUoagt"
    "rKm-.@l^$+67?gLD*Uc*vu7h*faxG-s2UI8mwSe$We0a#Rt;8.1MYS.(0p[6l;r`*TUsW-nO0O`wcaIhk:MEdeG,W-0lx'&6N,@G,N`@.@xa'40R8-D1Z3>diTf,Dn&L^#s8w0p<I$4*"
    "v[Pk%`@rr$gj_=%.9_20AJsc)Vfv[-l<01X%t//1'DD]$B-aWuI5%1C:_Ce#k`pPWe_fw>ED3uH3TdA#Mb.<-njv5.Y9</ME+=&+9.xI%/1GouvSxG)g=%?da9$C#_+<i*LF#-OvrZ1M"
    "?)^fLSS&j%GN7al[sB#>W50JbwspL;.b?>Q>k*<-?`G1%ef=gLmYrG)_L%?P#DAc&w%iu4<,)5MV`$F<TLq^#Z_j=.e.Y)49]QW-cC.LPqrapuuQoH<OFX2Cx_@e*/oat-vn1d*NG8jK"
    "9*`3=)SGw$VI1N(/R;X6(8-F%2cWV?spJe$t'Fp.k5Z9CoRGqDE$DQNs^.Pduq,HdAP/F%OU(8'fJ6RaA%XD#XZ=W-4Q4L#rWOeHFQ1kbU,O/MS>1'@mXY9'f?$D'Ef2'MC*+(Mr_,o$"
    "D_0bRP,`A=h3hM<i@q[6Dg;hL/IXD#`Li;-lQQ5.a4Qq7m[M6j6Lq-M(E<NM4w7DCoj9o-X;c-+xp//1i,V49=V?-dD6Q'A7m`'4tuPW-'#dERH`s:UOah=%Hw@R*Yp:8C2#1Q8]H9u?"
    "%v:o8wTNYf./gfL80mo$5c18.NO$##+8=m$X`Cs-/UYI<.C2h)/j4Z6X1tM(EVY[/ML[H+[Et1'auh;-p.<M%*cq0#Y[.W-Uw@euneP;-lIZ.'PUf5#4`U(%PDbtC<aE,M9JXD#0O)gL"
    "kNiv[E,sFrKWGW-*Ha6E`hN>Awp//1Fu6>diZ<-M?N&;Q:M+^#20Bo%p8w%+5Wus-j;BG;jZU0av29s$qktnrU#41^/pp;-X+cn%,kf;-QWuk-C)dF<@Qg;.Avxa[QWZ;.Zj@gLZ)>r&"
    "w)loL(Da'Q3(``jG^CG)_eJ=B(t@X-YAe,,SuUW$'21HM*qT=Y7H9>dx^2gL0RGGMh8Ee#2F#W-9/-I->/xo:f(=2ChRbd2tlWY,LI$##-iL%6Nv@+4&P[7V'M#<-Yr4_$.4Wq.<oWD#"
    "Y*<+Nxs8e?kj5mh-oZUpmQqnDljWh=r<NtCFpG92)UucMMDad*$rcZ>C]KN(m_b'4vvd<CFc9X6Y7tM(X([sK`Wt1'E8DcuF4_t(0QCw*^k(t?)tK^#OiZ=Yk],W-12uKGmJ+gLi4oiL"
    "BMXD#h+Po$Bwra*a?iv>efU$BxB*huDvEr$3[Sb%INT29g_'^#<S`m/(^Q(#'.Y)4)c*C&f)g88oGl)4efVD3uLXX]h.X_$XtTfL4*x9..YiG;U.F2CF3Co%u'+q%)_3vML[?.%=q$gL"
    "OAHa$sc##:`J)rpC>:D.o/vG)Z#c29L65H3?2L%6?-QL%xaPZ.cs9h$f5c;-6WjU-5Zlk-l?wW_SStGMB1.AMwkn`*ODggL=3Uo-W`P)$b>ZX$<JF-ZuMW]+d^6r@aXhB#6IZV-XXTV6"
    "Zs)S_oLX_$o9]&FJtg>$Gp#_$ia%I#PQgIh)GHs-f4iEM_(0/1FVO;-nUT^=swvY61hZo$lXY'8$w<2Cg@<?.q`?.%]U%U:*<9Z-QlRfLx[nO(F9k-$5o0N(rbDW-*Fv03rnKb*qR%HD"
    "e0-Z$($U)Mb%>b$0gKW-LHn,6PIbn-JvC-d>f$,8ttr/:4*nY6akpA#?4We$4A0eZ1j4Z6u>KS%LQU)uV-Oc%/B[pLm)(-%T;6W--E*-=rl]R.6otM(mlV20oDsI3'Yj=.HQ,F%RBMhL"
    "$Uu]><xKb?+j#v*19PT.2;=M$RZU68aPF$K>vR^f6G8F-0ppf$t.Ga*Q19v7v<6O(WkQX-@%w9.CAtM(-$PU(4HcFd$ZL1KpVtZpWE$YOgQGGMWuSY,6.6t&,W[s.T@gG3D,i&dO?lw$"
    "3dT3CehS9CdaCw86]3>dc.g$'C5)W-d47Kstm,D-E.'0%?G5[9];A:Jqe;r7G7G)4+jXo$hEe052jp8.Bul=G;,Y58)X?dbx,W$9++EVn:0G)+ja4W-<SW-?dc;m/'wZ)4d`MD3U]2Y$"
    "9vsI3G#2dE1<cN)1N&;Q<L=2C/RUm)ZEt1'g*8>dwW)gL&CGGMGT^0)ESXV-a[ZV-guZ'=$#$Z$IP'gL3DFQdoRL/CXKl(H='ml$wEO-*rUkw5t*9&$#s>?&6_P1%5<:DC1C_*$?DpFC"
    "W$E:%)D'W?XkC)H_AsI3kf;(:jDkKl8;i8.2p98.7LC#$#;-<-,00_-+jEwgDW>vSam9h$OPBgL;buVC:=T9VZqO8I:/B+EI>T;..[Da*hrPa*(DZa*H$6^=Q8*IF,CGGMmuKgLU($t$"
    "cq7>de(X;.IYCs-8EB,;J7wY67o=c-k<l3D0LGGMq_9;=QJ$LGh2kTimWs;-O-`kF&6x1@R)Zg)s]p[6t)HD-.@]+/RsMF3=B'g4Obj=.qqiK=+ee%@qhP;-v_Sf$b%CJ#wb;2OOW%/M"
    "LMvu#(0c5/=s1/(P;[`*F:n29?oK#$Z(Yk%',]CQwD^8%/x13X4<Gk-4*3w9U8RtLSOj$#Tqn%#1Tl8.t$kD#.6H#>/K[0Y,ph<Ua#*4=ECZNt;j5/1@^0W-Lsp0>HD6<-J;:MWXCgr%"
    ">HIwBkT1nWV@gG3YZuvD`6hrCIrdT%/Gngu]K[DN)^U';#23YJY2SS[a*#]>?v]G3Vq(gL8[nO('bmJaF3Ix%M1.<-*$9;-C$G$%7@Ns-[YLxL'$k=G'Z+@.37;X-pahr^sSgE%*qJ$&"
    "uMW]+/4A6M<XkD#7cNj$@s`'4LXYw5tmqiB/o0^#cnkg$<s]a*VCfm/H7w1'bk>e#PTAZ$vWRa*jd<k;<2'fD0/_Aeq+sG)KF$##u7L%68#0i)C8vb%4*Ig)9,B+4CvYCFlw^>$o:9pI"
    "3Ut2Co,`P$lDj*%=XsM9irL/s$$0/1x7&v*dC3t-bu;qIt(-Z$Q_>Z%&.iMBq]3>daPms-P`2uJ%-83i*f/k$NIXD#,iD..-DXA>+vsY-.[/<-16aRU3x.qL@kxZ$)0D,&bI:0Ct2P3b"
    "X-Y&==PsbN['OB#Q5YY#'Nq-MDS*OsxuiRWM[BONfl>x%^l###NQqt5)Z?w'j''H<xoH#6*'2$%X/xY60HF:./5T;-Dk`i/6>0N(VuQ4C;G2j9t=X2CfNX`C`C4Au]xKH%qn,-u,s(3:"
    "JWb]u?Eb.@`;HK)i-.5/i@7YKu#Z%.e.qkL`*xY6B.-lLL@)Z%G.c$'*cq0#<j5/1KiVU1-aV;-#UxG)$J@;?-QiC$IqT=Ym%AjLZXGq:0WX&#'cBwgQrA,MoB),)%xw[-rda+KSIc^$"
    "M$Lk$=c5*#^v#`MMeA+MT'PsL6Z.R&R5r%4&xZJ*R_%<-u#Jn$x>Da<G+sR8YNEtC^Vks]cYrG)-F-oS1cmt1VCwW$4(HaO06./;xs*+H5_0a#@<(T.@t;8.48^V-vTU:@;-&oSj5Zk*"
    "dYrG)4om8.822HC#3%h%P9#^FF7k>-om:u?0*$Z$Jg>h)b9u`#e(G)4K2uxFAmYCFhl3>dx'S9%BYS]uTvxF-F*G?%e&>>dU>;4)UL-##n>&&$2U1@GZnH#6QCxddJ$nO(]kY)4S<3^%"
    "/0HnhhirG)9-_S#g:cK.ee(TdkMg;-cWX,6vD#&#L,>>#A:rkL)5x9._<wiL%@,gLEM-<.U.`8.BUkD#$r#gLT-4S;m(i3=R-#7CjsK^<nq<>d3^Nj$7S/Z>M@O2(gdDvA#[76%4cYV-"
    "@wiYMLI@2NX%g7N)tl:C0:Ce#P1%MHrZ4?eag#QJUY0WJ]CNC=t:.+4Pa)F3XL3]-D[kD#Xo+&+G&&n$if%&4;Hq:d(F1qL>:I]M_P&;Q0L?\?$/h?%=Oq>D$9j&;Qo6T9CI1>e#webl="
    "npJ%#4Duf*]Q@B-A.0_-#k&bceM`]$cih.X9wK&$cQ3Z6%YY8.musI3OCN,;m3>(-YEt1'GvL2CEwLl-5(W_&^+cw'QZtP8wdBM<gFh5%0E'1:Sc`vGr8X&%Bmf$MB#b'4G:j)E>h3>d"
    "0-9>d>KKT.8.7/1;1K;-;7pr-@g'v*ld(q.5eQV-pITj2#+p+MW]-lL*.x9.v&(O&L(LP8x0DDd[LnP&s3Ap.8:w0pHRFLG4_@e*oV#'O/H/wJc0YD#$:_S-a>ZX$E=Hg)Auq[6m=eC#"
    "'6V9C43C2C:T:o;V*e39L6o%u$EGM:x6kW88Hjd*?xK?A=6pLaWl7vcg=/a<$hD7SCIDdOA9Ot4_:wT%@&RJ(?+Rq$pegV6IXj=.U%N)<ApN>d+Y&[8-SbA#N:P5JZfB#$ZfrV%S_;J:"
    "fi,g)D<k8._kY)4;m$N0a/v[-F$nO(aNEjL%I?c*'m%K:F(6+WW,-Z$7ljxAq8X2C7u4qLfvagt41gKMI`GW-.SLgbNaGGIF<*>5(_#H)ULw;-+X$l$ZDI6:5X,l$_c?m/=4e193t<%="
    "msfi$6*bE5]q&WOY)5[$@xs`#__1iO)#+`>lP'L5XqBdFugDxLMlEKtr95MOAYiSaGu*F3Z`9X6aXE^-n^Z9Mk;oO(Jxc<-Q;Bc$A[E=l$p:m8d2O2Cw5tg*OX0Jdb:,N<btTO.`$(,)"
    "PK#Z6VXKk.]f9X6gNt_MN`tR-N.;2N--^r%?QA.V-h[?ph?`OTSZid*A($##K^3d*25_,M'>;8.D[kD#xm@X-%n9-m`3>Q/a+gIhj%sG)ITZ5JbMK?d[Dm=G/1(I#b6>##'7?$$lV.%#"
    "U'+&#olJ5;*lVs%eg;hLg9#A#:l*F3Jrd<_hfv1't#4Pd3p4Pd]j?gl<T(1.G86qL*(,k-J$'@'K5YY#A**##4ru8&O8[`*e==q.K`J&$tWnHmEu@P#Jx_KACc7p&'vb/C/qJ&ZMZLn="
    "0=KjE1NF:.u1L%6bDgG362MW$ogB^#/R.K9M(=2C&m3C&6`NrLUx&,(rS9p7owW>-:w)gLeoUD3(tBUB]3k=.BD0N(DI,^$c_Yw5w?gA$LJkGMT82G`-:$Z$F(kJ%4Wp;-%kKA8i0F2C"
    "T4,d$lA%]>f2-Z$>7fULpbYV-<dCg:)JC1LI,tY-f7*T.ZegV6EH#Y;v9L0c*lX]uKwunUZ;SS[q[VW#O)D9&'/-W-23-uQ[RC&,=_e;-Mhp;gBIi+8@u^N(&n@X-Q'@/CiSg;-_(G'%"
    "pk-YMN%tfJ9kTIV%'uhUEE$$$a#M$#$^jd=6Tx6%Yax9.)h-al*IuM(OGQ4C-8m8..[a19n47c%xK`*$I)kCe0N&;Q^4d;-@d-#q,^.#/A$(,)6)@k=kL3]-g&wH&N<MGM./B=l%5YY#"
    "kSuIUXGYw0*P18.:lbe+=#0i)+?KW-n8CF,(t@X-M2M3rVEt1'_1,gLP=2G`L$Nk;X?Gb%WN4>dD5KgLg:Yx>HNH:R$F+s''ssW@WkKN(%Mtu58P_mVH7*u$7Tk;-omN7%x6tg:N8q#$"
    "'-'U#wbWY$>Uc]uRv4qL7+J6M%VWj$+p%QJ-Obg3%P)Nr`8xY6FF3]-8BF:.MRTK%w=Hs_k8k=G$tn=G>DNJMS0MX%Is/kL2xQe%gb@<-Q+Wi-,8J-d'6pm1.HF:.n:xkT%u3>d/m=p&"
    "M9F>d;TlKc#E#Q8x*h>$f,,ki-9K'&cQ3Z6chqP%Y/YhW/SjX%%,tr&*9KW-,EnXCbpQ2KH,g:`/uaC=6>;s9>d7iLrQA+4uB:&$BdiA#nf_8.oDsI39xKgL;<8DCU?Y9C.f1Pds:.c<"
    "ASF&#Go.sT1)O@?5n;aG)-x9.JUg_-U[X-?XB/'dq$-&%c?i'8o=x%u*%g6<E6Sp7ingJ):/Ls-iRdlLbo@X-hpmA#C0I61_J@5$n&CJ#.jZK#'@Bc$%(`9@msL:Rfw)'#Xd&.$Wa`RS"
    "I?'W]`<N$#%xF)4%@9Q/Rm`'43QeIhFt*W-O>:uHm08o8a@=kO[A<r6D)L=QuxcG*el:4&m=<o9bhcBZuB%p.s]p[6X/G)4H0vl$<k=DCB3U8%#[B2C-^]t(_RimLJn.Y-[f8L,;3B*["
    "vjbs-n5``*^&:C.Gj>SnU5?h)jc;F3Y_^V6Y#bf:8VbA#Q__j%8P_Z@4<]8Uu]aIhNPO=Q)f^-6X&J`$B-jf:Z]GF#bW;M/@lX*$DaNrLs1I1Kc@X,%Our&-O;J3%BIJ=f*l1T.5NAX-"
    "t5>)%A[E=l?HO3E)i?=l_HK$K&m>>daBfa$;G_w^dQG_Jd?1a#a3u8MYw)iMDZkD#l^$:&Mp,>N7QJEdD%8jN+i?=lrpNq;iv%]%bLI_Smp$)*7#Kc$p-v&mFw:X6o*)W-[<A)4c4.T."
    "?@'p#j9[A-jHOR-$3Gj$-*9>dWGN&#LTt8.c'42'a=$gL**,L:;+*Z6FnsWU7%;8.>R=2C&sm2C?)%[#D;ea$?sdNNdklw.=WP1KG^fFEtTOKj/_YgLV6),)I:N/NirYV-q7q[6^.LLM"
    "-pMu$C.I:;BifUR.m[A,SAiA=1#b-=4;7&47kH)4aUkD#9eEs-J?aBI@g&I?KZ0W-n_-ka1/Ht$fREj9<_BN(-^p[6lbuWJ1%C^#Rm3jL5PO/Mjl6]-mrA_8'WI<qxSkNdDoa+&%KqYG"
    "lWkA#4vdN9#3pE@;O*20gDj6.q96,M$YYh%C%:/&(xsI52UkD#o/.Z>axN>de*loL.t7DC_AAM4?'ml$(VWEC,RAB&OUf5#hXMtD[GD(=>WN:IUX9Z&@&Yr$YgfX-Oi>ftd9::@`>q^#"
    "aQ3Z6'OJEdL>c;-VQ5Z%6T1d%TM[t(JLuFru:iU:25ipIUG?\?$H5=&#._HwBGxF)4+6:d%M4TA$]Xj=.u9@+4ikvY6tXOv%Y<(nu70t(9wQ=j*WMOa*HUag:tt-1*t7J#%Ux1<-&U^N'"
    "6_Na%VB8p.[x_A&n9h]uwnx+B:vK^#_.kFd6fWd$1^KW$<#2m&b]9qA[kI'615>a*7S<<AVv34D8#b8$lN)g;qa:4DY.S8@8QdZ$:J0N(C<XBI%0=2C#lT^#uSC5$CwJmaTh4F&1<:DC"
    "]'CJ#<=LxpjvS%#25^l8nU_@.v*;8.@[kD#_rQ#6&lo`*vVg,MokP@.pNn=G7D)4j>(i%MarS9C:[Xt(vN;a*<6m:B9&Mp/N>1A=8Ek<Sa=:P%`@ER)_vSa*O#0hLUtII%Pw`0j7:iT."
    "8@`G;>Y?4%J>uu#:B3uHUqQ]uCp>%.r;t2C/3kx%PIw[-abZV-B<>OM+xB`$RIr@C^buA%&5L9p@(i%Mt)jr=ioK#$]o)t%+>&&$R)m5/.vQJ(wpwi9mH#<.cUkD#G-DB#&S`du'?%LP"
    "j<Z+:N6o%u-;P$+R%os->'L-Mt$7&4seVN(HGSV6qD1qL3%T9CB[#m$oX&v.GV1/1I4]t(J0c3DGT]G;JX8R3L'ok9pG'N(`VSm_a/ZJ(>*X7%'YkgL=pC&ME]Rm-l<>*eFZ=.4CJ%Q-"
    "=V+58XrG,*@Sb503na3=2BBI$fq:X6ZDsI341NJPt?wmLS@v`-<,KwSwp//16lNh#cT[_8,I*1#KhFJ(SF5#%#d]_8S*N8.8l^GMo&PsL^TBX#87I>%CU1T@[6T;.*Q'H3Z*hfLasgSO"
    "WsA_/(*x9.(]J&$2JB,Mgq//15uHs-7A->N.Nrg*4RB<-tW%/MOJt_Mh#+$%grqM9$;P)4B.tH?=p0PdG:f/OwYaIhHta/%?2Y3FTLr=GQPlVnuYmx4%s5N(Xlo`*'S,E<KB>H3@[kD#"
    "1`KH#o$D8.,1_>$9?^/:b1.gs?[AT<h>b2CGts1'Dif]udDwx;NVb]u?g2'McVu)%VA4L,5k*<-B,Xb$wNG-d+VZs-nJpkLP#TfLX[+3&n-3o:N3%I-$LZ=&Lu@2CO.P#$>*4Z%Ce3>d"
    "1(=2CTQwGM:Y8F%@OU50MgF)4SMt,MQ+uK0o`0i)JV/a#Xc_e$D=+^GYvY*7h2kTib,Um)FoMBMCh3vLtHJEd$wX<-I>SU&x<bT.TDjc)o*?l$h[vNFEj]'A8Wu29o^tA##O]_d/g;n:"
    "V=7T%9t6gLV$2rA;S-uQ3/wdbkt.r8uMvTMr2(W-T25L#>b`)+UqLwI)e3n%.^J1)nsCX%W1@w#q/_'#2)`d;rw43D*39f$4rQo`#1qb*3^#t-OxKb*%X/68,GHth;;^a*J3Sa*r%c)N"
    "b0->>A=4KjA+OF3k02m$mkkP'A&^6Ed*u9)Z$UfL@>>d$^K[HM]lf#+)Q,HM;PKN%Tu_?#$lkE=BKXwpbYDD3i%E7&N#@)?gT67Awf<-=M4&v$u*B2C4Gh;-?0?l&)r$##]'mo7VVB)l"
    "_i=a$;bkD#B*N8.mAPq7W]3>dag[Y)HkP]u9Efq$cY.[KZ.l,Mo2Ed*t,_s-82X=I2l_v@Pt:H-Sr'P%5Stn'kug@SxYaq-xJGC&3nEW-@]P3)B[Yw5C(Tc0V$nO(Rt;8.5ff`**VMgL"
    "V*i%MoJk=GXHi?uOi_S#ELYbup/nA/>w@P#s)M*MHu=%&'$5W-<FBU^X/4Z6lQg_-RuJE#A(i%M]m=5$n&CJ#`s-=]m(hK-qx8;-wF5s-u5vcC5J<n2aGsI3f2&.MF'QJ(+raID&tE>d"
    "hf#CSMprHCV@+oLi6(MuhAB?.n5``*gK)gLcX&v5r6n2MbHMU%[#]GMh6qW$EgPdb8Nsd*le8n/@YDD3>HF:.]RBk.1j4Z6jAIuL];AouE>gk$wT%F$5ex5&2JV'Fo8D)+8L/T.g`v[-"
    "k-)m$sGeA#wca>$=d;c&,;2/1ZFe5/Bul=G8PAJH%MlBAxk?M&*H^s-VKE]FA,gPWKn3AF<1ViO_Qr5'bJds-MXDd*lfZp7'_JgkYM4^%$3k;-AS0W$,GaqIQd)Qhv2aHGsu'^u-g&.$"
    "x+iOBZR0N(c66)GMtk;J^6wlL5PXD#))$R(m*Ar8fmAUDnQ+T.$<>DCgci`$T(@68$(=2C:(n;-CSHw&W&:m8;0hWhsUev7w)LO*rY,@B0hNn%+wER*=n*W-n^K6L,Y,<-xS*S8$l*wp"
    "6/OCXcKEp^rmjR%oR#gL'J>f*(BUjBeuV;plSQe%V7P#$6PFc%^s]AOP^^P)s_b'4V$nO(Z1;jC*oHr:#:si-Liis9Xaj@Cmv3h=OFwT%.TB<-cQmr^w?Du&of,Xq)Kkj;26=dXZ7^-#"
    "NOi;-ugqG&T],iCl_pppCHh/?0>t>dIGi?u=CCq8'6JDt800/8FnlrBC&=+NQ9bJ-.>Xm$B:k)uYG3v%It4qL^'OvL$w//1Fb7XA/%OcS*C@d$i0#C-fu,-.;XDd*?v.01Gts1'=x.Bd"
    "mxBBuslD0>Z/Zg)sQ3Z6f=:lL.JJEd0mg;-d0WY$0Cj*%a<Wa*r$0Y=mqPEn&&TY,GqqNB5h`a*1G-NEs+tY-9lxlA`ZZ`*G>Ym*qO7<-O2uv02`rG)B_tX]`@f+%4'ee@sgE.DW?)-*"
    "fnLS.I=2xPF/EP(]_G)4fl`D3<^#A#=xs`#HADD3kax9.6gdm/QMp=G(l=DCqvn^$oCm=G8:=2C7PcsgmiE.&4C1B#igCZuiR]20^5Bq#+I4Pd-B):MWUf5#U'+&#k^''#Mu)T.VRG)4"
    "7fUWJ'>`B#Rr7&4v%qm%X24kO9jX2CCRK/XJvU6r7FD=-#$OvLExE+.su/kL&bamL'DAouHC@SnH'COaV-)T712V`3]$%##XF+F3XLtu5Hj@+4Amn,:`8kM(oNSs-g)kJMYY4k%L&pgL"
    "^TPd$<<]p7T)-Z$*^/7.:RPB=U*=2CKwE>d@3qq)WEdA#:i+^=Rn(T/b1*^oq1t;M_9+V&jhEW-$j5IQI_M-FkMFJ-&l4$%k))2h#T-0=WRCq7#VCupTuTn+%T*w?A&LQ;8_9&$1=d:="
    "OF9n*=3Pa*eTJW-OW'?A@o%e%]S;7&NYH042T&;QSd.@KD<gT%PUf5#HSM:%HxF)4BrLp$-K'H3o9wlLBUt`#/<4#&Oc(W-.*Nb>S'0Z$4m7h%ir7dX)3tJ'7sSns;BPEd4G`s-n@?j:"
    "[#r%eer8N%nw430&JjD#?Rt`#tAsB8_BFgC(I4AuEvtf%O]mZo0+J6M%7k=GU%<;?bE*tqUq@59A[NT.b?O&#G$O/'AYU7BS9kM(n6+<-jv6C[@7]R&8UW>d?_kuE;`kA#d_P_$($q2C"
    "RSv(%.OK/HOa(`JDv:Tjjv91&q;w%+h#8]%XOxp.)IuM((rm[PJppKW1iM1AaRIT.N_R%#/C6:r:NfX-i*-^$FTnO($b_[cn^kA#GGAc&goFa*H3U9.La4/1af72hr:>(F6$Nw$%YI%#"
    "kj9'#QQ[qJK&.[B+CY'A1MjD#>DoB.fER>#ZaSm8@rZIMnfR&G_+dduo[8x'K^K]M(Xt1'[bIEdWE>*Zt&bV$]X<6/Qqn%#0=id*5B1-MX?5,MQ$7&4I.P#$=qhpMmeSU&YOb[KHce'/"
    "prapu[p@_/ppkUR6Rgp935Qt$KP[<%pdcs%aXZGMO/pk9nGX2Cx3@DAE4,F%#Gll`UTVu9g$Q6sFk0Tp9=;?@^DSS[i'io.(T>G25)E^'[wT,&megV6lAha)dOQa*cb3-Mc)Lc*Y#v29"
    "KWkA#oips$=(n8.uQCA+39LV%ar1YTSHH)4b?1a#/j4Z6A+WcY1x.qLWvT=YB)XrT#r7DCwGc5/ab#I#8*;r9P=+KjwYaIh>fZfX76`KEA[9&$_?b8.]_G)4Bk8W-1(M4B@<=2C6%&Gl"
    "9,l1Wckd2&l9/x^w/[s?d/1xP6hV&vi)p%#XO=J%BvQJ(Yc9X6O`=o$/&:s7ob0^#Mk@+mSfn2.Fv4qLAs//1`f:>d]cDj0e1=b$%o]E#6jLEd`C,gsdLW<B-'=2CM'H&#ENK'&PUf5#"
    "N?@TDv%[#6#8nJ(%3'H3rJ>f*Ru2<-'&8.IA]Nm*G^FgL,Thv$w#@5$s0b'%pdbat2[tb*(X/q7mQ'N(<C^[8)t@X-WD?hGS0L=Qt(uA#tkFWA^KbG%eXX9C&<B7I<pK#$1`#A'Cr/N0"
    "VMXw5B`MD3GvN'O)PXD#4Jf0Y$-sTV>q+Q/u03/1UX4G`qAe&#UuIfL+bGGMt>55&Iv?D*U/sS_>jx6%:8jH,8XkD#xeo8W%x7DC0`&[/$WaIh?wAT%(Pt:C$'<b$xm79$1o]E#Wb-nM"
    "X;/r19Npl&Ip$)*]4pu,d3qHMIG`hL>5fX-xI,W-07=X(IG0HM`*sqL<C]^$vh/Bobj,[$/tZwBS[3>d2vR&#V'OY$bU.%#%DW4:3DPOg(^Zuos;UD&Cd#A#iQhG3ROBZ>BPi>nERL?d"
    "?37/ubuV60>YxH#3o''u)[uk^AjrQN'uv]S2%g^$])mF,=O)H*V6?=LOQ+<-gBT(/(=2/1S;MwKO/Y&O+'k=GSoqD.)dMGMRZb/)h&4-vUR'K1A(eX-=bdV-<shD34eSZ7EnT=Ya'7O4"
    "r8.&uIa@e*2KZdDRdB#$-r)<-A`MG%N$hG3UpV-MMIB$1E4/Z$Q<3a*VCb9.3RgIhcV_(),xcIhu-B=lrr(9.,i]`*o`c8.-IXD#0d,eZaxpkLb1H)4OUT6_rj%>MB/3b0=9W=Y$tn=G"
    "Fu/kLleI6%.oDZ/3uj+&XVjMKsMH?$Cme%#&3A=@(E&;iw;fL)gLQvRL#4#&8L/1'@)E1(4Y[R)1+J6Mk7N'QeW&%#AZ8gM6REi%CBKZ-q7q[6=a0WfGKXcmnTt(Mm_*c8E:I]MnOJEd"
    "VMFktZdFCd6,69.kpcc)HAW;.kgo<-0>+$/K`J&$pOg[Yd2=2C/D;*vg:KW&(Z7Au#l@u-7@[x*bV.9.4[6;-N#:9.Q?1a#D52aE7EWZ6L_j=.G(Dd*EMDT.sHHg)Dema$R>(X'2,3f%"
    "7PgIhH9)m)V;+KMQnT=Yi]`h#`?DgLG/ee4a@wF<N;kM(I2J^#HtTfL$JH)4JPjD#X02N(DxLp$C`f`*L5^gLw+J6MjX>W-Qk9dk>(i%MpEon$11lA#qEdL'hW`-#B7f:d$xF.,iw=2C"
    ";bca-`7RI.j59x9s6R_Q8d&;QM@)-u61O$MHgYuVx8h^#A**##P.xG)>*,/(jc;F3F<d>%@vQJ(KH4Pdo6T9CU-mBt$'o%MwUJEd8h$1pYuZp%=)d3#8(V$#:3>_JAMpnfWkOk4REv&#"
    "eYYJ$$<8F-9&LN*,*x9.)/Fk=(:_;Co2WZsFT=Z-*CJF.cP4kOT#4jLr$Us.Zv4/1N5S<hM'(M.l@f:dPpHGMwR?Q'Kv$)*)OE)4S^5Z6Uh61M>sa1)^k1gL9f*dZ]dwI-qCt]`6?Pk-"
    "NYG:;n80X:.?ru5.97bQGc'KMKCsI3Klr*/o<:DC3pmNX66b`ji^n,MFX6##'E`0$q/_'#pOM/)X4#.+q*eMM])BOM-)E6&&<_$6M`0i)A(eX-F<d>%5L4W8H^FQdGocG;lp,-ufeIf$"
    "7Co$'`U.%#E;N+MiOXD#mW_-6sk7g)68x^$A,[J(F:9_-X/<f+ajG[$-;]CFugd--f:<j0t16G`Q<OP&WKBD37):H3>HF:.BB$i$Fx4^#+89bBP?#'vkHg:dNb:3tSJ###njIw%`uYp8"
    "-hE>dA0G'o=SC1#LYqr$&1ei0o0ZS[`Olr-tgEM0l7@j9:5#w/F@uc-B&&XLH=9&$MJ>f*@*`W-LEJIi7Mf#.^6:A;1$*+jr/:B#pj@e#R%>sLPM?t.$XWECO=2I$jqT%+k6S3OQ2G>#"
    "LcDs#^cK:#EP*wpM=N*IQG/Jd#7b`jBKo%MWtpdSYkd##D$(,)sTFv-sQ]s7J]3>d+J$HM4w-:M%8-##Zj`#$8xbn*e:FgC:vlkgnZ@k=uW^2CaR%pA+t_LLB0pE@4-3oArSDBmDe#%("
    "&:k=.uLkA#tEu[$<BYpRI5$##8#[E0CI78%@6GJ(V=+wpD/;Z6eRo+M;C:p7KlWd=g#k=Goh@p.jSxG)H%F6_QtDg:v9hB#ZWkSI7Ac`jM;9X:6Smt'l'LI9ew^#$M1niLMw(hLT8hW$"
    "Y*N8.@[kD#$U_s:HmbZ-E@([I+8Fd=xcl&#,`f70T#NF3Z_OF38vKe$t+)T.u6lD#O:Mt-0p$5JSLD(=.@=2C5>[2C$]I6MrWaIhe^BI-Q[RxC+sX3FRlC$#hgS&Z2@4)#n?DjVVQ<nm"
    "4t6g)C.t`#8#LI$vQ)x5Ah<3rM-H)42aK)4B?v`-R*_$BX&(W-j,hEPU.C5%)lKHM5gP;-e'c$06?j,MS3@x%0Yho.ESXV-T9*gCH1T;.+GF-mT*N8.2ILTM^T/@#x>bH9PBQthGb4@'"
    "__M2B&h:tK%q//1_dea*Wk?hDGd&Z$ZuV/1WSW]+fh18.x)'/1s?;p$PR6B%_$3Z6&0x9.D7Wq.616g)a.*$6n*U/)iuY2C/LAp.e18G`PVE(v#x7DCZh<&o>HFQdVp#S8C3L^u4H4Pd"
    "(k8>Crs/F%2_V;-b)nA/(>uu#]<5)M+Pa$#@,>>#C(]ad<%lC.*HF:.9XmA%_vkTT#LE5arntl$A>Jb-@f#fVYc[0WU;X:'HPlG3Zlih$Okqb*;O.n1$####wf&.$'tn%#xe$oBeuQ#6"
    "4mwiLgb,T81**Z6wMGwT<U3>d(=lg%TBJ88hRsD%uc&V-/:0_-pp,f;WX0N($w*j9c^JVKbDSa$.f_ZR1N&;Q7*SAOBbw^$%o`pTY#Of-Ug0I$[>,f*Ko+q./1(I#n>P@@$Z5T%`O(t&"
    "x-&_SnG,N(#mw[>61=<S4IKDTZ7q1&SOP#$T:BsMe7^-#`R%T.MQd;#o-C9&qrxiL$`0g%XJ_p7u=9q^r(ah+j/Lh$eeV29#Y/U`1mB<-OqN4)`:)<-rXnX$68nQj`E?,MH;,f*BSUBO"
    "8*P`%..I)NtJf>-A+VK*HMGf*RS_p7NmxbIJ0fX--7;58J(>)4HuxA#lPDwK#BFdM$r//1ufG<-J3a]Js;$w$4iQW-,tjhbvG5&#iHmO(,4)b*xZ_p.Qp`'4a_A^Qp?K$.GW278^/nw'"
    "u9)I#iG5tTsh=5$MP,+#M[uh%_8Gs-Iv8s7]9T;.,@G^6%9QD-$h$q$nJa#$/e3nDGx:^oL1$##s6j;-i^u=8mX9N(XBs8.$u#Z$uAub7jiGW-(ZfH4T:EJ8wRL$B[2S,Moql&#rws`#"
    "4%xB&sWYm/.UkD#dD2]-<(33&HK@gL[S61%1jk,DdRnXH3>P)4`t_Oj$Um&%Ec69.Lo+^$1V/:;c=M$uxcT3C#4;k<*i>'$pZg:d`RdG;w,BJ1/WKW-;,Hq;;(5W?C/>)4Lk[W-H^k9B"
    "`m>&'L7>B#N1x1$+.ja%0QCw*?pb)N],Ve$Ov:D3:lr/3sAxHDE3UsfMXj=.9u+<.Zl*F3hgjGMusR*J2xc<-r&Uo-=^n/50UrsbSSZT/(BK?d=21/1k<pp.S/@D*=>evp'n@X-G83Z-"
    "G`h*%=SjD#3BZ=Y7q(;Q=SJ(MCG%Q-0`DNVO%7T%J5>###ntNF[Uf5#fWt&#c$(,)t9u`#.[>d2J=Hg)<IHg)Auq[61e0a#S.x<(LelS/q^0Z6,XWn<J_9bmOY;;?S=[IM?_[:1oqS9C"
    "@]]k+wJ`l*d*3HM<>Wd*WGXZ8Se6#6eT;[%<dBN(_T/i)cutl8ND'N(f%l/af20>G0Gk2C[hX=YcCpV%$?BE5JDY%M-+J6Mrp//1pFt&#3)]-#<^tM97DTZ-<BF:.J`q*%6b/g)3Ie;-"
    "&^iA.3Av1'`t8JrrDeA#/%=2C;=i587&s[]FR0&.+9$C#vY@)$F5>##V1tM(Q_e6/&>^6$]=bH9;&gcj63Mk+CDjc)[EUwK?adNkFN4H3ma:;?6LQFaq4-Z$^]rWF5abbEqjK#$#o$20"
    "ALASnN5[`*$eda$bl26/*U1N(ebTfLiv`0C=i8,&BWU8p&CGGMnxF_-Ok3I+4Q&;Qqx8;-$BQD-ruMrVoGq=U9'>Sn8h']>&sw],e0/<-hmL1.F^S`E;^,g)W#^G3&IE)4:q<J'#TkNd"
    "6.bx`+iEt(M0cW%K0TG%)o*o9lTUK*1(.#o/.pg%ka/W-&/h6<K/gKM;DexMX>bIA<=6?$&m:$#@v7B.cB:&$G]]<.H*CD3kax9.Dg:%Gee3>d_4f3C</ZGCV7R1CUw-;Q@DB0%Eh99)"
    "#F*nL2ulw5>EwY6&Ag;-1K2]$q$Ut1:E1qL-U]G;2f@<]%SWj$7(>kXNT%RN%KZ$$>Pd;#>C,1%[(OF3c&CCk+d1*4>IHg)t9u`#p@@#616na$kO)W-x)mKjtSNq-7d$:)`CdpKPfO:*"
    "R:lp7)d.E+WkQX-V3(30:=6g)kLtu5D*u9)XiG2CwN:><S@4AuWH3#.$aNrLIa0L&j8E#$w9as-getgLuL),)D1eX-<sCD3TsfG3Q(#dMQ6a7u2;$C#OYPs-U*M*MGW]G;LSimLY?Z0d"
    "fZ/%#Hm-w[t#a?9L_5P37^x?&)EsI3<-$,Mj];]&a0nu&x9)I#<-B=lD(IN9^>-XLAA:<-K$iH-4>6e'$X>kFk=iS%UfF,MWPXD#8vA_m>j'k&iGg;-OATn*q:&E<Pe4&uLd]Y#R$4T."
    ",;r$#a6:t%L0nO(Flo`*=i9T.Y_^V6+mKs$#Z?2Ci4D>d6qr&.KVl%MWEt1'Q5J'JwJ`l*@nv20<=KGM^OVS%%i96;(*x9.pu74C[iZ=Y-)w<C?5^Y#$tn=G%(_Qsks?D*[#uN(J%>&+"
    "36&p.*e.thpCtA#*FWo::[3>d.X#)td,lb$`B*edMHYKsWl;;?)Mac)bconfIN`'8bTdGM+FFg$28RGtwiq@-k5o%=OvBB#K)hSIx[f-6T8###<[ZV-X0fX-UPAB#k9Tu$u5dpLaP[M."
    "]Q,+#T$i>$>0Us.t@@Sn^bvG.h6)gLT-E6.sql1MaflwPbbL]$O)###GeKGM_9,,)7@8T.sYDD3+qE7%sr98.Bdic)O:Mt-qD1qLKa.Pd8pdS#ZY'w/iWhfu#$HHu]e37%#v(W-pGDPN"
    ")EQD-1s[>/+X?#6TIaG<IClBAZ#0i)019$.%jj7MkU>[.^IBHuTHU5%SOs'Mj[_%OMn8%#VhFJ(9INh#U1s1NY`,T8HwvY6u_TwL:Voq*2c>gLt8qZJT`tA#QvxNX,2X'.q'I`%G%]3F"
    "=:.Q'v52_A/3N,Mk5wxO>=1W$r>lQs,03Z$#;+g$*]@p.$$dIh^Zl2;hm8%#5jd(#Zs?=dk%^G3;oDg)VhpeV2XI1C0aaZ6`bGg)dW0/1X;BgLW,?l$GEN,M*E2G`eTA;?F4@a%Vt[dZ"
    "*%c_$4`Ek*'K,9._9,,)6Tu#6,T/i)(^Q5hVeH1M'IH>#_CF$Kmi=,M04Yg$hgZ=Y##;DC=[_19Yq7W-nV)0jJ:?>#7_bJr#<9G`l.YhL]sJfLnli21Lerr$hV)F3<ImO(G)4v/lo*-u"
    "^<F]urL)Q0aB%;QBK>;?,i]`*/5gQ/=pgb2Qe./1i0UZ-(kJEdBl+1,AVXq2mlNGM(Y?(v1-;hL@Bvl&d/0i)-jLEdv'HdbUEt1';)q;-:.TV->0DF7pdD?$;kJM'LNvE7[ax9.eUXD#"
    "8I(C&F?d;.<BR2Mhoq9Mwbg<M96pfLa/b^MaE?##U=i;-Tx6A-mO*B-0ea3M.(xC#o@MePE[(T7Ck61^p4JGM?U3>dAGw9;:2cEelZCG)QlRfL]=uM(T/Y)4I$_fL4@`9.(=8DCC16a*"
    "OqvDNxx.qL*DPx7X5BpT*o8k$23(B#Tk8>Ck.kFdROf7ux'8jL#hcgLWrgG3JMj;.b3Ab.4X7F.=UB2h0MiQMMP`iL))(SM%(2hL<qp]$qVT,.wPRw>HLt2Cksp=G&p:L>S`c;-p)e/8"
    "^lK#$el`a-#Nvdd)F1_A>q//1^UgQae6j$B_X[5/BK>;?eHPfLikYw5ta`)+q0Ak=9<(L>Xw+qtm'>e:eOF&#Jv)R<SM###0F=Z6G-I@d6#%T.=B=%=h)#cFsVX&#)pgw0&VU5'HqcA#"
    "9Z(Z6-K^oLDXaIh.Y#I?ZNH4K)QZ9MeqY:2[*_fLKlT1;nSMq2eXp92Is/kL?7mh/WXx9$Rej)#^*vB#VMXw5CQwY6=,/;M.9BHueK3)u)7bf-LnKx0M;###[]uM(t>ojLGxX?-UY#9%"
    "Ikfk=_u6g)#5co$P3e19r%j9;l6:Z-i89,;:XIvnUDkGN'Z+(.J,loLux(hLun.F/iLtu5@?6,MCHs]$)L,Q:n.=2CcDg;-Qc6=.>hFJ(OeiG3nl+b$tN(Sur1niLw]]#M<ZEnLL&G?-"
    "m]87.dhH1MR#/qLP@2G`-o%L>(&>s%oA3^#pp:__.N;x.$tn=G9-6kXMKiB#Oxs`#8*x9.`nhPj@:I]M^1I1Kunx_/u@BW8X8fjLC#8DC?W+@0NUK[0gM8gL-@VhLh[t`#b%>/M4C)Z%"
    "WqPl$Is/kL^FF3NY7Fc%=N]p7QwfG3;Ss+M2'xY6tQ)RUZ+R]-`weHm9q,Hdf+W9Cf]5AMskMY$^DtS&b(Y0cV%ZnEfqQAOoDCTM</'88jItM(0)-h-qldd+B1.AM[s4a:4B=2CKZ[gL"
    "F.;hL$_nO(H%:hL_HuM(cm;hLJTAv0_S`duk>^6$d*f%uj)wA#GMUh#T(;<-_1RA-Pwpi;:e<>d8.?:827o%u<m8h8ZT&:DI`)UDZ3H#%BRT9CaITt1`^$4$Sw]kL33.AM6u@O:=QaZ6"
    "5_0a#IMjD#sSp%&ebrMMRSs+&d66m4C`9X6/UkD#Yd`>$5<C2CN`-XC6N:6(IIo*&S7A'-Ma<r7dtsY-Vig05kTKdOsVaIh`/>W-;5BOVD$7^OUh*FN^LYGMK.;hLM'1kL3vQMM];I]M"
    ":L#I#YNH$8lvkA#=SDu$Z$UfL=)i%MkmP;-Z]$emu9ZEn?BngLXoYw5%$nO(bN?gLVLx>-GZ`UN<4<n%;AmN(ntvY6Bl.&40E0W$Rse9VoYb059DoA#kC(D.#<9G`a0j;-$jfj%G(5K<"
    "B[eC9M>X,O_X]70#sxXu-l`8$kQuQW%-XG>/?x9.h1G)4U%juLf$k=G`cM2C`:U;cO7sqL?_s$#5RG)4;7Lq@jFl)4p:'I#K:3Y.E[cdu6LqLMM7S>%m0@9^Qg@#6]Oju.@nmV-jP>#n"
    ".:]S9ExIoq]nXR-I:C`._ffG3YCSrLasS9CLC8qV&6aGMS,t76m9o'M]?fq$U0<W-&/-W-XU)NMc*4CM12wo$Ge]#6.o`B#5R#Z-rG5>dl6qA#SP](P[YtV$6IeG3%p#N(E($xL;.F_$"
    "4?0W-pA$BQBL*>NPW2#&mcGs-gq7494R4C/Hk@o8-+=2Cl`u8.$v-/(EcvP/CQwY6u+eJ(T>D2C*w8m8dI4$gbJ_697vK#$s$ju$[As?BTlCs-@G+8Num>q85Il'&_qd)N@8W$#g=,gL"
    "PYo;%>@p%uImoC%=e)UDXV,eQYlc;-.@?a$Ipc<UaHJW-t/@XU&hE^On?Ve$Axs`#aRqkLG65GM1F:=Nehv&86B2FRP'dA#4I'(tPJZp0+wqquVb-v:]<[W]0OuM(OhdV-?c8e?NG,;Q"
    "fW//1YJ#Q8CtP&QlAq-%+:dQs?PXw5ao5d.F<d>%AQKW$[Zm]uwsp=GdI(HMX*uk%w'+d*g0Sal/4As82-F2CsShlLlkP;-84qa>@[GhL&,F$89P?xK14U-M%PSZ$J)cj)v6wMMOxqqu"
    "3bHd$vxaZ%fZ.G7+kJb7Hx+a*cg$ENfo6:&n2t6*j/1g)EP1WS-6x9.Sp`'4GiYw.',i2'w6*2@w0j:M8sk;-%$[6NGo$q$EC.W-Gqp#%?A(A&90]5%jtgwK(A79gZolP9[:Jb.S3Np7"
    "44=2Cb_vP8-1hwKDe8#6A%.W-Xp^QC_[qp$r*ddu-Rc/NC/sFr:D#1%KA_K*JwhH-J#9<Tiw8?.X_Y)4=7I)4=C3/%?PQp.*c*9u-BujtY$cW%eP#<-/&Ef%`EwY6w5,T.Cr7&41QPg$"
    "O*sP8Gl:pJ(:xm*Bs,9.7H,;QE#URNwHs2M'@$a$eh%NMh[lT'5o_9MwaK&dGZR/&B/Fb7EKq5:@1.AMFWGp%3U3?%[)0S8>&'Z-Wm,t$u=jKctUa2CAK?-PW(8H=%pWd=F#f.M<IwE."
    "6n@X-iFv;-bcN^HV/PN`$Gj+&/l)?PkV*r79f0^#=NJ_%G'f58[l9LPI@;p7&Wx^fUq;p77YkA#eonm-pwpNO<&vwgWu(W-d&s5`2=+nMqqJfLs'^fL-s?/$>K*1#m6h-MJ*8d)F[P>%"
    "@O.Pd,3^V-%Mbd=Y('-u9u`*I$6U'#B)@ouX$w0%-cU0C=E$C#Hg*P(m@5H3IW`;&.F/?d7q.Pd=pgb235pGMp%Y`:FrAe?+gKqK/>Ciu.`2?9*usY-d74Z-M:d2VTbZ>#7;)I#A`NrL"
    "))k=Gdvhd-2V@e?_9j;-VJcw0#<9G`2P###>i`B#@*PZ6c[:8./6`hjsVaIhra/U0oWPw.uOI1Ke6j9`FIQs%Ek'L5O[9&$XQvA#`Lb?K;730aEnT=YrNi34;IL'JRt93$^^E=lqK2X8"
    "(CGGMk7mM(RrL9r+f?X-affG3a%/qLNIJEd.0e#$h@c)MEo`_:.MBJ`?IJ-*pIXw5%*x9.G+oi$EK3jLjsH&/t28G`le=O;=2#)%#q+9.Q)5G`F[2T&PN?e6qrxiL5O#A#>c?X-2,eJ("
    "U0W8.v6Z8&^)?bIIs/kLI6t+.oS#v*B/9T.b>^M#b*V.(C.t`#bJoN%wOt:CLhZ=YVB`19Hvj^oOchdXv*hb*Z6K6/@%eX-a:m68]Ik2C%Hr4%h*uG/FNuFrt(v(CsJF&#uX9^#7?fkF"
    "dE(,)3sN50ZZ)B#gPu%=746X1o'u%ulI$t$qA0X:29U2Cg.Q9`1i_9M,@%;Q'o@g:%x9B#b>2]-);Fn-fk$OFh7$,$cx+qt/lU[%I,>>#%]s<<G8kM(jpJH3RHL&$jG0O9biE>dowJHF"
    "]`OkLdcwq$P].R36lER3m/w%+mScW-6(Fk4oLtu5eER>#31*Y$$+ddu0up=G2e*T7;]@p.7l5/1oup6<:nPfL<B]c;O.]p&WKBD3^/:g)mw%)<XF'L5YT4>dFN4W-,6$?R)US/..-wiL"
    "mtY:ZeAvJ(VM8^$?&RJ(Rv=)4]F)UMJT0AMqr*'Mw)N'%8f@m/p:'I#vHfdsw@re$cS$R9%PO&#Dt4[$4+DT&T`sx+Pxip.&rw@#cXOaGe&#A#G.2kX:ECh>umf<Ch#e]uMO;mLs%^S#"
    "7;)I#4xBBud[E=l<lXq$e<8H;6.6K)U_?X-;PHs-rB8v7G@g9;-'I9rA:CgLx=+nM/$/qL/D6##]d:3;`aA%#W1*T.F<d>%*iJ;/5B8p.dvN/M&'(m8@Y.9(A(i%M4RJEd$OKfLJ^YW/"
    "Kqn%#L,>>#A3YZ6#.0a#:*hfLT9ZJ(BPg/)`<Vh>>'rFr0g:N9'.F2C2o(J97c#embfv1'R'i<J`euE$?8wv9[IF&#(03$$>K*1#%R-iLr#q]$RmwiL&#TfL+>uM(BSBgL>P]S#Cr/SM"
    "*p&;Q]n''u_M1Dd.Km;SN%dd$r_#$$vD(7#1L#@.beTA$p`f;%LGW+M_+J6MTbaIhlEPV='Nv9M=a-nMc#/qLC2NbMNOWU-];+19XANZ6;:Ju.,$0X:2M0N(bofG3O,sFrZT>2Cj&R]u"
    "ux[0YsUdL-*BkvL6(0/1fJX%+ht&a<fc*wpNb.<-.]mL-v'LS-<2A'.ls[m8-gGKb$?Bo%atWx#UUc*9hi8q^@<[&%fHL&$g8:QC6bkD#KffA$?fOB#t6R1CQYV2Mk3.AMWlw^$(kB2C"
    "?G1:Mm5<A+ZZ,Q/1Jsc)UB8p.cd49..[ZV-Dod3OPfCZuAM6nZtqP;-)+(T7vW8>dHn9@@+f?=l0b,X&)62@.XqNY$5q&T.t$kD#;xMi-*<.0FrOO_$We_GMPL?lN6M4o$6FvX&$xv;-"
    ",XGiXJH3R80Vmv$cQ3Z6W.1p$1@6eMRr7DCD^Ap7nah?Bv(vu:uqDe-qn,-ulHu[M#UJEdTw;;$J#v20<:r$#o&U'#hQoQa5o0N(Wv+W-JpZw'P3eDuPCH>#Gitb$cBO:R%d3>dErMm/"
    "Cwb7u/inSutLr9.+TD4u]M,Q8hZL$BJ5YY#lu.p?wq0wgvCuKP/uf7u)J3)u)j]&?CrvIU)5###-YK8'j2&X8?\?>Sn9;:8%>#K%6bPA_8>8#gLlFt1';6'9.KR-;QFld8.$n@X-vdQB#"
    "fbw@#o+(]N7Z;;?;XS_8U@5^#_E9(/^OVS%@1h+$3?AX-(p(;Q]A)-uithNtfHji-rhCF.2?j9DI%h--6V'?$o&uDOO?iSIFmNw9qVb*.7dIGMauhSI$^RRNRG$T.?nvV-,pN<97O=&u"
    "%&rfs_8/b-C#sY/26V-MNJ]S#Dx^`$H2G>#MC15%J/###9'JY-kEvL,(=t(3LH`Ihi-c-6^#2'owPQlJ/WL*mR)wX/(N#,28YR]4He,87Xp[i9i%6D<#1fu>3<?PACGo+DSRH]Fd^x7I"
    "tiQiK.u+DN>*[uPN55PS_@e+VoK>]X)Wn7[9cGi^InwCaY#Quckdkg):n.i)Tc-/$8wO>Dv`N:I^_((&oQAb.sb+0tW'7Y%Jl%oSP0=2C&mb&#v32n8fbtV/?Fw7I5bOuc#.xi'F9ei9"
    "I4Q]F,o4DN_((##/Lh%Ov?Ne$+'u_PQsdBXNJt6f$7@)RTm-T-YL*7P966xNB0*>P4kVIMmY=KMe-<iM&qP=p1ni6(.(GI#`v*_O_O&;QTcC1L)n0^#q(`-'7s4a*4&S#Pl7=J%;JD'."
    "6oT$I;U_O+`GqVQn^N$P/lXr$8eHv$KUC[-P&So1o*io.+gHT%aW9G;sbix=-nBS@=#s.CM.L`E^9&;HnDUlJ(P/GM8[_xOHg8SRXrh.Ui'B`W#3r:Z3>Kl]CI%G`STTxbd`.Setk^.h"
    "QZ*##<IXD#Z#G:.TCU;-bEr`$=,dF%F&RJ(FbBD3^k1dMxO7_#D[TfL9#6)OV66H3tqchLj/j.Mphr=-8(jFOa30oL1_`iLwSQs.eWmS/M;W;.wmL[0IxoA#<ot5vD7Ao),r_EHH$PjD"
    "O];;?Tukj$$aX5_tVaIhbU*m+2InAOVkl-M)]jFP@Av5MR39P9]/Hv$jitVM(P]S#g?kxM(/i$bRF9EYws-Bpm5_d%l5t&O*HhD&(0Q,2t,C;?&,###2n@X-$o%2'oKf/:-Ihk+97_k+"
    "GwUB#(K:;?`l/T7M>L'HCq.>-xv,D-R?)&.cq#U:ltSq'IKx>-Tp9W.XSXw5^8D]$#'F2C]oh>dKhCZu$h,`-Y=2Dd'ZGP&];X8&1`hG3-GUlLRK]S#;l;+O_GD6&QvX_/(F1CA0T<>d"
    "`UR/:[c<>dGio5==@6Jr6_Ns-ZO^%O^Zq#<5WQ-HL^J<T#.gfL?Sd>#WE+`$V9(9MRS.Pd6fAN-Dnls-=SJ(MB[fF-CI,k-n91_A:pW'8>p`'4NofA$tWGtL,D(Y]sgfb2rItA#&B<B%"
    "^%%?de^V;-<#RfL-hlw5KC8Z6,t4qL3wT=Ynhl)cE)5@M4PGGMu:$s$,8&Z-NLE#np4I1K+wI'd5aFLWXnd##v9TA$KRR-Hq61&uhJbh:#lt&#xt%T$K0w1%T[]p.uQwY615Ph#=2k;?"
    "[rJe$N6QtLJkQT-wW2]$NQrQNxwI,*9:2L&+j_W-i5m92h0Xt1cY$r:_,:'=ga):M^C'a+n=%fM^1nY6f.pM%21=2Co_Xg%H.l>$j-Sl?\?:ARN`l###OCS8)f4L%6QxCc`8[kD#`66:n"
    "Pv0[gaY;,<'`O&##&)q%oKrV.nRnHD2Ql3)5XkD#/-&b'XVkVLk6J>'2hJ&Zrcl&#r*wP(W>0gM,sV&.tvWd*,MEN0s3rI3u6lD#bpEd*$<2b*.bhp7+?uN)8$.b'?5RN0D8[S#v^B#$"
    ")7C<HpvsY-':+<->D()I^'uNF*[>tB3#AC6R+Nx*cNs58ip6W]_wZ##GhFJ(wQhG3/.p+M%?9_--(_%8f,5]#Z?TC/=pgb2j*4qLq@A8#d+bZ%^1l+D=00Z-$7rB8JBF:.4Pq&6I;oO("
    "_E@+4RsMF3>HF:.DrS9CFrQEdLu1G`7mrg%fA5W-.Ca*@n3%1CKcsT`-#f&#UuIfLEa>SnJ3KM'mQEM012V`3ESXV-ai`B#SVCl/RT$&4s9Zw53AW/1mg*P(t@.@#d9u`#Xx=G-ajYND"
    "/RGGMg/*I#,u@I.h,@:MbpjK=nj.RN_2Z'Jp%D0lZC61^j/>>dNwAgL#)2b=.k9[0mM1DddPA;$)A7H*i'io.aeT&#kE$T-X?tc%HFdq7aQgZ-ef%&4CvMF3jOr;-40[mLJw*2%?XGn<"
    "Kr@2C1D>`e6LM=-%W?df?tt)#$iFJ(pe(`$XtTfLR1%x%QOtY-nV26/Z0x9.e.5jL'xmGE6GC/#-^CD3k/oCsf,.uLuRud9Ubc2VO*1g%jI2Pd/[J+<ei<dXd#k=G<)?9.[Dm=GxkAq."
    "md0'#T<=wT+a2pI+1p5WRhj=.hwgA$V`G)%L#q8.$5NQd_hI>d.tvE@f@?G$*Z'-d7i]S:]Nt1'@[^`*[)(9.aOLS..x1gLmF?5.uxxiL^Mj6.a'mo72RQ?$<?QJ(IS/a#%[R'AfUSFa"
    "Xk7p&$X>2h7.>gsUEt1'Id;Au2'su$MY5a*Ls9<-/B,vG?TnO(hdJ)4FZV&.k-E.N:_0a#?H;7VB1PG-TqV1-GQ]G;J62X-W3u</048YKa'9<O/a@(#-:sP86],g)BG5edl_j=.PU9Z-"
    "4$bf:ewvY6keK`$Z$UfLbiS:JOR1_AGKlkg<ZH-d_3]ACi.vP8Xc7@u4>u)63U>1%h1$##OKPx7.s.Niu]PVHj@e0>*ai*%`Nl`Me.:$&)fF&#2B0399f^(#o/vG)PG<A+jc;F3<vQJ("
    "8=Hg);WCh$Nn6g)/&@r8U.fp&kVWECj(D/#gk>.u_)GK.7r0PdC*p&?x;.:0]9F&#G;oO(H*RMt6qgA$?)j;.*Nc6<XF/F,((8DCk:MEdJna5/d-[P$OXEZMrKJEd%vs>$cv+ktTY#=:"
    "ZuFv$1iYGMQ$ZV-DtuM(J%:hL)70;MfZ;;?eNMC#d+mBtl6jot#CK?do1_'#xwGv$4$;.&j^_c)Q(eF<`:mg)_kY)4#f-lLn*tu5,T/i)#XhG3PvQJ(F*nO(EO0I$cxL>dI)E9rggZ=Y"
    "OR`dus=.bMgY*m$'SxG)k40^uX#/Bd^qC$pFoMBM[KYCT7/q3&Rl68%QlRfLQ`sU--[0Z$]=M$u^AXj$tE31#Q5Y%#`W#3#v`0B#gZ.H#X%cQ#.0HV#r=YY#X)od#a=:n#*0Ox#/Aq+$"
    "@E^Q$C-X=$q4hF$u<wO$_F/]$,?Vg$-Do5%dPvv$O4N)%'oQ0%,In8%wr^B%i1UJ%t;9U%)gT^%eI-g%*#Hr%;@&&&oUH,&>lk2&@H^6&ufg>&kUQG&fBEl&PBeW&x&i_&pGrg&F#dn&"
    ")Amv&_o^''WtvK'.[q7'.>tA'7OHg'n+]P'aYRqLYx@+M&kABkgb=t?g8^N^/pXt-<Y]BXxFkTe8ml01WeR$GGn+bak5XBtPF0C4=<Zt?WF.bj[o^$Png911JGr0U?dwb*(u:COTbl0q"
    "TEE78i'_bWeT7Irko_I;7oih_j6f7/T(XIVKV>],>U)cN>j1%lJq2V7]@kIV3(ou$j8IoJJuS7o-=g%>[#3]cRKfo8PNf%PBQ^+ju7IJ;1^+>Zf[V>-(9_]GHpPcj@/I2108xoJ*xhof"
    "Fr+d3)-/&YMQ2Q'$E;vHPo&vmqPGp8Eu2p](hFW.JnV,NabP8o5bC^>CJLvZQQvp&&W@WIs)tJr`Xw,E8X3Qgq]UQ97K8EXOe+_#`r#qAM8BEXCl2EtAb#e<#s1'P;lR^ud4mQB_C#wm"
    "+^Kw6_0R3L<rp^c&&bq&M$_-<AjfEXOq9@$[Yp9AbOIwZfQc_#v-SkCPpuKiOsjw?(s&Fb]xWR0%#r'PQ@&Ft:>XXIIR0`#>>+xHMkp@$`)A(>f[*xZYXAFtrFZ:Ad%ZFX^q+(u$x%SB"
    "D4v_cNaP`,wlQ(PWq')#H]XxH/HT4qhZ_l:CL^xQW_O(u*4JSBrB#;]_.gS0g0nrJu%WlhaY@#.g<WSKq>1;f_NoM)QVC)5Rp(mCSX4)PXXHGbnX.5qi;qG+,/gG=+[;#I,OOAZl:vro"
    "M%/Z7OKnST.>cs&P8<gEBHC#[4v<B$4l$B?.=S)YFI76(X]/HFF;f5_pwsMr.)^g<,h>Z[_#ZH+kb:BH?<]#nhB_T9^,06U22,[%Z=-$I=*8Bm3q@n1uZ/tJ-,wsf1kIn1%%H*GmdOBd"
    "sSQh*T,wND%QqZe)`k$.-<,UKIZ=$nW7f$7bjn<Slx([npg)7:^9)CQ[Sh<oxsQU9X46O`w`)i*%%)+Ggx/=]T7H%%w$ibG;=$1jp6u[7vDf[RW$mnh3rtC-c&;uA3V=7h-/Ni*%%M+G"
    "1vm[e);o+,M.9iNbAYCmWQ*>/M`IoLnrC%nvadu8<+NoU&GM%n[jsu/?NK]IoR:V^3(6&.o2Z]@NHq+Y@Ki1s.*I>8>UbiW%/O,,QxWPMt:R]n`8Y81SfI8L`HRPi2*.^76&>&R[$D8h"
    "=Ss2*1cBpCjYUDmD`h2<5jdDd$Ih&7kF-W^o$NE$NMKW9N$l2WVPtJtscNWBlSCdlj>,w/U#CEH#59EZERdE-xp.-PVoY-#37%kEY[uvfN43k<h/PKkiv-_@9_nW^=#IX'[Yi?J(scKk"
    "kp_'@^*[9hvoLX06dFePs<4(%+^r'@K>e-cD##x8V$cqU=`0l*dY)(IRj[qqFUkGifX9r(VZdL4Ic8(@TilePO3A@]VE6@o0>Dx/`)C.G%H`RV57iksR50`7ZUW@Sl=]Lb2bHf5DQwRM"
    "H'2xoa.;Y9TuqFQ@MQ4jgqs4<QjGr_QlA;(^m9MF@@T(nX`ux8+APfYiKGflj9.m*Qcjf>1;BMXm2*Ao#.J;:#)cfYde(T)+R4s:HwPAJ;)&sUHrilj>=Ks1XhkYTX(%shGb75s@crA/"
    "vS:mEK(1mWNSL;hOH9/uR[=Z0[7#H?Vd.ZKSe2gYRSuYgM6bMtTn'<1j0%BA7sIsUDZ/ae^S.*%)fQa7>40HHY-&HZsvqGm6QON+ODEN=e]BTMv+Ws_18T#p>v90,Uofg>+t5BSZ:<Ti"
    "094n*[7PH?9m7<Ui?u/lJu%b.&6K<CQFQNXsj_gl>&_0,h[cH?7ZiZTR;GBf&(_$'$a6nE^9]HZdfn*%SutH?;g%[T=CE6s`*8[B#D-UMh3wNXX5K*eGo^Bo2@CC&0rv*7%MoI$g[aI-"
    "M'tb7Ve91GSAECSmX3=hnA?OtmaZ=1Dl*oE$^HC]dMIi#F4K1G6IZOb5@oC8FT=+esMOo3G5+]TI7r[p;xQi5/*rCSlmtUrvIPS.(J;4#2xAgL*XPgL4=`T.,Mc##D3xU.<R@%#;rG<-"
    "4LM=-U@`T.5rC$#[rG<-8X`=-irG<->F@6/o5>##_RFgLkW/%#+A`T.3xL$#?A`T.R,_'#MA`T.;F.%#$B`T.Bke%#4tG<-O('`4t61eG3r*cHRn;F='DvlE>nIL<q<GA-xRI^5,Beh2"
    "mKBk0pkn+H5-xF-8-IL2nnjI-Utu+#P)1/#OH4.#];L/#%R?(#Fc;..s)IqLjkP.#F@FV.kUG+#oG5s-huVVRo0;.M,D_kL^'LC-*tVE-EnVE-GH+_.uJ6(#spX?-8#D[-T?*1#'Xn#M"
    "$6$##?0@BON`D;?-Hig_(8P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#eEbA#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#/QHC#"
    "3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#SiAE#WuSE#[+gE#`7#F#dC5F#hOGF#l[YF#phlF#tt(G#x*;G#&7MG#*C`G#.OrG#2[.H#6h@H#:tRH#>*fH#B6xH#FB4I#JNFI#"
    "NZXI#RgkI#Vs'J#Z):J#_5LJ#cA_J#gMqJ#kY-K#of?K#srQK#w(eK#%5wK#)A3L#-MEL#1YWL#5fjL#9r&M#=(9M#A4KM#E@^M#ILpM#MX,N#Qe>N#UqPN#Y'dN#^3vN#b?2O#fKDO#"
    "jWVO#ndiO#rp%P#v&8P#$3JP#(?]P#,KoP#0W+Q#4d=Q#8pOQ#<&cQ#@2uQ#D>1R#HJCR#LVUR#PchR#To$S#X%7S#]1IS#a=[S#eInS#iU*T#mb<T#qnNT#u$bT##1tT#'=0U#+IBU#"
    "/UTU#3bgU#7n#V#;$6V#?0HV#C<ZV#GHmV#KT)W#Oa;W#SmMW#W#aW#[/sW#`;/X#dGAX#hSSX#l`fX#plxX#tx4Y#x.GY#'>YY#*GlY#.S(Z#2`:Z#6lLZ#:x_Z#>.rZ#B:.[#FF@[#"
    "JRR[#N_e[#Rkw[#Vw3]#Z-F]#_9X]#cEk]#gQ'^#k^9^#ojK^#sv^^#w,q^#%9-_#)E?_#-QQ_#1^d_#5jv_#9v2`#=,E`#A8W`#EDj`#IP&a#M]8a#QiJa#Uu]a#Y+pa#^7,b#bC>b#"
    "fOPb#j[cb#nhub#rt1c#v*Dc#$7Vc#(Cic#,O%d#0[7d#4hId#8t[d#<*od#@6+e#DB=e#HNOe#LZbe#Pgte#Ts0f#X)Cf#]5Uf#aAhf#eM$g#iY6g#mfHg#qrZg#u(ng##5*h#'A<h#"
    "+MNh#/Yah#3fsh#7r/i#;(Bi#?4Ti#C@gi#GL#j#KX5j#OeGj#SqYj#W'mj#[3)k#`?;k#dKMk#hW`k#ldrk#pp.l#t&Al#x2Sl#&?fl#*Kxl#.W4m#2dFm#6pXm#:&lm#>2(n#B>:n#"
    "FJLn#JV_n#Ncqn#Ro-o#V%@o#Z1Ro#_=eo#cIwo#gU3p#kbEp#onWp#s$kp#w0'q#%=9q#)IKq#-U^q#1bpq#5n,r#9$?r#=0Qr#A<dr#EHvr#IT2s#MaDs#QmVs#U#js#Y/&t#^;8t#"
    "bGJt#fS]t#j`ot#nl+u#rx=u#v.Pu#$;cu#)Juu#,S1v#0`Cv#4lUv#8xhv#<.%w#@:7w#DFIw#HR[w#L_nw#Pk*x#Tw<x#X-Ox#]9bx#aEtx#eQ0#$i^B#$mjT#$qvg#$u,$$$#96$$"
    "'EH$$+QZ$$/^m$$3j)%$7v;%$;,N%$?8a%$CDs%$GP/&$K]A&$OiS&$Suf&$W+#'$[75'$`CG'$dOY'$h[l'$lh(($pt:($t*M($x6`($&Cr($*O.)$.[@)$2hR)$6te)$:*x)$>64*$"
    "BBF*$FNX*$JZk*$Ng'+$Rs9+$V)L+$Z5_+$_Aq+$cM-,$gY?,$kfQ,$ord,$s(w,$w43-$%AE-$)MW-$-Yj-$1f&.$5r8.$9(K.$=4^.$A@p.$EL,/$IX>/$MeP/$Qqc/$U'v/$Y320$"
    "^?D0$bKV0$fWi0$jd%1$np71$r&J1$v2]1$$?o1$(K+2$,W=2$0dO2$4pb2$8&u2$<213$@>C3$DJU3$HVh3$Lc$4$Po64$T%I4$X1[4$]=n4$aI*5$eU<5$ibN5$mna5$q$t5$u006$"
    "#=B6$'IT6$+Ug6$/b#7$3n57$7$H7$;0Z7$?<m7$CH)8$GT;8$KaM8$Om`8$S#s8$W//9$[;A9$`GS9$dSf9$h`x9$ll4:$pxF:$t.Y:$x:l:$&G(;$+V:;$.`L;$2l_;$6xq;$:..<$"
    ">:@<$BFR<$FRe<$J_w<$Nk3=$RwE=$V-X=$Z9k=$_E'>$cQ9>$g^K>$kj^>$ovp>$s,-?$w8?\?$%EQ?$)Qd?$-^v?$1j2@$5vD@$9,W@$=8j@$AD&A$EP8A$I]JA$Mi]A$QuoA$U+,B$"
    "Y7>B$^CPB$bOcB$f[uB$jh1C$ntCC$r*VC$v6iC$$C%D$(O7D$,[ID$r+xEF#6hC%KGRR2<Cn-NXn).NYt2.NZ$<.N[*E.N]0N.N^6W.N_<a.N`Bj.Nhs]/Ni#g/Nj)p/Nk/#0Nl5,0N"
    "l2pjMYmdL2Wa&g2b4hwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK'diwK.@RhMw'3i2b4hwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK(giwK"
    "(giwK(giwK(giwK(giwK(giwK/CRhMx-<i2b4hwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK)jiwK0FRhM#4Ei2b4hwK*miwK*miwK*miwK*miwK"
    "*miwK*miwK*miwK*miwK*miwK*miwK*miwK*miwK*miwK1IRhM$:Ni2b4hwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK+piwK;hRhM&wSj2YA8FI-E:FI"
    "-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI-E:FI.KCFIZ+]88rX:d-$vQF%6fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN886fN888usS8"
    "rX:d-%vQF%7ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS87ojS89(9p8rX:d-&vQF%8x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p88x/p8"
    "8x/p88x/p88x/p8:1T59rX:d-'vQF%9+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K599+K59;:pP9rX:d-(vQF%:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9"
    ":4gP9:4gP9:4gP9:4gP9:4gP9:4gP9:4gP9%'4g2?Fwq$-?eh2$'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-4T`wK+7RhM%cQL2%'4g2$4.e-5ZiwK"
    "+7RhMtkmh2b4hwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK%^iwK,NTs-=@[hM&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N&p).N"
    "&p).N&p).N&p).N&p).N<V<7%0Q*i2&3bG3hZ;Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g23^9W-9giwKSAY-v"
    "2Tt;-CU<7%^n8KsZ/je3=3@Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9te/g2O0sZ9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9"
    "s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%2eN.N<V<7%^n8Ks]AJF4[o(N9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ksl$SR9s_&g22Tt;-CU<7%^n8Ks^Jfb4"
    "hZ;Ksl$SR9s_&g22Tt;-3JU5%M20?nl$SR9c_&g2x#t;-3JU5%M20?nl$SR9de/g2hjf(%(qvJao*&;8_G:&5q4`A5pl1e$-viwKo7n>[_Bj.N[vf(%(;SdFje`U8@gA,3Qdo;-dmf(%"
    "x^vJai[D:8;hA,33cq;-e$22%grd>[hR)u7+hA,32cq;-IWWU-&.iH-bX>1%&wDKam*]R9;hA,3e>p;-w+M*%f^_&vlw@79&iA,31Tt;-CR<7%^k/Kslt.r8eq]G3v#t;-3DU5%M,t>n"
    "XmdL2KSc;-PGW/%4M2Wfa`=(57p0W-%X:&vhX2u7hufG3Mqjr$HnAV8'ufG3Ntjr$HnAV8'ufG3FCwq$u>h2DiX2u79p]G3O>Hm/s6L=%KGRR2C0*q-BPSdFaiXC5s8YA5s8YA5E]ZA5"
    "s8YA5s8YA5s8YA5s8YA5Rg+W-8-:FIJUr;-^I>1%Gbju7dn]G3,<t;-?+I6%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%?W^/N"
    "&QU5%QDB?nkeM:8GEf;-7PU5%QDB?npBF4:eq]G3$$t;-lDY)%QDB?npBF4:eq]G3$$t;-qb+w$5WMW/jbM:8No]G3$$t;-7PU5%QDB?npBF4:fwfG3`BS<:eq]G3$$t;-7PU5%vDq2D"
    "lniU8*;Z/:eq]G3$$t;-7PU5%QDB?npBF4:eq]G3$$t;-7PU5%0s''do9+o9E$#d3iVp;-'P@+%Aj,KNkhV:8[),d3Dnjr$?.nt7'xxc3$;6W-?G9RN?.nt7((,d3Dnjr$?.nt7'xxc3"
    "Vm4W-t?:&vo?4o9C),d3[F#l$?.nt7d](e3VV)WH`Yce3iuQF%<,(W-JnRpKY&3i2%w*g2%w*g2aA,W-9^iwK7mt;-K'08%fWL&vZ&3i2aA,W-9^iwKM6B?7Y&3i2%w*g25=%eFp?4o9"
    "B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFlqrU8s##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3=%r;-QGW/%.YxdFp?4o9C),d3wxt19E$#d3"
    "Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9B##d3Udo;-j#g(%.YxdFp?4o9C),d3trL*%Udo;-j#g(%"
    ".YxdFp?4o9B##d3Udo;-j#g(%.YxdFn-S79%$#d3Udo;-j#g(%.YxdFn-S79=$#d3NUr;-j#g(%/jk&dp?4o9B##d3Udo;-j#g(%.YxdFm$8r8(%#d38mt;-L*08%In/;8B)T&vm$8r8"
    "(%#d39v9W-R0SpKt)08%g^U&vm$8r8(%#d38mt;-L*08%g^U&vm$8r8n$#d3)E9W-v=;pplN<7%_k&Ksm$8r8v$#d3fHs;-$9o3%.db&dm$8r8E$#d3Unr;-j-22%.db&dm$8r8E$#d3"
    "E=r;-YxJ0%t'Yp^m$8r85$#d3E=r;-YxJ0%o8I'vumgL;)+,d3/(9X;(%#d3@mt;-TB08%o8I'vn'8r8kCvM9(%#d3:mt;-TB08%qVj?%p?4o9/xxc3B)l;-TB08%UdYW8thF59U[Bqp"
    "umgL;n$#d30<t;-D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;n$#d30<t;-D7I6%_R@qpumgL;o*,d33&M*%:GjwKa4m;-vtF<%MdfpK[+c4%Tk[E5x':oD''o2V6+pGM/u779"
    "KN%gLlA^P9luZw'<h4:2&5-W-w?QWSkt779,d.F%*>L,3-+of$AL/79A.,F%l$1F%9d*F7V&#d3OBLkFaPYD4fYu`4_J:&523VR<o<#x',@n-NWhv-N(<QD-R<QD-R<QD-R<QD-R<QD-"
    "R<QD-R<QD-R<QD-S?QD-l?TOM1jZL2B-f;-gOK0%Sa&g2e5-'?@nC*NgFwq$@Cjq8L/,F%=`e&d@nC*N6J#l$p-G59rUJe4VgZL2ODxfLtjv^8<cA,3tQ2gLk)TNM*iv-Nnjv-N9s779"
    ";kJ,3?Cwq$'Pp9VZ[sJs?e(eM?aVU8]xZw'C;c;-gsJc$En44:5iJ,3o[fZ$sFDF7kO#gL;=/W86vZw'+h-na4ZF,3-C*W-l>7q9HxOp7vTZHdoS'g2.M'gLs`Cq7,cA,37qn;-Xu/k$"
    "Db]R91.,F%Y[xpK@nC*N4[bg$JBUL;V-,F%MMG,36C$gLD7ev:K#^,3<.7LM-;+o95iJ,3],6X$@[93i(M#<-LqE884xZw'KA:j9thA,3@v3gLtxlm9^%[w'`t6q9[>8+4las;-+db4%"
    "0c+eFX-7r7`(>)4Bt$gL60Wb$f@pK<?e(eMnOX4:_vZw'Uw@)4Bt$gLib-q$rIf-d2Qnh2W+D)4/Xn;-3>U5%M&b>nln`:8g->)4Url;-3>U5%M&b>nln`:8g->)4u,9W-:^iwK&2JdF"
    "Z,<i2W-/WSkeDu7l,>)4u'n;-4E[#%21$?IrNX4:viJ,3RQ#9%Bt$gL4v5X$4GvVfrNX4:gi4R*AOwK<@nC*N?^'`$fLhwKBt$gLQoXl8+,>)46[*W--_JWArNX4:_vZw'tsj;-J]'`$"
    "fLhwKBt$gLtI'`$Cs5KNrNX4:gi4R*QFh&QrNX4:gi4R*#,h;-J]'`$I0Y4:V-,F%2SZ/:_vZw'=v^/:_vZw'`8g;-J]'`$a:,3Dkn`:86->)4Bt$gLQN`m$f@pK<[>8+4xh#gLX+09:"
    ">vZw'+J`32@nC*NTFY[$E+V_Axh#gL.[)'%xh#gL60iF:>vZw'hGY/:>vZw'cp&Q8#,>)48M-W-$?04V8=P)NKc)'%h>04VX-7r7x[.4VPRCq7x[.4V@nC*NLVUh$V;@WAp3AV8e5P)4"
    "ANO&dp<]r8@.G)4B=r3<-iJ,3^gBW$E[/:DUDxfLn3Vn;vw],3GORhMJG:p$5EMR<0?*gL@kb5:<xZw'_.:3i@nC*NE=Sn$8bRkXvd)gL,K^s$oK/W-fJB?n8=P)N5?q-%ajE3V[>8+4"
    "5M'gLL,%v8+,>)4X](W-,<I'v8=P)NA$Go$Hl3dX3BWb$I0Y4:8jJ,3Igd.%g?^Ks?e(eM3vi:8e5YD4iVp;-aI>1%GXNu7q1YD42cq;-LG4_$S2W+=6-,F%wFoP9d40k$%@;*>l/;3V"
    "k'fn9$BpK2b78(#$,Guu(SUV$_*e@^p$###"};
static_assert(Terminuz20_compressed_data_base85.size() == 60095);

void create_font(ImGuiIO& io, gsl::czstring name, f32 size, gsl::czstring data)
{
    // Build texture atlas
    ImFontConfig cfg;
    cfg.GlyphOffset = ImVec2{1.0f, -1.0f};
    cfg.OversampleH = 2;
    xr_strcpy(cfg.Name, name);

    auto f = io.Fonts->AddFontFromMemoryCompressedBase85TTF(data, size, &cfg);

    // add remap for local rus
    for (ImWchar i = 0; i < 256; i++)
        f->AddRemapChar(0xc0 + i, 0x410 + i);

    // Ёё
    f->AddRemapChar(0xa8, 0x401);
    f->AddRemapChar(0xb8, 0x451);

    // №—
    f->AddRemapChar(0xb9, 0x2116);
    f->AddRemapChar(0x97, 0x2014);

    // ҐґЄєІіЇї
    f->AddRemapChar(0xa5, 0x490);
    f->AddRemapChar(0xb4, 0x491);

    f->AddRemapChar(0xaa, 0x404);
    f->AddRemapChar(0xba, 0x454);

    f->AddRemapChar(0xb2, 0x406);
    f->AddRemapChar(0xb3, 0x456);

    f->AddRemapChar(0xaf, 0x407);
    f->AddRemapChar(0xbf, 0x457);
}
} // namespace

void themes_init()
{
    auto& io = ImGui::GetIO();
    create_font(io, "Main", 14.0f, Terminuz14_compressed_data_base85.data());
    create_font(io, "Large", 20.0f, Terminuz20_compressed_data_base85.data());

    active->apply();
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
