#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_main.h"

#include "embedded_editor_console.h"
#include "embedded_editor_console_vars.h"
#include "embedded_editor_guizmo.h"
#include "embedded_editor_helper.h"
#include "embedded_editor_hud.h"
#include "embedded_editor_pos_informer.h"
#include "embedded_editor_sound_env.h"
#include "embedded_editor_themes.h"
#include "embedded_editor_weather.h"

#include "UICursor.h"

#ifdef XR_IMGUI_LUA
#include "ai_space.h"
#include "script_engine.h"
#endif

#include "xr_level_controller.h"

#include "../xr_3da/xr_input.h"

namespace xr
{
namespace detail
{
namespace
{
constexpr u32 AutoRepeatDelay{300};
constexpr u32 AutoRepeatRate{15};

constexpr std::array<std::pair<gsl::index, ImGuiKey>, 109> imgui_key_arr{{
    {DIK_TAB, ImGuiKey_Tab},
    {DIK_LEFT, ImGuiKey_LeftArrow},
    {DIK_RIGHT, ImGuiKey_RightArrow},
    {DIK_UP, ImGuiKey_UpArrow},
    {DIK_DOWN, ImGuiKey_DownArrow},
    {DIK_PRIOR, ImGuiKey_PageUp},
    {DIK_NEXT, ImGuiKey_PageDown},
    {DIK_HOME, ImGuiKey_Home},
    {DIK_END, ImGuiKey_End},
    {DIK_INSERT, ImGuiKey_Insert},
    {DIK_DELETE, ImGuiKey_Delete},
    {DIK_BACK, ImGuiKey_Backspace},
    {DIK_SPACE, ImGuiKey_Space},
    {DIK_RETURN, ImGuiKey_Enter},
    {DIK_NUMPADENTER, ImGuiKey_KeypadEnter},
    {DIK_ESCAPE, ImGuiKey_Escape},

    {DIK_APOSTROPHE, ImGuiKey_Apostrophe},
    {DIK_COMMA, ImGuiKey_Comma},
    {DIK_MINUS, ImGuiKey_Minus},
    {DIK_PERIOD, ImGuiKey_Period},
    {DIK_SLASH, ImGuiKey_Slash},
    {DIK_SEMICOLON, ImGuiKey_Semicolon},
    {DIK_EQUALS, ImGuiKey_Equal},
    {DIK_LBRACKET, ImGuiKey_LeftBracket},
    {DIK_BACKSLASH, ImGuiKey_Backslash},
    {DIK_RBRACKET, ImGuiKey_RightBracket},
    {DIK_GRAVE, ImGuiKey_GraveAccent},

    {DIK_CAPITAL, ImGuiKey_CapsLock},
    {DIK_SCROLL, ImGuiKey_ScrollLock},
    {DIK_NUMLOCK, ImGuiKey_NumLock},
    {DIK_SYSRQ, ImGuiKey_PrintScreen},
    {DIK_PAUSE, ImGuiKey_Pause},

    {DIK_NUMPAD0, ImGuiKey_Keypad0},
    {DIK_NUMPAD1, ImGuiKey_Keypad1},
    {DIK_NUMPAD2, ImGuiKey_Keypad2},
    {DIK_NUMPAD3, ImGuiKey_Keypad3},
    {DIK_NUMPAD4, ImGuiKey_Keypad4},
    {DIK_NUMPAD5, ImGuiKey_Keypad5},
    {DIK_NUMPAD6, ImGuiKey_Keypad6},
    {DIK_NUMPAD7, ImGuiKey_Keypad7},
    {DIK_NUMPAD8, ImGuiKey_Keypad8},
    {DIK_NUMPAD9, ImGuiKey_Keypad9},

    {DIK_DECIMAL, ImGuiKey_KeypadDecimal},
    {DIK_DIVIDE, ImGuiKey_KeypadDivide},
    {DIK_MULTIPLY, ImGuiKey_KeypadMultiply},
    {DIK_SUBTRACT, ImGuiKey_KeypadSubtract},
    {DIK_ADD, ImGuiKey_KeypadAdd},
    {DIK_LSHIFT, ImGuiKey_LeftShift},
    {DIK_LCONTROL, ImGuiKey_LeftCtrl},
    {DIK_LMENU, ImGuiKey_LeftAlt},
    {DIK_LWIN, ImGuiKey_LeftSuper},
    {DIK_RSHIFT, ImGuiKey_RightShift},
    {DIK_RCONTROL, ImGuiKey_RightCtrl},
    {DIK_RMENU, ImGuiKey_RightAlt},
    {DIK_RWIN, ImGuiKey_RightSuper},
    {DIK_APPS, ImGuiKey_Menu},

    {DIK_0, ImGuiKey_0},
    {DIK_1, ImGuiKey_1},
    {DIK_2, ImGuiKey_2},
    {DIK_3, ImGuiKey_3},
    {DIK_4, ImGuiKey_4},
    {DIK_5, ImGuiKey_5},
    {DIK_6, ImGuiKey_6},
    {DIK_7, ImGuiKey_7},
    {DIK_8, ImGuiKey_8},
    {DIK_9, ImGuiKey_9},

    {DIK_A, ImGuiKey_A},
    {DIK_B, ImGuiKey_B},
    {DIK_C, ImGuiKey_C},
    {DIK_D, ImGuiKey_D},
    {DIK_E, ImGuiKey_E},
    {DIK_F, ImGuiKey_F},
    {DIK_G, ImGuiKey_G},
    {DIK_H, ImGuiKey_H},
    {DIK_I, ImGuiKey_I},
    {DIK_J, ImGuiKey_J},
    {DIK_K, ImGuiKey_K},
    {DIK_L, ImGuiKey_L},
    {DIK_M, ImGuiKey_M},
    {DIK_N, ImGuiKey_N},
    {DIK_O, ImGuiKey_O},
    {DIK_P, ImGuiKey_P},
    {DIK_Q, ImGuiKey_Q},
    {DIK_R, ImGuiKey_R},
    {DIK_S, ImGuiKey_S},
    {DIK_T, ImGuiKey_T},
    {DIK_U, ImGuiKey_U},
    {DIK_V, ImGuiKey_V},
    {DIK_W, ImGuiKey_W},
    {DIK_X, ImGuiKey_X},
    {DIK_Y, ImGuiKey_Y},
    {DIK_Z, ImGuiKey_Z},

    {DIK_F1, ImGuiKey_F1},
    {DIK_F2, ImGuiKey_F2},
    {DIK_F3, ImGuiKey_F3},
    {DIK_F4, ImGuiKey_F4},
    {DIK_F5, ImGuiKey_F5},
    {DIK_F6, ImGuiKey_F6},
    {DIK_F7, ImGuiKey_F7},
    {DIK_F8, ImGuiKey_F8},
    {DIK_F9, ImGuiKey_F9},
    {DIK_F10, ImGuiKey_F10},
    {DIK_F11, ImGuiKey_F11},
    {DIK_F12, ImGuiKey_F12},
    {DIK_F13, ImGuiKey_F13},
    {DIK_F14, ImGuiKey_F14},
    {DIK_F15, ImGuiKey_F15},

    {DIK_WEBBACK, ImGuiKey_AppBack},
    {DIK_WEBFORWARD, ImGuiKey_AppForward},
}};
xr_unordered_map<gsl::index, ImGuiKey> imgui_key_map;

void imgui_key_map_init()
{
    imgui_key_map.reserve(imgui_key_arr.size());

    for (auto& pair : imgui_key_arr)
        imgui_key_map.emplace(pair);
}

void ImGui_UpdateKeyModifiers(ImGuiIO& io)
{
    io.AddKeyEvent(ImGuiMod_Ctrl, !!pInput->iGetAsyncKeyState(DIK_LCONTROL) || !!pInput->iGetAsyncKeyState(DIK_RCONTROL));
    io.AddKeyEvent(ImGuiMod_Shift, !!pInput->iGetAsyncKeyState(DIK_LSHIFT) || !!pInput->iGetAsyncKeyState(DIK_RSHIFT));
    io.AddKeyEvent(ImGuiMod_Alt, !!pInput->iGetAsyncKeyState(DIK_LMENU) || !!pInput->iGetAsyncKeyState(DIK_RMENU));
    io.AddKeyEvent(ImGuiMod_Super, !!pInput->iGetAsyncKeyState(DIK_LWIN) || !!pInput->iGetAsyncKeyState(DIK_RWIN));
}

void ImGui_ResetKeys()
{
    auto& io = ImGui::GetIO();

    io.AddKeyEvent(ImGuiMod_Ctrl, false);
    io.AddKeyEvent(ImGuiMod_Shift, false);
    io.AddKeyEvent(ImGuiMod_Alt, false);
    io.AddKeyEvent(ImGuiMod_Super, false);

    for (gsl::index i{ImGuiMouseButton_Left}; i < ImGuiMouseButton_COUNT; ++i)
    {
        if (const auto btn = gsl::narrow_cast<s32>(i); ImGui::IsMouseDown(btn))
            io.AddMouseButtonEvent(btn, false);
    }

    for (const auto& keys : imgui_key_map)
    {
        if (ImGui::IsKeyDown(keys.second))
            io.AddKeyEvent(keys.second, false);
    }
}

void TextRight(gsl::czstring text)
{
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x - 4.0f);
    ImGui::TextUnformatted(text);
}
} // namespace

imgui_ingame_editor::imgui_ingame_editor()
{
    imgui_key_map_init();
    xr::detail::themes_init();
}

void imgui_ingame_editor::RenderMainWnd()
{
    const xr::detail::ImguiWnd wnd{"Ingame Editor"};
    if (wnd.Collapsed)
        return;

    ImGui::Text("ImGui ver: %s", ImGui::GetVersion());

    if (ImGui::Button("Console", ImVec2{-0.01f, 0.0f}))
        show_console_window = !show_console_window;

    if (ImGui::Button("Console Vars", ImVec2{-0.01f, 0.0f}))
        show_console_vars_window = !show_console_vars_window;

    ImGui::Separator();

    if (ImGui::Button("GameObject Editor", ImVec2{-0.01f, 0.0f}))
        show_gameobject_editor = !show_gameobject_editor;

    if (ImGui::Button("WayPoint Editor", ImVec2{-0.01f, 0.0f}))
        show_waypoint_editor = !show_waypoint_editor;

    if (ImGui::Button("Weather Editor", ImVec2{-0.01f, 0.0f}))
        show_weather_editor = !show_weather_editor;

    if (ImGui::Button("HUD Editor", ImVec2{-0.01f, 0.0f}))
        show_hud_editor = !show_hud_editor;

    if (ImGui::Button("SoundEnv Editor", ImVec2{-0.01f, 0.0f}))
        show_sound_env_editor = !show_sound_env_editor;

    ImGui::Separator();

    if (ImGui::Button("Position Informer", ImVec2{-0.01f, 0.0f}))
        show_position_informer = !show_position_informer;

    ImGui::Separator();

    if (IsEditorActive())
        TextRight("Input mode: CAPTURE");
    else
        TextRight("Input mode: OVERLAY");

    ImGui::Text("SHIFT sens:");
    ImGui::SetNextItemWidth(-0.01f);
    ImGui::SliderFloat("##sens", &mouse_shift_sens, 0.1f, 1.0f);

    static xr_vector<f32> frames;

    // Get frames
    if (frames.size() == 100) // Max seconds to show
        frames.erase(frames.begin());

    const f32 framerate = ImGui::GetIO().Framerate;
    frames.emplace_back(framerate);

    ImGui::Text("TPS %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::PlotHistogram("", frames.data(), gsl::narrow_cast<s32>(std::ssize(frames)), 0, nullptr, 0.0f, 300.0f, ImVec2{-0.01f, 100.0f});
}

void imgui_ingame_editor::RenderMainMenu()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    ImGui::Checkbox("Main", &show_main);

    if (ImGui::BeginMenu("Engine"))
    {
        ImGui::MenuItem("Console", nullptr, &show_console_window);
        ImGui::MenuItem("Console Vars", nullptr, &show_console_vars_window);

        if (ImGui::MenuItem("Stats", nullptr, psDeviceFlags.test(rsStatistic)))
            psDeviceFlags.invert(rsStatistic);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        ImGui::MenuItem("GameObject Editor", nullptr, &show_gameobject_editor);
        ImGui::MenuItem("WayPoint Editor", nullptr, &show_waypoint_editor);
        ImGui::MenuItem("Weather Editor", nullptr, &show_weather_editor);
        ImGui::MenuItem("HUD Editor", nullptr, &show_hud_editor);
        ImGui::MenuItem("SoundEnv Editor", nullptr, &show_sound_env_editor);

        ImGui::Separator();
        ImGui::MenuItem("Position Informer", nullptr, &show_position_informer);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("ImGui"))
    {
        ImGui::MenuItem("ImGui demo", nullptr, &show_imgui_demo);
        ImGui::MenuItem("ImGui metrics", nullptr, &show_imgui_metrics);
        ImGui::MenuItem("ImGuizmo", nullptr, &show_imgui_guizmo);
        ImGui::MenuItem("ImThemes", nullptr, &show_imgui_themes);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

bool imgui_ingame_editor::opened()
{
    curr_stage = target_stage;

    if (curr_stage != EditorStage::None)
        return target_stage != EditorStage::None;

    if (b_needResetInput)
    {
        ImGui_ResetKeys();
        b_needResetInput = false;
    }

    if (moused_was_captured)
    {
        GetUICursor()->SetUICursorPositionReal(mouse_pos);
        moused_was_captured = false;
    }

    return target_stage != EditorStage::None;
}

void imgui_ingame_editor::update()
{
    ImGuizmo::BeginFrame();

    static u32 lastFrame{};

    if (Device.dwFrame == lastFrame)
        return;

    lastFrame = Device.dwFrame;

    if (curr_stage == EditorStage::None)
    {
        if (b_needResetInput)
        {
            ImGui_ResetKeys();
            b_needResetInput = false;
        }

        if (moused_was_captured)
        {
            GetUICursor()->SetUICursorPositionReal(mouse_pos);
            moused_was_captured = false;
        }

        return;
    }

    XR_TRACY_ZONE_SCOPED();

    b_needResetInput = true;
    b_isAltHold = !!pInput->iGetAsyncKeyState(DIK_LMENU) || !!pInput->iGetAsyncKeyState(DIK_RMENU);
    b_isShiftHold = !!pInput->iGetAsyncKeyState(DIK_LSHIFT) || !!pInput->iGetAsyncKeyState(DIK_RSHIFT);

    auto& io = ImGui::GetIO();
    const bool is_editor_active = IsEditorActive();

    if (io.MouseDrawCursor != is_editor_active)
    {
        io.MouseDrawCursor = is_editor_active;

        if (is_editor_active && GetUICursor()->IsVisible())
        {
            POINT pos;
            GetCursorPos(&pos);

            mouse_pos.x = gsl::narrow_cast<f32>(pos.x);
            mouse_pos.y = gsl::narrow_cast<f32>(pos.y);

            io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);
            io.MousePos.x = mouse_pos.x;
            io.MousePos.y = mouse_pos.y;

            moused_was_captured = true;
        }
        else if (moused_was_captured)
        {
            GetUICursor()->SetUICursorPositionReal(mouse_pos);
            moused_was_captured = false;
        }
    }

    RenderMainMenu();

    if (show_main)
        RenderMainWnd();

    if (show_imgui_demo)
    {
        ImGui::SetNextWindowPos(ImVec2{650.0f, 20.0f}, ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&show_imgui_demo);
    }

    if (show_imgui_metrics)
    {
        ImGui::SetNextWindowPos(ImVec2{650.0f, 20.0f}, ImGuiCond_FirstUseEver);
        ImGui::ShowMetricsWindow(&show_imgui_metrics);
    }

    xr::detail::ShowImGuizmo(show_imgui_guizmo);
    xr::detail::themes_picker_show(show_imgui_themes);

    xr::detail::ShowConsole(show_console_window);
    xr::detail::ShowConsoleVars(show_console_vars_window);

    xr::detail::ShowWeatherEditor(show_weather_editor);
    xr::detail::ShowHudEditor(show_hud_editor);
    xr::detail::ShowSoundEnvEditor(show_sound_env_editor);

    xr::detail::ShowPositionInformer(show_position_informer);

#ifdef XR_IMGUI_LUA
    if (!pSettings->line_exist("engine_callbacks", "ingame_editor_update"))
        return;

    if (sol::function function; ai().script_engine().function(pSettings->r_string("engine_callbacks", "ingame_editor_update"), function))
        function(show_main);
#endif
}

bool imgui_ingame_editor::key_press(gsl::index key)
{
    if (key == DIK_F10)
    {
        target_stage = EditorStage{(std::to_underlying(target_stage) + 1) % std::to_underlying(EditorStage::Count)};
        return true;
    }

    if (!IsEditorActive())
        return false;

    if (get_binded_action(gsl::narrow_cast<s32>(key)) == kCONSOLE)
        show_console_window = !show_console_window;

    auto& io = ImGui::GetIO();
    ImGui_UpdateKeyModifiers(io);

    switch (key)
    {
    case DIK_F10: break;
    case MOUSE_1:
    case MOUSE_2:
    case MOUSE_3:
    case MOUSE_4:
    case MOUSE_5: io.AddMouseButtonEvent(gsl::narrow_cast<s32>(gsl::index{ImGuiMouseButton_Left} + key - MOUSE_1), true); break;
    default:
        if (const auto conv = imgui_key_map.find(key); conv != imgui_key_map.end())
            io.AddKeyEvent(conv->second, true);

        if (const auto ch = pInput->DikToChar(gsl::narrow_cast<s32>(key), false))
            io.AddInputCharacter(ch);

        i_key_holding[key] = Device.dwTimeContinual + AutoRepeatDelay;
        break;
    }

    return true;
}

bool imgui_ingame_editor::key_release(gsl::index key)
{
    if (!IsEditorActive())
        return false;

    auto& io = ImGui::GetIO();
    ImGui_UpdateKeyModifiers(io);

    switch (key)
    {
    case DIK_F10: break;
    case MOUSE_1:
    case MOUSE_2:
    case MOUSE_3:
    case MOUSE_4:
    case MOUSE_5: io.AddMouseButtonEvent(gsl::narrow_cast<s32>(gsl::index{ImGuiMouseButton_Left} + key - MOUSE_1), false); break;
    default:
        if (const auto conv = imgui_key_map.find(key); conv != imgui_key_map.end())
            io.AddKeyEvent(conv->second, false);

        i_key_holding.erase(key);
        break;
    }

    return true;
}

bool imgui_ingame_editor::key_hold(gsl::index key)
{
    if (!IsEditorActive())
        return false;

    switch (key)
    {
    case DIK_F10:
    case MOUSE_1:
    case MOUSE_2:
    case MOUSE_3:
    case MOUSE_4:
    case MOUSE_5: break;
    default:
        if (auto iter = i_key_holding.find(key); iter != i_key_holding.end() && Device.dwTimeContinual > iter->second)
        {
            if (const auto ch = pInput->DikToChar(gsl::narrow_cast<s32>(key), false))
                ImGui::GetIO().AddInputCharacter(ch);

            iter->second = Device.dwTimeContinual + AutoRepeatRate;
        }

        break;
    }

    return true;
}

bool imgui_ingame_editor::mouse_move(gsl::index dx, gsl::index dy)
{
    if (!IsEditorActive())
        return false;

    if (b_isShiftHold)
    {
        mouse_pos.x += gsl::narrow_cast<f32>(dx) * mouse_shift_sens;
        mouse_pos.y += gsl::narrow_cast<f32>(dy) * mouse_shift_sens;
    }
    else
    {
        mouse_pos.x += gsl::narrow_cast<f32>(dx);
        mouse_pos.y += gsl::narrow_cast<f32>(dy);
    }

    clamp(mouse_pos.x, 0.0f, gsl::narrow_cast<f32>(Device.dwWidth));
    clamp(mouse_pos.y, 0.0f, gsl::narrow_cast<f32>(Device.dwHeight));

    ImGui::GetIO().AddMousePosEvent(mouse_pos.x, mouse_pos.y);

    return true;
}

bool imgui_ingame_editor::mouse_wheel(gsl::index dir)
{
    if (!IsEditorActive())
        return false;

    ImGui::GetIO().AddMouseWheelEvent(0.0f, dir > 0 ? 1.0f : -1.0f);

    return true;
}

bool imgui_ingame_editor::script_mixer() const { return xr::detail::s_ScriptNoMixer; }
bool imgui_ingame_editor::script_time() const { return xr::detail::s_ScriptTime; }
bool imgui_ingame_editor::script_weather() const { return xr::detail::g_ScriptWeather; }
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
