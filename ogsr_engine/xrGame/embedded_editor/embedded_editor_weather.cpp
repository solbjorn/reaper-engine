#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_weather.h"

#include "embedded_editor_helper.h"

#include "../xr_3da/Environment.h"
#include "../xr_3da/thunderbolt.h"
#include "../xr_3da/xr_efflensflare.h"

#include "GamePersistent.h"
#include "Level.h"
#include "game_cl_base.h"

namespace xr
{
namespace detail
{
bool g_ScriptWeather{};
bool s_ScriptNoMixer{};
bool s_ScriptTime{};

namespace
{
f32 editor_longitude{};
f32 editor_altitude{};

[[nodiscard]] Fvector convert(Fvector v) { return Fvector{v.z, v.y, v.x}; }

[[nodiscard]] gsl::czstring enumCycle(void* user_data, s32 idx)
{
    const auto& cycles = *static_cast<xr_vector<shared_str>*>(user_data);
    return cycles[gsl::narrow<size_t>(idx)].c_str();
}

[[nodiscard]] gsl::czstring enumWeather(void* user_data, s32 idx)
{
    const auto& envs = *static_cast<xr_vector<CEnvDescriptor*>*>(user_data);
    return envs[gsl::narrow<size_t>(idx)]->m_identifier.c_str();
}

constexpr gsl::czstring empty{""};

[[nodiscard]] gsl::czstring enumIniWithEmpty(void* user_data, s32 idx)
{
    if (idx <= 0)
        return empty;

    const CInifile* ini = static_cast<CInifile*>(user_data);
    return ini->sections_ordered()[gsl::narrow<size_t>(idx - 1)].second->Name.c_str();
}

[[nodiscard]] gsl::czstring enumIni(void* user_data, s32 idx)
{
    const CInifile* ini = static_cast<CInifile*>(user_data);
    return ini->sections_ordered()[gsl::narrow<size_t>(idx)].second->Name.c_str();
}

bool s_ScriptWeatheParams{};

xr_set<shared_str> modifiedWeathers;

void saveWeather(gsl::czstring name, const xr_vector<CEnvDescriptor*>& env)
{
    CInifile f{nullptr, false, false, false};

    for (const auto el : env)
    {
        gsl::czstring sect = el->m_identifier.c_str();

        if (el->env_ambient != nullptr)
            f.w_string(sect, "ambient", el->env_ambient->name().c_str());

        f.w_fvector3(sect, "ambient_color", el->ambient);
        f.w_fvector4(sect, "clouds_color", el->clouds_color);
        f.w_string(sect, "clouds_texture", el->clouds_texture_name.c_str());
        f.w_float(sect, "far_plane", el->far_plane);
        f.w_float(sect, "fog_distance", el->fog_distance);
        f.w_float(sect, "fog_density", el->fog_density);
        f.w_fvector3(sect, "fog_color", el->fog_color);
        f.w_fvector4(sect, "hemisphere_color", el->hemi_color);
        f.w_fvector3(sect, "rain_color", el->rain_color);
        f.w_float(sect, "rain_density", el->rain_density);
        f.w_fvector3(sect, "sky_color", el->sky_color);
        f.w_float(sect, "sky_rotation", rad2deg(el->sky_rotation));
        f.w_string(sect, "sky_texture", el->sky_texture_name.c_str());
        f.w_string(sect, "sun", el->lens_flare_id.c_str());
        f.w_fvector3(sect, "sun_color", el->sun_color);
        f.w_float(sect, "sun_shafts_intensity", el->m_fSunShaftsIntensity);
        f.w_string(sect, "thunderbolt_collection", el->tb_id.c_str());
        f.w_float(sect, "thunderbolt_duration", el->bolt_duration);
        f.w_float(sect, "thunderbolt_period", el->bolt_period);
        f.w_float(sect, "tree_amplitude_intensity", el->m_fTreeAmplitudeIntensity);
        f.w_float(sect, "water_intensity", el->m_fWaterIntensity);
        f.w_float(sect, "wind_direction", rad2deg(el->wind_direction));
        f.w_float(sect, "wind_velocity", el->wind_velocity);
        f.w_float(sect, "sun_altitude", rad2deg(el->sun_dir.getH()));
        f.w_float(sect, "sun_longitude", rad2deg(el->sun_dir.getP()));
    }

    string_path fileName;
    std::ignore = FS.update_path(fileName, "$game_weathers$", name);

    xr_strconcat(fileName, fileName, ".ltx");
    std::ignore = f.save_as(fileName);
}
} // namespace

void ShowWeatherEditor(bool& show)
{
    if (!show)
        return;

    if (g_pGameLevel == nullptr)
    {
        show = false;
        return;
    }

    const xr::detail::ImguiWnd wnd{"Weather Editor", &show};
    if (wnd.Collapsed)
        return;

    auto& env = GamePersistent().Environment();
    CEnvDescriptor* cur = env.Current[0];

    const auto time = Level().GetEnvironmentGameTime() / 1000;
    ImGui::Text("Time: %02llu:%02llu:%02llu", (time / (60 * 60)) % 24, (time / 60) % 60, time % 60);

    f32 tf = Level().GetGameTimeFactor();

    if (ImGui::SliderFloat("Time factor", &tf, 0.0f, 10000.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        Level().Server->game->SetGameTimeFactor(tf);
        env.SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
    }

    xr_vector<shared_str> cycles;
    s32 iCycle{-1};

    for (const auto& el : env.WeatherCycles)
    {
        cycles.push_back(el.first);

        if (el.first == env.CurrentWeatherName)
            iCycle = gsl::narrow_cast<s32>(std::ssize(cycles) - 1);
    }

    ImGui::Text("Main parameters");

    if (!modifiedWeathers.empty())
    {
        ImGui::SameLine();
        ImGui::Text("* modified");
    }

    ImGui::Checkbox("Script weather", &g_ScriptWeather);
    ImGui::BeginDisabled(!g_ScriptWeather);

    if (ImGui::Combo("Weather cycle", &iCycle, enumCycle, &cycles, gsl::narrow_cast<s32>(std::ssize(env.WeatherCycles))))
        env.SetWeather(cycles[gsl::narrow<size_t>(iCycle)], true);

    ImGui::EndDisabled();
    ImGui::Checkbox("Script time", &s_ScriptTime);

    s32 sel{-1};

    for (auto [i, weather] : xr::views_enumerate(std::as_const(*env.CurrentWeather)))
    {
        if (weather->m_identifier == cur->m_identifier)
        {
            sel = gsl::narrow_cast<s32>(i);
            break;
        }
    }

    ImGui::BeginDisabled(!s_ScriptTime);

    if (ImGui::Combo("Current section", &sel, enumWeather, env.CurrentWeather, gsl::narrow_cast<s32>(std::ssize(*env.CurrentWeather))))
    {
        env.SetGameTime((*env.CurrentWeather)[gsl::narrow<size_t>(sel)]->exec_time + 0.5f, Level().game->GetEnvironmentGameTimeFactor());
        env.SetWeather(cycles[gsl::narrow<size_t>(iCycle)], true);
    }

    ImGui::EndDisabled();
    ImGui::Separator();
    ImGui::Checkbox("Disable weather mixer", &s_ScriptNoMixer);

    sel = -1;

    ImGui::Text("Ambient light parameters");

    for (auto [i, ambient] : xr::views_enumerate(std::as_const(env.m_ambients_config->sections_ordered())))
    {
        if (ambient.second->Name == cur->env_ambient->name())
        {
            sel = gsl::narrow_cast<s32>(i);
            break;
        }
    }

    bool changed{};

    if (ImGui::Combo("ambient", &sel, enumIni, env.m_ambients_config, gsl::narrow_cast<s32>(std::ssize(env.m_ambients_config->sections_ordered()))))
    {
        cur->env_ambient = env.AppendEnvAmb(env.m_ambients_config->sections_ordered()[gsl::narrow<size_t>(sel)].second->Name);
        changed = true;
    }

    if (ImGui::ColorEdit3("ambient_color", &cur->ambient[0]))
        changed = true;

    ImGui::Text("Clouds parameters");

    if (ImGui::ColorEdit4("clouds_color", &cur->clouds_color[0], ImGuiColorEditFlags_AlphaBar))
        changed = true;

    if (xr::detail::SelectFile("clouds_texture", "$game_textures$", cur->clouds_texture_name))
    {
        cur->get();
        changed = true;
    }

    ImGui::Text("Fog parameters");

    if (ImGui::SliderFloat("far_plane", &cur->far_plane, 0.01f, 10000.0f))
        changed = true;

    if (ImGui::SliderFloat("fog_distance", &cur->fog_distance, 0.0f, 10000.0f))
        changed = true;

    if (ImGui::SliderFloat("fog_density", &cur->fog_density, 0.0f, 10.0f))
        changed = true;

    if (ImGui::ColorEdit3("fog_color", &cur->fog_color[0]))
        changed = true;

    ImGui::Text("Hemi parameters");

    if (ImGui::ColorEdit4("hemisphere_color", &cur->hemi_color[0], ImGuiColorEditFlags_AlphaBar))
        changed = true;

    ImGui::Text("Rain parameters");

    if (ImGui::SliderFloat("rain_density", &cur->rain_density, 0.0f, 1.0f))
        changed = true;

    if (ImGui::ColorEdit3("rain_color", &cur->rain_color[0]))
        changed = true;

    ImGui::Text("Sky parameters");

    Fvector temp = convert(cur->sky_color);

    if (ImGui::ColorEdit3("sky_color", &temp[0]))
        changed = true;

    cur->sky_color = convert(temp);

    if (ImGui::SliderFloat("sky_rotation", &cur->sky_rotation, 0.0f, 6.28318f))
        changed = true;

    if (xr::detail::SelectFile("sky_texture", "$game_textures$", cur->sky_texture_name))
    {
        string_path buf;
        xr_strconcat(buf, cur->sky_texture_name.c_str(), "#small");
        cur->sky_texture_env_name._set(buf);

        cur->get();
        changed = true;
    }

    ImGui::Text("Sun parameters");

    sel = -1;

    for (auto [i, flare] : xr::views_enumerate(std::as_const(env.m_suns_config->sections_ordered())))
    {
        if (flare.second->Name == cur->lens_flare_id)
        {
            sel = gsl::narrow_cast<s32>(i);
            break;
        }
    }

    if (ImGui::Combo("sun", &sel, enumIni, env.m_suns_config, gsl::narrow_cast<s32>(std::ssize(env.m_suns_config->sections_ordered()))))
    {
        cur->lens_flare_id = env.eff_LensFlare->AppendDef(env, env.m_suns_config->sections_ordered()[gsl::narrow<size_t>(sel)].second->Name.c_str());
        env.eff_LensFlare->Invalidate();

        changed = true;
    }

    if (ImGui::ColorEdit3("sun_color", &cur->sun_color[0]))
        changed = true;

    if (ImGui::SliderFloat("sun_altitude", &editor_altitude, -360.0f, 360.0f))
    {
        cur->sun_dir.setHP(deg2rad(editor_longitude), deg2rad(editor_altitude));
        changed = true;
    }

    if (ImGui::SliderFloat("sun_longitude", &editor_longitude, -360.0f, 360.0f))
    {
        cur->sun_dir.setHP(deg2rad(editor_longitude), deg2rad(editor_altitude));
        changed = true;
    }

    if (ImGui::SliderFloat("sun_shafts_intensity", &cur->m_fSunShaftsIntensity, 0.0f, 2.0f))
        changed = true;

    sel = 0;

    for (auto [i, bolt] : xr::views_enumerate(std::as_const(env.m_thunderbolt_collections_config->sections_ordered())))
    {
        if (bolt.second->Name == cur->tb_id)
        {
            sel = gsl::narrow_cast<s32>(i + 1);
            break;
        }
    }

    ImGui::Text("Thunder bolt parameters");

    if (ImGui::Combo("thunderbolt_collection", &sel, enumIniWithEmpty, env.m_thunderbolt_collections_config,
                     gsl::narrow_cast<s32>(std::ssize(env.m_thunderbolt_collections_config->sections_ordered()) + 1)))
    {
        gsl::czstring sect = sel == 0 ? empty : env.m_thunderbolt_collections_config->sections_ordered()[gsl::narrow<size_t>(sel - 1)].second->Name.c_str();
        cur->tb_id = env.eff_Thunderbolt->AppendDef(env, env.m_thunderbolt_collections_config, env.m_thunderbolts_config, sect);

        changed = true;
    }

    if (ImGui::SliderFloat("thunderbolt_duration", &cur->bolt_duration, 0.0f, 2.0f))
        changed = true;

    if (ImGui::SliderFloat("thunderbolt_period", &cur->bolt_period, 0.0f, 10.0f))
        changed = true;

    ImGui::Text("Water parameters");

    if (ImGui::SliderFloat("water_intensity", &cur->m_fWaterIntensity, 0.0f, 2.0f))
        changed = true;

    ImGui::Text("Wind parameters");

    if (ImGui::SliderFloat("wind_velocity", &cur->wind_velocity, 0.0f, 1000.0f))
        changed = true;

    if (ImGui::SliderFloat("wind_direction", &cur->wind_direction, 0.0f, 360.0f))
        changed = true;

    ImGui::Text("Trees parameters");

    if (ImGui::SliderFloat("trees_amplitude_intensity", &cur->m_fTreeAmplitudeIntensity, 0.01f, 0.25f))
        changed = true;

    if (changed)
    {
        modifiedWeathers.insert(env.CurrentWeatherName);
        s_ScriptWeatheParams = true;
    }

    if (s_ScriptWeatheParams && ImGui::Button("Save"))
    {
        for (const auto& name : modifiedWeathers)
            saveWeather(name.c_str(), env.WeatherCycles[name]);

        modifiedWeathers.clear();
        s_ScriptWeatheParams = false;
    }

    ImGui::SameLine();

    if (s_ScriptWeatheParams && ImGui::Button("Reset"))
    {
        s_ScriptWeatheParams = false;
        env.SetWeather(env.CurrentWeatherName, true);

        modifiedWeathers.clear();
    }
}
} // namespace detail
} // namespace xr
#endif // IMGUI_DISABLE
