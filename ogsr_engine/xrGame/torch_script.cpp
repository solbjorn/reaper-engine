#include "stdafx.h"
#include "torch.h"
#include "script_game_object.h"
#include "inventory_item_object.h"
#include "../xr_3da/LightAnimLibrary.h"

IRender_Light* CTorch::GetLight(int target) const
{
    if (target == 0)
        return light_render._get();
    else if (light_omni)
        return light_omni._get();

    return nullptr;
}

void CTorch::SetAnimation(LPCSTR name) { lanim = LALib.FindItem(name); }

void CTorch::SetBrightness(float brightness)
{
    fBrightness = brightness;
    auto c = m_color;
    c.mul_rgb(fBrightness);
    light_render->set_color(c);
}

void CTorch::SetColor(const Fcolor& color, int target)
{
    switch (target)
    {
    case 0:
        m_color = color;
        light_render->set_color(m_color);
        break;
    case 1:
        if (light_omni)
            light_omni->set_color(color);
        break;
    case 2:
        if (glow_render)
            glow_render->set_color(color);
        break;
    }
}

void CTorch::SetRGB(float r, float g, float b, int target)
{
    Fcolor c;
    c.a = 1;
    c.r = r;
    c.g = g;
    c.b = b;
    SetColor(c, target);
}

void CTorch::SetAngle(float angle, int target)
{
    switch (target)
    {
    case 0: light_render->set_cone(angle); break;
    case 1:
        if (light_omni)
            light_omni->set_cone(angle);
        break;
    }
}

void CTorch::SetRange(float range, int target)
{
    switch (target)
    {
    case 0: {
        light_render->set_range(range);
        calc_m_delta_h(range);
        break;
    }
    case 1:
        if (light_omni)
            light_omni->set_range(range);
        break;
    case 2:
        if (glow_render)
            glow_render->set_radius(range);
        break;
    }
}

void CTorch::SetTexture(LPCSTR texture, int target)
{
    switch (target)
    {
    case 0: light_render->set_texture(texture); break;
    case 1:
        if (light_omni)
            light_omni->set_texture(texture);
        break;
    case 2:
        if (glow_render)
            glow_render->set_texture(texture);
        break;
    }
}
void CTorch::SetVirtualSize(float size, int target)
{
    switch (target)
    {
    case 0: light_render->set_virtual_size(size);
    case 1:
        if (light_omni)
            light_omni->set_virtual_size(size);
    }
}

void CTorch::script_register(sol::state_view& lua)
{
    lua.new_usertype<CTorch>("CTorch", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CTorch>), "factory", &client_factory<CTorch>,
                             // alpet: управление параметрами света
                             "on", sol::readonly(&CTorch::m_switched_on), "enable", sol::resolve<void(bool)>(&CTorch::Switch), "switch", sol::resolve<void()>(&CTorch::Switch),
                             "get_light", &CTorch::GetLight, "set_animation", &CTorch::SetAnimation, "set_angle", &CTorch::SetAngle, "set_brightness", &CTorch::SetBrightness,
                             "set_color", &CTorch::SetColor, "set_rgb", &CTorch::SetRGB, "set_range", &CTorch::SetRange, "set_texture", &CTorch::SetTexture, "set_virtual_size",
                             &CTorch::SetVirtualSize,
                             // работа с ПНВ
                             "nvd_on", sol::readonly(&CTorch::m_bNightVisionOn), "enable_nvd", sol::resolve<void(bool)>(&CTorch::SwitchNightVision), "switch_nvd",
                             sol::resolve<void()>(&CTorch::SwitchNightVision), sol::base_classes, xr_sol_bases<CTorch>());

    lua.set_function("get_torch_obj", [](CScriptGameObject* script_obj) { return smart_cast<CTorch*>(&script_obj->object()); });
}
