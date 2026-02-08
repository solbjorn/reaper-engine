#include "stdafx.h"

#ifndef IMGUI_DISABLE
#include "embedded_editor_console.h"

#include "../xrExternal/imgui.h"
#include "../xr_3da/xr_ioc_cmd.h"

#include <imgui_internal.h>

namespace xr
{
namespace detail
{
class ImGuiGameConsole
{
public:
    ImGuiGameConsole();
    ~ImGuiGameConsole();

private:
    enum class Console_mark : s32
    {
        no_mark = ' ',
        mark0 = '~',
        mark1 = '!',
        // error
        mark2 = '@',
        // console cmd
        mark3 = '#',
        mark4 = '$',
        mark5 = '%',
        mark6 = '^',
        mark7 = '&',
        mark8 = '*',
        mark9 = '-',
        // green = ok
        mark10 = '+',
        mark11 = '=',
        mark12 = '/',
    };

    static constexpr size_t CONSOLE_BUF_SIZE{1024};
    static constexpr gsl::index VIEW_TIPS_COUNT{14};
    static constexpr size_t MAX_TIPS_COUNT{220};

    std::array<char, CONSOLE_BUF_SIZE> m_edit_string{};
    shared_str m_cur_cmd;

    xr_vector<shared_str> m_temp_tips;
    xr_vector<TipString> m_tips;

    xr_vector<shared_str> m_cmd_history;
    size_t m_cmd_history_max;
    gsl::index m_cmd_history_idx;
    shared_str m_last_cmd;

    gsl::index m_start_tip;
    gsl::index m_select_tip;
    gsl::index m_prev_length_str{};
    shared_str m_prev_str;

    u32 m_tips_mode{};
    bool m_disable_tips{};

    bool bVisible{};
    bool m_refresh_keyboard_focus{};

    [[nodiscard]] static const ImVec4& get_mark_color(Console_mark type);

public:
    void Show();
    void Hide();

private:
    void Render();

    [[nodiscard]] s32 InputCallback(ImGuiInputTextCallbackData* data);

    void Prev_cmd();
    void Next_cmd();

    void Begin_tips();
    void End_tips();

    void PageUp_tips();
    void PageDown_tips();

    void SelectCommand();
    void ExecuteCommand(gsl::czstring cmd);
    void AddCmdHistory(gsl::czstring str);

    void next_cmd_history_idx();
    void prev_cmd_history_idx();
    void reset_cmd_history_idx();
    void add_cmd_history(const shared_str& str);

    void next_selected_tip();
    void check_next_selected_tip();
    void prev_selected_tip();
    void check_prev_selected_tip();
    void reset_selected_tip();

    void update_tips();
    void reset_tips();
    void select_for_filter(gsl::czstring filter_str, const xr_vector<shared_str>& in_v, xr_vector<TipString>& out_v);

    [[nodiscard]] IConsole_Command* find_next_cmd(gsl::czstring in_str, shared_str& out_str);
    [[nodiscard]] bool add_internal_cmds(gsl::czstring in_str, xr_vector<TipString>& out_v);
};

namespace
{
class embedded_editor_font
{
private:
    ImFont* large;
    bool initialized{};

    void init();

public:
    embedded_editor_font() = default;

