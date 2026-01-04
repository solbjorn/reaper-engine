#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_pos_informer.h"

#include "embedded_editor_helper.h"

#include "Actor.h"
#include "ai_object_location.h"

#include <fstream>

namespace xr
{
namespace detail
{
namespace
{
string256 section_name{"section"};

void SavePosition(gsl::czstring sect)
{
    const auto path = xr_string{FS.get_path("$app_data_root$")->m_Path} + "//position_info.txt";
    std::fstream informer;

    informer.open(path, std::fstream::app);
    if (!informer.is_open())
        return;

    informer << "[" << sect << "]\n";
    informer << "position = " << Actor()->Position().x << ", " << Actor()->Position().y << ", " << Actor()->Position().z << '\n';
    informer << "direction = " << Actor()->Direction().x << ", " << Actor()->Direction().y << ", " << Actor()->Direction().z << '\n';
    informer << "game_vertex_id = " << Actor()->ai_location().game_vertex_id() << '\n';
    informer << "level_vertex_id = " << Actor()->ai_location().level_vertex_id() << "\n\n";
}
} // namespace

void ShowPositionInformer(bool& show)
{
    if (!show)
        return;

    if (Actor() == nullptr)
    {
        show = false;
        return;
    }

    const xr::detail::ImguiWnd wnd{"Position Informer", &show};
    if (wnd.Collapsed)
        return;

    auto actor_position = Actor()->Position();
    auto actor_direction = Actor()->Direction();
    auto actor_game_vertex = s32{Actor()->ai_location().game_vertex_id()};
    auto actor_level_vertex = gsl::narrow<s32>(Actor()->ai_location().level_vertex_id());

    ImGui::InputText("section name", &section_name[0], sizeof(section_name));
    ImGui::InputFloat3("position", &actor_position[0]);
    ImGui::InputFloat3("direction", &actor_direction[0]);
    ImGui::InputInt("game_vertex_id", &actor_game_vertex);
    ImGui::InputInt("level_vertex_id", &actor_level_vertex);

    if (ImGui::Button("Save"))
        SavePosition(section_name);
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
