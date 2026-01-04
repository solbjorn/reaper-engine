#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_console_vars.h"

#include "embedded_editor_helper.h"

#include "../xr_3da/xr_ioc_cmd.h"

namespace xr
{
namespace detail
{
namespace
{
string256 command_filter{""};
}

void ShowConsoleVars(bool& show)
{
    const xr::detail::ImguiWnd wnd{"Console Vars", &show};
    if (wnd.Collapsed)
        return;

    ImGui::InputText("Filter", command_filter, sizeof(command_filter));

    static f32 drag_intensity{0.0001f};

    ImGui::DragFloat("Speed", &drag_intensity, 0.000001f, 0.000001f, 1.0f, "%.6f");
    ImGui::Separator();

    for (const auto& [Name, Command] : Console->Commands)
    {
        if (xr_strlen(command_filter) > 0 && std::strstr(Name, command_filter) == nullptr)
            continue;

        if (auto mask = smart_cast<CCC_Mask*>(Command); mask == smart_cast<void*>(Command))
        {
            bool test = mask->GetValue();

            if (ImGui::Checkbox(mask->Name(), &test))
                mask->Execute(test ? "1" : "0");

            continue;
        }

        if (auto _float = smart_cast<CCC_Float*>(Command); _float == smart_cast<void*>(Command))
        {
            f32 test = _float->GetValue();
            const auto min = std::clamp(_float->min, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);
            const auto max = std::clamp(_float->max, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);

            if (ImGui::DragFloat(_float->Name(), &test, drag_intensity, min, max))
            {
                string32 String;
                xr_sprintf(String, "%3.5f", test);

                _float->Execute(String);
            }

            continue;
        }

        if (auto _int = smart_cast<CCC_Integer*>(Command); _int == smart_cast<void*>(Command))
        {
            s32 test = _int->GetValue();

            if (ImGui::SliderInt(_int->Name(), &test, _int->min, _int->max))
            {
                string32 String;
                xr_sprintf(String, "%i", test);

                _int->Execute(String);
            }

            continue;
        }

        if (auto token = smart_cast<CCC_Token*>(Command); token == smart_cast<void*>(Command))
        {
            const xr_token* tok = token->GetToken();
            const auto curr = *token->value;
            gsl::czstring Value = "?";

            while (tok->name != nullptr)
            {
                if (tok->id == curr)
                {
                    Value = tok->name;
                    break;
                }

                tok++;
            }

            if (ImGui::BeginCombo(token->Name(), Value))
            {
                tok = token->GetToken();

                while (tok->name != nullptr)
                {
                    if (ImGui::Selectable(tok->name, tok->id == curr))
                        token->Execute(tok->name);

                    tok++;
                }

                ImGui::EndCombo();
            }

            continue;
        }

        if (auto vector = smart_cast<CCC_Vector3*>(Command); vector == smart_cast<void*>(Command))
        {
            auto& Val = *vector->GetValuePtr();
            const auto min = std::clamp(vector->min.x, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);
            const auto max = std::clamp(vector->max.x, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);

            if (ImGui::DragFloat3(vector->Name(), &Val[0], drag_intensity, min, max))
            {
                string64 str;
                xr_sprintf(str, "(%f, %f, %f)", Val.x, Val.y, Val.z);

                vector->Execute(str);
            }

            continue;
        }

        if (auto vector = smart_cast<CCC_Vector4*>(Command); vector == smart_cast<void*>(Command))
        {
            auto& Val = *vector->GetValuePtr();
            const auto min = std::clamp(vector->min.x, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);
            const auto max = std::clamp(vector->max.x, -std::numeric_limits<f32>::max() / 2.0f, std::numeric_limits<f32>::max() / 2.0f);

            if (ImGui::DragFloat4(vector->Name(), &Val[0], drag_intensity, min, max))
            {
                string128 str;
                xr_sprintf(str, "(%f, %f, %f, %f)", Val.x, Val.y, Val.z, Val.w);

                vector->Execute(str);
            }
        }
    }
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