    [[nodiscard]] ImFont* GetLarge()
    {
        init();
        return large;
    }
};

void embedded_editor_font::init()
{
    if (initialized)
        return;

    initialized = true;

    // shitty hack
    large = ImGui::GetIO().Fonts->Fonts[1];
}

void ltrim(xr_string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

void rtrim(xr_string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

void trim(xr_string& s)
{
    rtrim(s);
    ltrim(s);
}

void strlwr(xr_string& data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](char c) { return xr::tolower(c); });
}

void split_cmd(const xr_string& str, xr_string& out1, xr_string& out2)
{
    size_t end{};
    gsl::index it{};

    while (true)
    {
        const auto start = str.find_first_not_of(' ', end);
        if (start == xr_string::npos)
            break;

        if (it == 0)
        {
            end = str.find(' ', start);
            out1 = str.substr(start, end - start);
        }
        else
        {
            end = str.length();
            out2 = str.substr(start, end - start);

            return;
        }

        ++it;
    }
}

constexpr gsl::czstring ioc_prompt{">>>"};
constexpr size_t cmd_history_max{64};

constexpr ImVec4 prompt_font_color{0.8941f, 0.8941f, 1.0f, 1.0f};
constexpr ImVec4 tips_font_color{0.9020f, 0.9804f, 0.9020f, 1.0f};
constexpr ImVec4 cmd_font_color{0.5412f, 0.5412f, 0.9608f, 1.0f};
constexpr ImVec4 total_font_color{0.9804f, 0.9804f, 0.0588f, 0.7059f};

constexpr ImVec4 back_color{0.0784f, 0.0784f, 0.0784f, 0.7843f};
constexpr ImVec4 tips_back_color{0.0784f, 0.0784f, 0.0784f, 0.7843f};
constexpr ImVec4 tips_select_color{0.3529f, 0.3529f, 0.5490f, 0.9020f};

constexpr ImU32 tips_word_color{color_rgba(5, 100, 56, 200)};

embedded_editor_font ImGuiFonts;
ImGuiGameConsole g_console;
} // namespace

void ShowConsole(bool& show)
{
    if (show)
        g_console.Show();
    else
        g_console.Hide();
}

ImGuiGameConsole::ImGuiGameConsole()
{
    m_cmd_history_max = cmd_history_max;

    m_cmd_history.reserve(m_cmd_history_max + 2);
    m_tips.reserve(MAX_TIPS_COUNT + 1);
    m_temp_tips.reserve(MAX_TIPS_COUNT + 1);

    reset_selected_tip();
}

ImGuiGameConsole::~ImGuiGameConsole() = default;

const ImVec4& ImGuiGameConsole::get_mark_color(Console_mark type)
{
    static constexpr ImVec4 mark0_color{1.0f, 1.0f, 0.0f, 1.0f};
    static constexpr ImVec4 mark1_color{1.0f, 0.0f, 0.0f, 1.0f};
    static constexpr ImVec4 mark2_color{0.3922f, 0.3922f, 1.0f, 1.0f};
    static constexpr ImVec4 mark3_color{0.0f, 0.8706f, 0.8039f, 0.6078f};
    static constexpr ImVec4 mark4_color{1.0f, 0.0f, 1.0f, 1.0f};
    static constexpr ImVec4 mark5_color{0.6078f, 0.2157f, 0.6667f, 0.6078f};
    static constexpr ImVec4 mark6_color{0.0980f, 0.7843f, 0.1961f, 1.0f};
    static constexpr ImVec4 mark7_color{1.0f, 1.0f, 0.0f, 1.0f};
    static constexpr ImVec4 mark8_color{0.5020f, 0.5020f, 0.5020f, 1.0f};
    static constexpr ImVec4 mark9_color{0.0f, 1.0f, 0.0f, 1.0f};
    static constexpr ImVec4 mark10_color{0.2157f, 0.6078f, 0.5490f, 1.0f};
    static constexpr ImVec4 mark11_color{0.8039f, 0.8039f, 0.4118f, 1.0f};
    static constexpr ImVec4 mark12_color{0.5020f, 0.5020f, 0.9804f, 1.0f};
    static constexpr ImVec4 default_font_color{0.9804f, 0.9804f, 0.9804f, 0.9804f};

    switch (type)
    {
    case Console_mark::mark0: return mark0_color;
    case Console_mark::mark1: return mark1_color;
    case Console_mark::mark2: return mark2_color;
    case Console_mark::mark3: return mark3_color;
    case Console_mark::mark4: return mark4_color;
    case Console_mark::mark5: return mark5_color;
    case Console_mark::mark6: return mark6_color;
    case Console_mark::mark7: return mark7_color;
    case Console_mark::mark8: return mark8_color;
    case Console_mark::mark9: return mark9_color;
    case Console_mark::mark10: return mark10_color;
    case Console_mark::mark11: return mark11_color;
    case Console_mark::mark12: return mark12_color;
    default: return default_font_color;
    }
}

void ImGuiGameConsole::add_cmd_history(const shared_str& str)
{
    if (str.empty())
        return;

    m_cmd_history.emplace_back(str);

    if (m_cmd_history.size() > m_cmd_history_max)
        m_cmd_history.erase(m_cmd_history.begin());
}

void ImGuiGameConsole::next_cmd_history_idx()
{
    --m_cmd_history_idx;
    m_cmd_history_idx = std::max(m_cmd_history_idx, 0z);
}

void ImGuiGameConsole::prev_cmd_history_idx()
{
    ++m_cmd_history_idx;

    if (m_cmd_history_idx >= std::ssize(m_cmd_history))
        m_cmd_history_idx = std::ssize(m_cmd_history) - 1;
}

void ImGuiGameConsole::reset_cmd_history_idx() { m_cmd_history_idx = -1; }

void ImGuiGameConsole::next_selected_tip()
{
    ++m_select_tip;
    check_next_selected_tip();
}

void ImGuiGameConsole::check_next_selected_tip()
{
    if (m_select_tip >= std::ssize(m_tips))
        m_select_tip = std::ssize(m_tips) - 1;

    const auto sel_dif = std::max(m_select_tip - VIEW_TIPS_COUNT + 1, 0z);
    m_start_tip = std::max(sel_dif, m_start_tip);
}

void ImGuiGameConsole::prev_selected_tip()
{
    --m_select_tip;
    check_prev_selected_tip();
}

void ImGuiGameConsole::check_prev_selected_tip()
{
    m_select_tip = std::max(m_select_tip, 0z);
    m_start_tip = std::min(m_start_tip, m_select_tip);
}

void ImGuiGameConsole::reset_selected_tip()
{
    m_select_tip = -1;
    m_start_tip = 0;
}

void ImGuiGameConsole::reset_tips()
{
    m_temp_tips.clear();
    m_tips.clear();

    m_cur_cmd._set(nullptr);
}

void ImGuiGameConsole::select_for_filter(gsl::czstring filter_str, const xr_vector<shared_str>& in_v, xr_vector<TipString>& out_v)
{
    out_v.clear();

    if (filter_str == nullptr || in_v.empty())
        return;

    const bool all = xr_strlen(filter_str) == 0;

    for (auto& str : in_v)
    {
        if (all)
        {
            out_v.emplace_back(str);
            continue;
        }

        if (gsl::czstring fd_str = std::strstr(str.c_str(), filter_str); fd_str != nullptr)
        {
            const auto fd_sz = str.size() - xr_strlen(fd_str);
            out_v.emplace_back(str, fd_sz, fd_sz + xr_strlen(filter_str));
        }
    }
}

void ImGuiGameConsole::update_tips()
{
    if (!bVisible)
    {
        reset_tips();
        return;
    }

    gsl::czstring cur = m_edit_string.data();
    const auto cur_length = xr_strlen(cur);

    if (cur_length == 0)
    {
        reset_tips();
        m_prev_length_str = 0;

        return;
    }

    if (cur_length == m_prev_str.size() && std::is_eq(xr_strcmp(m_prev_str, cur)))
        return;

    m_prev_str._set(cur);
    reset_tips();

    if (m_prev_length_str != cur_length)
        reset_selected_tip();

    m_prev_length_str = cur_length;

    xr_string s_cur{cur}, first, last;
    split_cmd(s_cur, first, last);

    const auto first_lenght = first.length();

    if (first_lenght > 2 && first_lenght + 1 <= gsl::narrow_cast<size_t>(cur_length)) // param
    {
        if (cur[first_lenght] == ' ')
        {
            if (m_tips_mode != 2)
                reset_selected_tip();

            if (const auto it = Console->Commands.find(first.c_str()); it != Console->Commands.end())
            {
                if (IConsole_Command* cc = it->second; cc->bEnabled)
                {
                    if (first_lenght + 2 <= gsl::narrow_cast<size_t>(cur_length) && cur[first_lenght] == ' ' && cur[first_lenght + 1] == ' ')
                        last.erase(0, 1); // fake: next char

                    cc->fill_tips(m_temp_tips);

                    m_tips_mode = 2;
                    m_cur_cmd._set(first.c_str());
                    select_for_filter(last.c_str(), m_temp_tips, m_tips);

                    if (m_tips.empty())
                        m_tips.emplace_back(shared_str{"(empty)"});

                    if (std::ssize(m_tips) <= m_select_tip)
                        reset_selected_tip();
                }

                return;
            }
        }
    }

    // cmd name
    std::ignore = add_internal_cmds(cur, m_tips);
    m_tips_mode = 1;

    if (m_tips.empty())
    {
        reset_selected_tip();
        m_tips_mode = 0;
    }

    if (std::ssize(m_tips) <= m_select_tip)
        reset_selected_tip();
}

IConsole_Command* ImGuiGameConsole::find_next_cmd(gsl::czstring in_str, shared_str& out_str)
{
    string_path t2;
    xr_strconcat(t2, in_str, " ");

    if (const auto it = Console->Commands.lower_bound(t2); it != Console->Commands.end())
    {
        IConsole_Command* cc = it->second;
        out_str._set(cc->Name());

        return cc;
    }

    return nullptr;
}

bool ImGuiGameConsole::add_internal_cmds(gsl::czstring in_str, xr_vector<TipString>& out_v)
{
    if (out_v.size() >= MAX_TIPS_COUNT)
        return false;

    const auto in_sz = xr_strlen(in_str);
    xr_string name2;
    bool res{};

    // word in begin
    for (const auto& cmd : Console->Commands)
    {
        if (gsl::czstring name = cmd.first; xr_strlen(name) >= in_sz)
        {
            name2.assign(name, gsl::narrow_cast<size_t>(in_sz));

            if (std::is_eq(xr::strcasecmp(name2, in_str)))
            {
                shared_str temp{name};

                if (const bool dup = std::find(out_v.begin(), out_v.end(), temp) != out_v.end(); !dup && cmd.second->bEnabled)
                {
                    out_v.emplace_back(std::move(temp), 0, in_sz);
                    res = true;
                }
            }
        }

        if (out_v.size() >= MAX_TIPS_COUNT)
            return res;
    }

    // word in internal
    for (const auto& cmd : Console->Commands)
    {
        if (gsl::czstring name = cmd.first, fd_str = std::strstr(name, in_str); fd_str != nullptr)
        {
            shared_str temp{name};

            if (const bool dup = (std::find(out_v.begin(), out_v.end(), temp) != out_v.end()); !dup && cmd.second->bEnabled)
            {
                const auto name_sz = xr_strlen(name);
                const auto fd_sz = name_sz - xr_strlen(fd_str);

                out_v.emplace_back(temp, fd_sz, fd_sz + in_sz);
                res = true;
            }
        }

        if (out_v.size() >= MAX_TIPS_COUNT)
            return res;
    }

    return res;
}

void ImGuiGameConsole::Render()
{
    if (Device.dwFrame % 5 == 0)
        update_tips();

    ImVec2 pos{};

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2.0f, 2.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{4.0f, 2.0f});

