#ifndef __XRGAME_EMBEDDED_EDITOR_MAIN_H
#define __XRGAME_EMBEDDED_EDITOR_MAIN_H

#include "../xr_3da/IGame_Persistent.h"

namespace xr
{
namespace detail
{
class imgui_ingame_editor : public xr::ingame_editor
{
    RTTI_DECLARE_TYPEINFO(xr::detail::imgui_ingame_editor, xr::ingame_editor);

private:
    bool show_main{true};

    bool show_console_window{};
    bool show_console_vars_window{};
    bool show_gameobject_editor{};
    bool show_hud_editor{};
    bool show_position_informer{};
    bool show_sound_env_editor{};
    bool show_waypoint_editor{};
    bool show_weather_editor{};

    bool show_imgui_demo{};
    bool show_imgui_metrics{};
    bool show_imgui_guizmo{};
    bool show_imgui_themes{};

    bool moused_was_captured{};
    bool b_isAltHold{};
    bool b_isShiftHold{};
    bool b_needResetInput{};

    xr_map<xr::key_id, s64> i_key_holding;

    Fvector2 mouse_pos{};
    f32 mouse_shift_sens{0.4f};

    enum class EditorStage : s32
    {
        None,
        Light,
        Full,
        Count,
    };

    EditorStage curr_stage{EditorStage::None};
    EditorStage target_stage{EditorStage::None};

    [[nodiscard]] bool IsEditorActive() const { return (curr_stage == EditorStage::Full && !b_isAltHold) || (curr_stage == EditorStage::Light && b_isAltHold); }

    void RenderMainWnd();
    void RenderMainMenu();

public:
    imgui_ingame_editor();
    ~imgui_ingame_editor() override = default;

    [[nodiscard]] bool opened() override;
    void update() override;

    [[nodiscard]] bool key_hold(xr::key_id key) override;
    [[nodiscard]] bool key_press(xr::key_id key) override;
    [[nodiscard]] bool key_release(xr::key_id key) override;

    [[nodiscard]] bool mouse_move(gsl::index dx, gsl::index dy) override;
    [[nodiscard]] bool mouse_wheel(gsl::index dir) override;

    [[nodiscard]] bool script_mixer() const override;
    [[nodiscard]] bool script_time() const override;
    [[nodiscard]] bool script_weather() const override;
};
} // namespace detail
} // namespace xr

#endif // !__XRGAME_EMBEDDED_EDITOR_MAIN_H
