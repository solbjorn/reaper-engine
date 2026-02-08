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

constexpr std::array<std::pair<xr::key_id, ImGuiKey>, 119> imgui_key_arr{{
#define XR_KEY(k, i) {xr::key_id{sf::Keyboard::Scancode::k}, ImGuiKey_##i}
    XR_KEY(A, A),
    XR_KEY(B, B),
    XR_KEY(C, C),
    XR_KEY(D, D),
    XR_KEY(E, E),
    XR_KEY(F, F),
    XR_KEY(G, G),
    XR_KEY(H, H),
    XR_KEY(I, I),
    XR_KEY(J, J),
    XR_KEY(K, K),
    XR_KEY(L, L),
    XR_KEY(M, M),
    XR_KEY(N, N),
    XR_KEY(O, O),
    XR_KEY(P, P),
    XR_KEY(Q, Q),
    XR_KEY(R, R),
    XR_KEY(S, S),
    XR_KEY(T, T),
    XR_KEY(U, U),
    XR_KEY(V, V),
    XR_KEY(W, W),
    XR_KEY(X, X),
    XR_KEY(Y, Y),
    XR_KEY(Z, Z),
    XR_KEY(Num1, 1),
    XR_KEY(Num2, 2),
    XR_KEY(Num3, 3),
    XR_KEY(Num4, 4),
    XR_KEY(Num5, 5),
    XR_KEY(Num6, 6),
    XR_KEY(Num7, 7),
    XR_KEY(Num8, 8),
    XR_KEY(Num9, 9),
    XR_KEY(Num0, 0),
    XR_KEY(Enter, Enter),
    XR_KEY(Escape, Escape),
    XR_KEY(Backspace, Backspace),
    XR_KEY(Tab, Tab),
    XR_KEY(Space, Space),
    XR_KEY(Hyphen, Minus),
    XR_KEY(Equal, Equal),
    XR_KEY(LBracket, LeftBracket),
    XR_KEY(RBracket, RightBracket),
    XR_KEY(Backslash, Backslash),
    XR_KEY(Semicolon, Semicolon),
    XR_KEY(Apostrophe, Apostrophe),
    XR_KEY(Grave, GraveAccent),
    XR_KEY(Comma, Comma),
    XR_KEY(Period, Period),
    XR_KEY(Slash, Slash),
    XR_KEY(F1, F1),
    XR_KEY(F2, F2),
    XR_KEY(F3, F3),
    XR_KEY(F4, F4),
    XR_KEY(F5, F5),
    XR_KEY(F6, F6),
    XR_KEY(F7, F7),
    XR_KEY(F8, F8),
    XR_KEY(F9, F9),
    XR_KEY(F10, F10),
    XR_KEY(F11, F11),
    XR_KEY(F12, F12),
    XR_KEY(F13, F13),
    XR_KEY(F14, F14),
    XR_KEY(F15, F15),
    XR_KEY(F16, F16),
    XR_KEY(F17, F17),
    XR_KEY(F18, F18),
    XR_KEY(F19, F19),
    XR_KEY(F20, F20),
    XR_KEY(F21, F21),
    XR_KEY(F22, F22),
    XR_KEY(F23, F23),
    XR_KEY(F24, F24),
    XR_KEY(CapsLock, CapsLock),
    XR_KEY(PrintScreen, PrintScreen),
    XR_KEY(ScrollLock, ScrollLock),
    XR_KEY(Pause, Pause),
    XR_KEY(Insert, Insert),
    XR_KEY(Home, Home),
    XR_KEY(PageUp, PageUp),
    XR_KEY(Delete, Delete),
    XR_KEY(End, End),
    XR_KEY(PageDown, PageDown),
    XR_KEY(Right, RightArrow),
    XR_KEY(Left, LeftArrow),
    XR_KEY(Down, DownArrow),
    XR_KEY(Up, UpArrow),
    XR_KEY(NumLock, NumLock),
    XR_KEY(NumpadDivide, KeypadDivide),
    XR_KEY(NumpadMultiply, KeypadMultiply),
    XR_KEY(NumpadMinus, KeypadSubtract),
    XR_KEY(NumpadPlus, KeypadAdd),
    XR_KEY(NumpadEnter, KeypadEnter),
    XR_KEY(NumpadDecimal, KeypadDecimal),
    XR_KEY(Numpad1, Keypad1),
    XR_KEY(Numpad2, Keypad2),
    XR_KEY(Numpad3, Keypad3),
    XR_KEY(Numpad4, Keypad4),
    XR_KEY(Numpad5, Keypad5),
    XR_KEY(Numpad6, Keypad6),
    XR_KEY(Numpad7, Keypad7),
    XR_KEY(Numpad8, Keypad8),
    XR_KEY(Numpad9, Keypad9),
    XR_KEY(Numpad0, Keypad0),
    XR_KEY(NonUsBackslash, Oem102),
    XR_KEY(Menu, Menu),
    XR_KEY(LControl, LeftCtrl),
    XR_KEY(LShift, LeftShift),
    XR_KEY(LAlt, LeftAlt),
    XR_KEY(LSystem, LeftSuper),
    XR_KEY(RControl, RightCtrl),
    XR_KEY(RShift, RightShift),
    XR_KEY(RAlt, RightAlt),
    XR_KEY(RSystem, RightSuper),
    XR_KEY(Back, AppBack),
    XR_KEY(Forward, AppForward),
#undef XR_KEY
}};
xr_unordered_map<xr::key_id, ImGuiKey> imgui_key_map;