    ImGui::PushFont(ImGuiFonts.GetLarge(), 20.0f);

    // Console window
    ImGui::SetNextWindowSize({640.0f, 480.0f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({0.0f, 0.0f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints({640.0f, 480.0f}, {gsl::narrow_cast<f32>(Device.dwWidth), gsl::narrow_cast<f32>(Device.dwHeight)});

    ImGui::PushStyleColor(ImGuiCol_WindowBg, back_color);

    constexpr ImGuiWindowFlags console_window_flags{ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing};

    if (ImGui::Begin("Console", nullptr, console_window_flags))
    {
        const auto amount = std::ssize(LogFile);
        const f32 footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        static f32 prev_scroll_max = ImGui::GetScrollMaxY();

        constexpr ImGuiWindowFlags log_flags{ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground};

        // Log subwindow
        if (ImGui::BeginChild("Log", {0.0f, -footer_height_to_reserve}, 0, log_flags))
        {
            ImGuiListClipper clipper;
            clipper.Begin(gsl::narrow_cast<s32>(amount), ImGui::GetTextLineHeightWithSpacing());

            while (clipper.Step())
            {
                for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    const auto& line = LogFile[gsl::narrow<size_t>(i)];
                    if (line.c_str() == nullptr)
                        continue;

                    const auto& color = get_mark_color(Console_mark{line[0]});
                    pos = ImGui::GetCursorScreenPos();

                    // shadow
                    ImGui::SetCursorScreenPos(ImVec2{pos.x + 1.0f, pos.y + 1.0f});

                    const f32 a = std::min(color.w * 255.0f, 220.0f);
                    const f32 y = 0.299f * (color.x * 255.0f) + 0.587f * (color.y * 255.0f) + 0.114f * (color.z * 255.0f);
                    const f32 c = y > 40.0f ? 20.0f : 120.0f;

                    XR_DIAG_PUSH();
                    XR_DIAG_IGNORE("-Wold-style-cast");
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(c, c, c, a));
                    XR_DIAG_POP();

                    ImGui::TextUnformatted(line.c_str(), line.c_str() + line.size());
                    ImGui::PopStyleColor();

                    ImGui::SetCursorScreenPos(pos);

                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(line.c_str(), line.c_str() + line.size());
                    ImGui::PopStyleColor();
                }
            }

            if (ImGui::GetScrollY() + 1.0f >= prev_scroll_max)
                ImGui::SetScrollHereY(1.0f);

            prev_scroll_max = ImGui::GetScrollMaxY();
        }

        ImGui::EndChild();

        // Command input
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(prompt_font_color, "%s", ioc_prompt);
        ImGui::SameLine();

        pos = ImGui::GetCursorScreenPos();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, {});
        ImGui::PushStyleColor(ImGuiCol_Text, cmd_font_color);
        ImGui::SetNextItemWidth(-140.0f);

        constexpr ImGuiInputTextFlags input_text_flags{ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit |
                                                       ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll};

        constexpr auto callback = [](ImGuiInputTextCallbackData* data) { return static_cast<ImGuiGameConsole*>(data->UserData)->InputCallback(data); };

        if (m_refresh_keyboard_focus)
        {
            ImGui::SetKeyboardFocusHere();
            m_refresh_keyboard_focus = false;
        }

        if (ImGui::InputText("##input", m_edit_string.data(), m_edit_string.size(), input_text_flags, callback, this))
        {
            if (m_select_tip < 0 || m_select_tip >= std::ssize(m_tips))
            {
                ExecuteCommand(m_edit_string.data());
                m_refresh_keyboard_focus = true;
            }
            else
            {
                const shared_str& str = m_tips[gsl::narrow_cast<size_t>(m_select_tip)].text;
                string1024 buf;

                if (m_tips_mode == 1)
                {
                    xr_strconcat(buf, str.c_str(), " ");
                    xr_strcpy(m_edit_string.data(), m_edit_string.size(), buf);
                }
                else if (m_tips_mode == 2)
                {
                    xr_strconcat(buf, m_cur_cmd.c_str(), " ", str.c_str());

                    if (gsl::zstring p = std::strstr(buf, "  (current)"); p != nullptr)
                        *p = '\0';

                    xr_strcpy(m_edit_string.data(), m_edit_string.size(), buf);
                }

                reset_selected_tip();

                if (ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetItemID()); state != nullptr)
                    state->ReloadUserBufAndMoveToEnd();
            }

            ImGui::SetKeyboardFocusHere(-1);
        }

        ImGui::SetItemDefaultFocus();
        ImGui::PopStyleColor(2);

        // Current keyboard language
        ImGui::SameLine();

        static std::array<char, 3> localeName;
        const LANGID langID = LOWORD(reinterpret_cast<DWORD_PTR>(GetKeyboardLayout(0)));
        const auto len = GetLocaleInfoA(langID, LOCALE_SISO639LANGNAME, localeName.data(), localeName.size());

        ImGui::TextColored(total_font_color, "[%s]", len > 0 ? localeName.data() : "??");

        // Amount of log lines
        ImGui::SameLine();
        ImGui::TextColored(total_font_color, "[%zd]", amount);

        pos.y = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();

        // Tips
        if (!m_disable_tips && !m_tips.empty())
        {
            constexpr ImGuiWindowFlags tips_window_flags{ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing};

            ImGui::PushStyleColor(ImGuiCol_WindowBg, tips_back_color);
            ImGui::PushStyleColor(ImGuiCol_Text, tips_font_color);

            const f32 tooltip_h = ImGui::GetTextLineHeightWithSpacing() * 20.0f;
            const auto max_count = std::min(std::ssize(m_tips), gsl::narrow_cast<gsl::index>(std::floor(500.0f / ImGui::GetTextLineHeightWithSpacing())));
            const f32 max_height = gsl::narrow_cast<f32>(max_count) * ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y;

            pos.y = std::min(pos.y, gsl::narrow_cast<f32>(Device.dwHeight) - std::min(max_height, tooltip_h));

            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSizeConstraints({300.0f, ImGui::GetTextLineHeightWithSpacing()}, {400.0f, tooltip_h});

            if (ImGui::Begin("##tooltip", nullptr, tips_window_flags))
            {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();

                ImGuiListClipper clipper;
                clipper.Begin(gsl::narrow_cast<s32>(std::ssize(m_tips)));

                // Scroll to focused item once
                static gsl::index itm_scroll_to{-1};

                if (itm_scroll_to != m_select_tip && m_select_tip >= 0)
                    clipper.IncludeItemByIndex(gsl::narrow_cast<s32>(m_select_tip));

                while (clipper.Step())
                {
                    for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                    {
                        const auto& line = m_tips[gsl::narrow<size_t>(i)].text;
                        if (line.empty())
                            continue;

                        gsl::czstring text = line.c_str();
                        pos = ImGui::GetCursorScreenPos();

                        if (const TipString& ts = m_tips[gsl::narrow_cast<size_t>(i)]; ts.HL_start >= 0 && ts.HL_finish >= 0 && ts.HL_start <= ts.HL_finish)
                        {
                            if (const auto str_size = std::ssize(ts.text); ts.HL_start < str_size && ts.HL_finish <= str_size)
                            {
                                const ImVec2 text_size_before = ImGui::CalcTextSize(text, text + ts.HL_start);
                                const ImVec2 text_size_after = ImGui::CalcTextSize(text + ts.HL_start, text + ts.HL_finish);
                                const ImVec2 char_pos{pos.x + text_size_before.x, pos.y};

                                draw_list->AddRectFilled(char_pos, {char_pos.x + text_size_after.x, char_pos.y + ImGui::GetTextLineHeight()}, tips_word_color);
                            }
                        }

                        ImGui::PushStyleColor(ImGuiCol_Header, tips_select_color);

                        ImGui::BeginDisabled(true);
                        ImGui::Selectable(text, i == m_select_tip);
                        ImGui::EndDisabled();

                        ImGui::PopStyleColor();

                        if (itm_scroll_to != m_select_tip && i == m_select_tip)
                        {
                            ImGui::SetScrollHereY(1.0f);
                            itm_scroll_to = m_select_tip;
                        }
                    }
                }
            }

            ImGui::End();
            ImGui::PopStyleColor(2);
        }
    }

