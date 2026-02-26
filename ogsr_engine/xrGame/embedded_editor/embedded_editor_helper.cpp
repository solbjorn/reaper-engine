#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_helper.h"

#include <imgui_internal.h>

namespace xr
{
namespace detail
{
bool ImGui_HoverDragFloat(gsl::czstring label, f32& v, f32 v_speed, f32 v_min, f32 v_max, gsl::czstring format)
{
    // from ImGui::DragFloat
    const bool rv = ImGui::DragScalar(label, ImGuiDataType_Float, &v, v_speed, &v_min, &v_max, format, ImGuiSliderFlags_NoSpeedTweaks);

    if (!ImGui::IsItemHovered())
        return rv;

    if (!ImGui::GetIO().KeyCtrl)
        return rv;

    const f32 wheel = ImGui::GetIO().MouseWheel;
    if (fis_zero(wheel))
        return rv;

    ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

    if (ImGui::IsItemActive())
    {
        ImGui::ClearActiveID();
        return rv;
    }

    v += v_speed * wheel;

    return true;
}

namespace
{
[[nodiscard]] bool HoverDragFloatImpl(gsl::czstring label, std::span<f32> v, f32 v_speed, f32 v_min, f32 v_max, gsl::czstring format)
{
    // from ImGui::DragScalarN
    const auto& style = ImGui::GetStyle();
    bool value_changed{};

    ImGui::BeginGroup();
    ImGui::PushID(label);
    ImGui::PushMultiItemsWidths(gsl::narrow_cast<s32>(std::ssize(v)), ImGui::CalcItemWidth());

    for (auto [i, elem] : xr::views_enumerate(v))
    {
        ImGui::PushID(gsl::narrow_cast<s32>(i));

        if (i > 0)
            ImGui::SameLine(0, style.ItemInnerSpacing.x);

        value_changed |= ImGui::DragScalar("", ImGuiDataType_Float, &elem, v_speed, &v_min, &v_max, format, ImGuiSliderFlags_NoSpeedTweaks);

        if (ImGui::IsItemHovered() && ImGui::GetIO().KeyCtrl)
        {
            if (const f32 wheel = ImGui::GetIO().MouseWheel; !fis_zero(wheel))
            {
                ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

                if (ImGui::IsItemActive())
                {
                    ImGui::ClearActiveID();
                }
                else
                {
                    elem += v_speed * wheel;
                    if (!fsimilar(v_min, v_max))
                        clamp(elem, v_min, v_max);

                    value_changed = true;
                }
            }
        }

        ImGui::PopID();
        ImGui::PopItemWidth();
    }

    ImGui::PopID();

    if (gsl::czstring label_end = ImGui::FindRenderedTextEnd(label); label != label_end)
    {
        ImGui::SameLine(0, style.ItemInnerSpacing.x);
        ImGui::TextEx(label, label_end);
    }

    ImGui::EndGroup();

    return value_changed;
}
} // namespace

bool ImGui_HoverDragFloat3(gsl::czstring label, std::span<f32, 3> v, f32 v_speed, f32 v_min, f32 v_max, gsl::czstring format)
{
    return HoverDragFloatImpl(label, v, v_speed, v_min, v_max, format);
}

namespace
{
////////////////////////////////////////////////////////////////////////////////////////////////////
// from https://www.strchr.com/natural_sorting
////////////////////////////////////////////////////////////////////////////////////////////////////

[[nodiscard]] gsl::index count_digits(gsl::czstring s)
{
    gsl::czstring p = s;

    while (std::isdigit(*p))
        ++p;

    return p - s;
}

[[nodiscard]] auto compare_naturally(gsl::czstring a, gsl::czstring b)
{
    using ret_t = std::compare_three_way_result_t<std::string_view>;

    for (;;)
    {
        if (std::isdigit(*a) && std::isdigit(*b))
        {
            const auto acnt = count_digits(a);
            auto diff = acnt - count_digits(b);
            if (diff != 0)
                return diff < 0 ? ret_t::less : ret_t::greater;

            diff = std::memcmp(a, b, gsl::narrow_cast<size_t>(acnt));
            if (diff != 0)
                return diff < 0 ? ret_t::less : ret_t::greater;

            a += acnt;
            b += acnt;
        }

        if (*a != *b)
            return *a - *b < 0 ? ret_t::less : ret_t::greater;

        if (*a == '\0')
            return ret_t::equal;

        ++a;
        ++b;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

[[nodiscard]] bool list_box(gsl::czstring label, gsl::index& current_item, gsl::czstring (*items_getter)(void*, gsl::index), void* data, gsl::index items_count,
                            const ImVec2& size_arg)
{
    if (!ImGui::BeginListBox(label, size_arg))
        return false;

    ImGuiListClipper clipper;
    clipper.ItemsCount = -1;
    clipper.Begin(gsl::narrow_cast<s32>(items_count), ImGui::GetTextLineHeightWithSpacing());

    const bool scrollTo = ImGui::IsWindowAppearing();
    if (scrollTo)
        clipper.IncludeItemsByIndex(gsl::narrow_cast<s32>(current_item - 5), gsl::narrow_cast<s32>(current_item + 5));

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{0.30f, 0.34f, 1.0f, 1.0f});
    bool value_changed{};

    while (clipper.Step())
    {
        for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            const bool item_selected = i == current_item;
            gsl::czstring item_text = items_getter(data, i) ?: "*Unknown item*";

            ImGui::PushID(i);

            if (ImGui::Selectable(item_text, item_selected))
            {
                current_item = i;
                value_changed = true;
            }

            if (scrollTo && item_selected)
                ImGui::SetScrollHereY(1.0f);

            ImGui::PopID();
        }
    }

    ImGui::PopStyleColor();
    ImGui::EndListBox();

    return value_changed;
}
} // namespace

bool SelectFile(gsl::czstring label, gsl::czstring initial, shared_str& file_name)
{
    string256 currValue;
    xr_strcpy(currValue, file_name.data());

    bool changed{};

    ImGui::PushID(label);

    if (ImGui::InputText("", currValue, sizeof(currValue)))
    {
        file_name._set(currValue);
        changed = true;
    }

    static shared_str prevValue;
    static xr_string curr_dir;

    ImGui::SameLine();

    if (ImGui::Button("..."))
    {
        ImGui::OpenPopup("Choose file...");
        prevValue = file_name;

        string_path _dir;
        _splitpath_s(file_name.c_str(), nullptr, 0, _dir, sizeof(_dir), nullptr, 0, nullptr, 0);
        curr_dir = _dir;

        if (curr_dir[curr_dir.length() - 1] != '\\')
            curr_dir += '\\';
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::SetNextWindowSize(ImVec2{250.0f, 400.0f}, ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Choose file...", nullptr, 0))
    {
        if (ImGui::Button(".."))
        {
            // go up one directory
            if (const auto end = curr_dir.find_last_not_of("/\\"); end != xr_string::npos)
            {
                if (const auto last_slash = curr_dir.find_last_of("/\\", end); last_slash != xr_string::npos)
                    curr_dir = curr_dir.substr(0, last_slash + 1);
                else
                    curr_dir = "\\";
            }
        }

        ImGui::SameLine();
        ImGui::Text("path: %s", curr_dir.c_str());

        xr_string engine_dir = curr_dir;
        if (engine_dir[0] == '\\')
            engine_dir.erase(0, 1);

        static xr_map<xr_string, xr_vector<xr_string>> fs_folders;

        auto& current_folders = fs_folders[curr_dir];
        if (current_folders.empty())
        {
            xr_vector<gsl::zstring>* items = FS.file_list_open(initial, engine_dir.c_str(), FS_ListFolders | FS_RootOnly);
            if (items != nullptr)
            {
                current_folders.resize(items->size());
                const auto e = std::copy(items->begin(), items->end(), current_folders.begin());

                current_folders.resize(gsl::narrow_cast<size_t>(e - current_folders.begin()));
                std::ranges::sort(current_folders, [](const auto& a, const auto& b) { return std::is_lt(compare_naturally(a.c_str(), b.c_str())); });
            }

            FS.file_list_close(items);
        }

        static xr_map<xr_string, xr_vector<xr_string>> fs_files;

        auto& current_files = fs_files[curr_dir];
        if (current_files.empty())
        {
            xr_vector<gsl::zstring>* items = FS.file_list_open(initial, engine_dir.c_str(), FS_ListFiles | FS_RootOnly);
            if (items != nullptr)
            {
                current_files.resize(items->size());
                const auto e = std::copy_if(items->begin(), items->end(), current_files.begin(),
                                            [](auto x) { return std::strstr(x, "#small") == nullptr && std::strstr(x, ".thm") == nullptr; });

                current_files.resize(gsl::narrow_cast<size_t>(e - current_files.begin()));
                std::ranges::sort(current_files, [](const auto& a, const auto& b) { return std::is_lt(compare_naturally(a.c_str(), b.c_str())); });
            }

            FS.file_list_close(items);
        }

        if (ImGui::BeginTable("tbl", 2))
        {
            ImGui::TableNextColumn();

            static gsl::index curr_folder_index{-1};

            ImGui::TextUnformatted("folders:");

            if (list_box(
                    "##folders", curr_folder_index,
                    [](void* data, gsl::index idx) -> gsl::czstring {
                        const auto& list = *static_cast<xr_vector<xr_string>*>(data);
                        return list[gsl::narrow_cast<size_t>(idx)].c_str();
                    },
                    &current_folders, std::ssize(current_folders), ImVec2{-4.0f, -ImGui::GetFrameHeightWithSpacing()}))
            {
                string256 _dir;
                xr_strconcat(_dir, curr_dir.c_str(), current_folders[gsl::narrow_cast<size_t>(curr_folder_index)].c_str());

                curr_dir = _dir;
                if (curr_dir[curr_dir.length() - 1] != '\\')
                    curr_dir += '\\';

                curr_folder_index = -1;
            }

            ImGui::TableNextColumn();

            string_path curr_name;
            _splitpath_s(currValue, nullptr, 0, nullptr, 0, curr_name, sizeof(curr_name), nullptr, 0);
            if (strext(curr_name) == nullptr)
                xr_strconcat(curr_name, curr_name, ".dds"); // default extension

            gsl::index curr_name_index{-1};

            for (auto [idx, file] : xr::views_enumerate(std::as_const(current_files)))
            {
                if (file == curr_name)
                {
                    curr_name_index = idx;
                    break;
                }
            }

            ImGui::TextUnformatted("files:");

            if (list_box(
                    "##files", curr_name_index,
                    [](void* data, gsl::index idx) -> gsl::czstring {
                        const auto& list = *static_cast<xr_vector<xr_string>*>(data);
                        return list[gsl::narrow_cast<size_t>(idx)].c_str();
                    },
                    &current_files, std::ssize(current_files), ImVec2{-4.0f, -ImGui::GetFrameHeightWithSpacing()}))
            {
                xr_strconcat(currValue, engine_dir.c_str(), current_files[gsl::narrow_cast<size_t>(curr_name_index)].c_str());
                fix_texture_name(currValue);

                file_name._set(currValue);
                changed = true;
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("OK", ImVec2{120.0f, 0.0f}))
            ImGui::CloseCurrentPopup();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2{120.0f, 0.0f}))
        {
            ImGui::CloseCurrentPopup();

            file_name = prevValue;
            changed = true;
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();

    return changed;
}
} // namespace detail
} // namespace xr
#endif // IMGUI_DISABLE