void imgui_key_map_init()
{
    imgui_key_map.reserve(imgui_key_arr.size());

    for (auto& pair : imgui_key_arr)
        imgui_key_map.emplace(pair);
}

void ImGui_UpdateKeyModifiers(ImGuiIO& io)
{
    io.AddKeyEvent(ImGuiMod_Ctrl,
                   pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LControl}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RControl}));
    io.AddKeyEvent(ImGuiMod_Shift, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LShift}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RShift}));
    io.AddKeyEvent(ImGuiMod_Alt, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LAlt}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RAlt}));
    io.AddKeyEvent(ImGuiMod_Super,
                   pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LSystem}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RSystem}));
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
    b_isAltHold = pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LAlt}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RAlt});
    b_isShiftHold = pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LShift}) || pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RShift});

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

bool imgui_ingame_editor::key_press(xr::key_id key)
{
    if (key == xr::key_id{sf::Keyboard::Scancode::F10})
    {
        target_stage = EditorStage{(std::to_underlying(target_stage) + 1) % std::to_underlying(EditorStage::Count)};
        return true;
    }

    if (!IsEditorActive())
        return false;

    if (get_binded_action(key) == EGameActions::kCONSOLE)
        show_console_window = !show_console_window;

    auto& io = ImGui::GetIO();
    ImGui_UpdateKeyModifiers(io);

    if (key.is<sf::Mouse::Button>())
        io.AddMouseButtonEvent(s32{ImGuiMouseButton_Left} + std::to_underlying(key.get<sf::Mouse::Button>()) - std::to_underlying(sf::Mouse::Button::Left), true);

    if (const auto conv = imgui_key_map.find(key); conv != imgui_key_map.end())
        io.AddKeyEvent(conv->second, true);

    if (!key.is<sf::Keyboard::Scancode>())
        return true;

    if (const auto ch = pInput->DikToChar(key.get<sf::Keyboard::Scancode>(), false); ch != 0)
        io.AddInputCharacter(ch);

    i_key_holding[key] = Device.dwTimeContinual + AutoRepeatDelay;

    return true;
}

bool imgui_ingame_editor::key_release(xr::key_id key)
{
    if (!IsEditorActive())
        return false;

    auto& io = ImGui::GetIO();
    ImGui_UpdateKeyModifiers(io);

    if (key == xr::key_id{sf::Keyboard::Scancode::F10})
        return true;

    if (key.is<sf::Mouse::Button>())
        io.AddMouseButtonEvent(s32{ImGuiMouseButton_Left} + std::to_underlying(key.get<sf::Mouse::Button>()) - std::to_underlying(sf::Mouse::Button::Left), false);

    if (const auto conv = imgui_key_map.find(key); conv != imgui_key_map.end())
        io.AddKeyEvent(conv->second, false);

    if (key.is<sf::Keyboard::Scancode>())
        i_key_holding.erase(key);

    return true;
}

bool imgui_ingame_editor::key_hold(xr::key_id key)
{
    if (!IsEditorActive())
        return false;

    if (key == xr::key_id{sf::Keyboard::Scancode::F10} || !key.is<sf::Keyboard::Scancode>())
        return true;

    if (auto iter = i_key_holding.find(key); iter != i_key_holding.end() && Device.dwTimeContinual > iter->second)
    {
        if (const auto ch = pInput->DikToChar(key.get<sf::Keyboard::Scancode>(), false); ch != 0)
            ImGui::GetIO().AddInputCharacter(ch);

        iter->second = Device.dwTimeContinual + AutoRepeatRate;
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