    ImGui::End();

    ImGui::PopFont();
    ImGui::PopStyleColor(); // ImGuiCol_WindowBg

    ImGui::PopStyleVar(); // ImGuiStyleVar_WindowBorderSize
    ImGui::PopStyleVar(); // ImGuiStyleVar_FramePadding
    ImGui::PopStyleVar(); // ImGuiStyleVar_WindowPadding
}

void ImGuiGameConsole::Show()
{
    if (!bVisible)
    {
        bVisible = true;

        m_edit_string[0] = '\0';
        m_disable_tips = false;

        reset_cmd_history_idx();
        reset_selected_tip();
        update_tips();

        m_refresh_keyboard_focus = true;
    }

    Render();
}

void ImGuiGameConsole::Hide()
{
    if (!bVisible)
        return;

    bVisible = false;

    reset_selected_tip();
    update_tips();
}

s32 ImGuiGameConsole::InputCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion: {
        shared_str out_str;

        if (IConsole_Command* cc = find_next_cmd(m_edit_string.data(), out_str); cc != nullptr && !out_str.empty())
        {
            xr_strcpy(m_edit_string.data(), m_edit_string.size(), out_str.c_str());

            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, m_edit_string.data());
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
        const bool ctrl = ImGui::IsKeyDown(ImGuiMod_Ctrl);
        const bool alt = ImGui::IsKeyDown(ImGuiMod_Alt);

        if (ctrl && alt)
        {
            if (data->EventKey == ImGuiKey_UpArrow)
                Begin_tips();
            else if (data->EventKey == ImGuiKey_DownArrow)
                End_tips();
        }
        else if (alt)
        {
            if (data->EventKey == ImGuiKey_UpArrow)
                PageUp_tips();
            else if (data->EventKey == ImGuiKey_DownArrow)
                PageDown_tips();
        }
        else if ((m_tips.empty() || m_disable_tips) && !m_cmd_history.empty())
        {
            const auto cmd_idx = m_cmd_history_idx;

            if (data->EventKey == ImGuiKey_UpArrow)
                Prev_cmd();
            else if (data->EventKey == ImGuiKey_DownArrow)
                Next_cmd();

            if (cmd_idx != m_cmd_history_idx)
            {
                m_disable_tips = true;

                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, m_edit_string.data());
            }
        }
        else
        {
            if (data->EventKey == ImGuiKey_UpArrow)
                prev_selected_tip();
            else if (data->EventKey == ImGuiKey_DownArrow)
                next_selected_tip();
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackEdit: m_disable_tips = false; break;
    }

