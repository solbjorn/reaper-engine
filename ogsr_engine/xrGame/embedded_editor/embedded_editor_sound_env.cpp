#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_sound_env.h"

#include "embedded_editor_helper.h"

#include <mmeapi.h>

#include <../eax/eax.h>

namespace xr
{
namespace detail
{
namespace
{
constexpr std::array<std::string_view, EAX_ENVIRONMENT_COUNT> env_names{"GENERIC",     "PADDEDCELL", "ROOM",    "BATHROOM",  "LIVINGROOM",      "STONEROOM", "AUDITORIUM",
                                                                        "CONCERTHALL", "CAVE",       "ARENA",   "HANGAR",    "CARPETEDHALLWAY", "HALLWAY",   "STONECORRIDOR",
                                                                        "ALLEY",       "FOREST",     "CITY",    "MOUNTAINS", "QUARRY",          "PLAIN",     "PARKINGLOT",
                                                                        "SEWERPIPE",   "UNDERWATER", "DRUGGED", "DIZZY",     "PSYCHOTIC"};
}

void ShowSoundEnvEditor(bool& show)
{
    const xr::detail::ImguiWnd wnd{"SoundEnv Editor", &show};
    if (wnd.Collapsed)
        return;

    CSound_environment* env = ::Sound->DbgCurrentEnv();

    static string256 env_name;
    xr_strcpy(env_name, env->name.c_str());

    ImGui::InputText("Current Zone Name", env_name, sizeof(env_name), ImGuiInputTextFlags_ReadOnly);
    ImGui::Separator();

    static bool paused{};

    if (ImGui::Checkbox("Script Sound Env", &paused))
        ::Sound->DbgCurrentEnvPaused(paused);

    ImGui::BeginDisabled(!paused);

    ImGui::SliderFloat("Room", &env->Room, EAXLISTENER_MINROOM, EAXLISTENER_MAXROOM);
    ImGui::SliderFloat("RoomHF", &env->RoomHF, EAXLISTENER_MINROOMHF, EAXLISTENER_MAXROOMHF);
    ImGui::SliderFloat("RoomRolloffFactor", &env->RoomRolloffFactor, EAXLISTENER_MINROOMROLLOFFFACTOR, EAXLISTENER_MAXROOMROLLOFFFACTOR);
    ImGui::SliderFloat("DecayTime", &env->DecayTime, EAXLISTENER_MINDECAYTIME, EAXLISTENER_MAXDECAYTIME);
    ImGui::SliderFloat("DecayHFRatio", &env->DecayHFRatio, EAXLISTENER_MINDECAYHFRATIO, EAXLISTENER_MAXDECAYHFRATIO);
    ImGui::SliderFloat("Reflections", &env->Reflections, EAXLISTENER_MINREFLECTIONS, EAXLISTENER_MAXREFLECTIONS);
    ImGui::SliderFloat("ReflectionsDelay", &env->ReflectionsDelay, EAXLISTENER_MINREFLECTIONSDELAY, EAXLISTENER_MAXREFLECTIONSDELAY);
    ImGui::SliderFloat("Reverb", &env->Reverb, EAXLISTENER_MINREVERB, EAXLISTENER_MAXREVERB);
    ImGui::SliderFloat("ReverbDelay", &env->ReverbDelay, EAXLISTENER_MINREVERBDELAY, EAXLISTENER_MAXREVERBDELAY);
    ImGui::SliderFloat("EnvironmentSize", &env->EnvironmentSize, EAXLISTENER_MINENVIRONMENTSIZE, EAXLISTENER_MAXENVIRONMENTSIZE);
    ImGui::SliderFloat("EnvironmentDiffusion", &env->EnvironmentDiffusion, EAXLISTENER_MINENVIRONMENTDIFFUSION, EAXLISTENER_MAXENVIRONMENTDIFFUSION);
    ImGui::SliderFloat("AirAbsorptionHF", &env->AirAbsorptionHF, EAXLISTENER_MINAIRABSORPTIONHF, EAXLISTENER_MAXAIRABSORPTIONHF);

    static auto cur_env = gsl::narrow<s32>(env->Environment);

    if (ImGui::SliderInt("Current: ", &cur_env, EAXLISTENER_MINENVIRONMENT, EAXLISTENER_MAXENVIRONMENT))
        env->Environment = gsl::narrow<u32>(cur_env);

    ImGui::SameLine();
    ImGui::TextUnformatted(env_names[gsl::narrow<size_t>(cur_env)].data());

    if (ImGui::Button("Save"))
        ::Sound->DbgCurrentEnvSave();

    ImGui::EndDisabled();
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