    return 0;
}

void ImGuiGameConsole::ExecuteCommand(gsl::czstring cmd_str)
{
    xr_string edt{cmd_str};
    trim(edt);

    if (edt.empty())
        return;

    AddCmdHistory(edt.c_str());

    xr_string first, last;
    split_cmd(edt, first, last);

    // search
    if (const auto it = Console->Commands.find(first.c_str()); it != Console->Commands.end())
    {
        if (IConsole_Command* cc = it->second; cc != nullptr && cc->bEnabled)
        {
            if (cc->bLowerCaseArgs)
                strlwr(last);

            if (last.empty())
            {
                if (cc->bEmptyArgsHandled)
                {
                    cc->Execute(last.c_str());
                }
                else
                {
                    IConsole_Command::TStatus stat;
                    cc->Status(stat);

                    Msg("- %s %s", cc->Name(), stat);
                }
            }
            else
            {
                cc->Execute(last.c_str());
                cc->add_to_LRU(shared_str{last.c_str()});
            }
        }
        else
        {
            Msg("! Unknown command: %s", first.c_str());
        }
    }
    else
    {
        Msg("! Unknown command: %s", first.c_str());
    }

    m_edit_string[0] = '\0';
    m_disable_tips = false;
}

void ImGuiGameConsole::AddCmdHistory(gsl::czstring str)
{
    reset_cmd_history_idx();
    reset_selected_tip();

    if (m_last_cmd.c_str() != nullptr && std::is_eq(xr_strcmp(m_last_cmd, str)))
        return;

    static constexpr std::array<char, 2> c{std::to_underlying(Console_mark::mark2), '\0'};

    Msg("%s %s", c.data(), str);

    shared_str sh{str};
    add_cmd_history(sh);
    m_last_cmd = std::move(sh);
}

void ImGuiGameConsole::Prev_cmd() // Ctrl + Up
{
    prev_cmd_history_idx();
    SelectCommand();
}

void ImGuiGameConsole::Next_cmd() // Ctrl + Down
{
    next_cmd_history_idx();
    SelectCommand();
}

void ImGuiGameConsole::Begin_tips()
{
    m_select_tip = 0;
    m_start_tip = 0;
}

void ImGuiGameConsole::End_tips()
{
    m_select_tip = std::ssize(m_tips) - 1;
    m_start_tip = m_select_tip - VIEW_TIPS_COUNT + 1;

    check_next_selected_tip();
}

void ImGuiGameConsole::PageUp_tips()
{
    m_select_tip -= VIEW_TIPS_COUNT;
    check_prev_selected_tip();
}

void ImGuiGameConsole::PageDown_tips()
{
    m_select_tip += VIEW_TIPS_COUNT;
    check_next_selected_tip();
}

void ImGuiGameConsole::SelectCommand()
{
    if (m_cmd_history.empty())
        return;

    VERIFY(m_cmd_history_idx >= 0 && m_cmd_history_idx < std::ssize(m_cmd_history));

    const auto it_rb = m_cmd_history.rbegin() + m_cmd_history_idx;
    xr_strcpy(m_edit_string.data(), m_edit_string.size(), it_rb->c_str());

    reset_selected_tip();
}
} // namespace detail
} // namespace xr
#endif // !IMGUI_DISABLE
