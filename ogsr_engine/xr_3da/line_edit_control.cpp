////////////////////////////////////////////////////////////////////////////
// Module : line_edit_control.cpp
// Created : 21.02.2008
// Author : Evgeniy Sokolov
// Description : line edit control class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "line_edit_control.h"

#include "os_clipboard.h"
#include "../COMMON_AI/object_broker.h"
#include "xr_input.h"
#include "../xrCore/buffer_vector.h"

#include "edit_actions.h"

namespace
{
constexpr float g_console_sensitive{0.15f};
}

namespace text_editor
{
static bool terminate_char(char c, bool check_space = false)
{
    switch (c)
    {
    case ' ': return check_space;
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '<':
    case '>':
    case '\'':
    case '\"':
    case '=':
    case '+':
    case '-':
    case '*':
    case '\\':
    case '/':
    case '&':
    case '|':
    case '!':
    case '@':
    case '#':
    case '~':
    case '`':
    case '$':
    case '%':
    case '^':
    case ':':
    case ';':
    case '?':
    case ',':
    case '.':
    case '_': return true;
    }
    return false;
}

// -------------------------------------------------------------------------------------------------

line_edit_control::line_edit_control(u32 str_buffer_size)
{
    init(str_buffer_size);

    update_key_states();
}

line_edit_control::~line_edit_control()
{
    xr_free(m_edit_str);
    xr_free(m_inserted);
    xr_free(m_undo_buf);
    xr_free(m_buf0);
    xr_free(m_buf1);
    xr_free(m_buf2);
    xr_free(m_buf3);

    constexpr size_t array_size = sizeof(m_actions) / sizeof(m_actions[0]);
    buffer_vector<Base*> actions(m_actions, array_size, &m_actions[0], &m_actions[0] + array_size);
    std::ranges::sort(actions);
    actions.erase(std::unique(actions.begin(), actions.end()), actions.end());
    delete_data(actions);
}

namespace
{
[[nodiscard]] constexpr bool get_caps_lock_state() { return false; }
} // namespace

void line_edit_control::update_key_states()
{
    m_key_state.zero();

    set_key_state(ks_LShift, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LShift}));
    set_key_state(ks_RShift, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RShift}));
    set_key_state(ks_LCtrl, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LControl}));
    set_key_state(ks_RCtrl, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RControl}));
    set_key_state(ks_LAlt, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LAlt}));
    set_key_state(ks_RAlt, pInput->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RAlt}));
    set_key_state(ks_CapsLock, text_editor::get_caps_lock_state());
}

void line_edit_control::clear_states()
{
    m_edit_str[0] = 0;
    clear_inserted();
    m_undo_buf[0] = 0;

    m_buf0[0] = 0;
    m_buf1[0] = 0;
    m_buf2[0] = 0;
    m_buf3[0] = 0;

    m_cur_pos = 0;
    m_inserted_pos = 0;
    m_select_start = 0;
    m_p1 = 0;
    m_p2 = 0;

    m_accel = 1.0f;
    m_cur_time = 0.0f;
    m_rep_time = 0.0f;
    m_last_frame_time = 0;
    m_last_key_time = 0.0f;
    m_last_changed_frame = 0;

    m_hold_mode = false;
    m_insert_mode = false;
    m_repeat_mode = false;
    m_mark = false;
    m_cursor_view = false;
    m_need_update = false;
    m_unselected_mode = false;

    update_key_states();
}

void line_edit_control::init(u32 str_buffer_size, init_mode mode)
{
    m_buffer_size = str_buffer_size;
    clamp(m_buffer_size, (int)MIN_BUF_SIZE, (int)MAX_BUF_SIZE);

    xr_free(m_edit_str);
    m_edit_str = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));
    xr_free(m_inserted);
    m_inserted = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));
    xr_free(m_undo_buf);
    m_undo_buf = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));

    xr_free(m_buf0);
    m_buf0 = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));
    xr_free(m_buf1);
    m_buf1 = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));
    xr_free(m_buf2);
    m_buf2 = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));
    xr_free(m_buf3);
    m_buf3 = (LPSTR)xr_malloc(m_buffer_size * sizeof(char));

    clear_states();

    for (auto& act : m_actions)
        xr_delete(act);

    if (mode == im_read_only)
    {
        assign_callback(sf::Keyboard::Scancode::A, ks_Ctrl, CallMe::fromMethod<&line_edit_control::select_all_buf>(this));
        assign_callback(sf::Keyboard::Scancode::C, ks_Ctrl, CallMe::fromMethod<&line_edit_control::copy_to_clipboard>(this));
        assign_callback(sf::Keyboard::Scancode::Insert, ks_Ctrl, CallMe::fromMethod<&line_edit_control::copy_to_clipboard>(this));

        assign_callback(sf::Keyboard::Scancode::Home, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_home>(this));
        assign_callback(sf::Keyboard::Scancode::End, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_end>(this));
        assign_callback(sf::Keyboard::Scancode::Left, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_left>(this));
        assign_callback(sf::Keyboard::Scancode::Right, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_right>(this));
        assign_callback(sf::Keyboard::Scancode::Left, ks_Ctrl, CallMe::fromMethod<&line_edit_control::move_pos_left_word>(this));
        assign_callback(sf::Keyboard::Scancode::Right, ks_Ctrl, CallMe::fromMethod<&line_edit_control::move_pos_right_word>(this));
    }
    else
    {
        assign_char_pairs(mode);

        assign_callback(sf::Keyboard::Scancode::Insert, ks_free, CallMe::fromMethod<&line_edit_control::flip_insert_mode>(this));
        assign_callback(sf::Keyboard::Scancode::A, ks_Ctrl, CallMe::fromMethod<&line_edit_control::select_all_buf>(this));
        assign_callback(sf::Keyboard::Scancode::Z, ks_Ctrl, CallMe::fromMethod<&line_edit_control::undo_buf>(this));

        assign_callback(sf::Keyboard::Scancode::C, ks_Ctrl, CallMe::fromMethod<&line_edit_control::copy_to_clipboard>(this));
        assign_callback(sf::Keyboard::Scancode::V, ks_Ctrl, CallMe::fromMethod<&line_edit_control::paste_from_clipboard>(this));
        assign_callback(sf::Keyboard::Scancode::X, ks_Ctrl, CallMe::fromMethod<&line_edit_control::cut_to_clipboard>(this));

        assign_callback(sf::Keyboard::Scancode::Insert, ks_Ctrl, CallMe::fromMethod<&line_edit_control::copy_to_clipboard>(this));
        assign_callback(sf::Keyboard::Scancode::Insert, ks_Shift, CallMe::fromMethod<&line_edit_control::paste_from_clipboard>(this));
        assign_callback(sf::Keyboard::Scancode::Delete, ks_Shift, CallMe::fromMethod<&line_edit_control::cut_to_clipboard>(this));

        assign_callback(sf::Keyboard::Scancode::Home, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_home>(this));
        assign_callback(sf::Keyboard::Scancode::End, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_end>(this));
        assign_callback(sf::Keyboard::Scancode::Left, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_left>(this));
        assign_callback(sf::Keyboard::Scancode::Right, ks_free, CallMe::fromMethod<&line_edit_control::move_pos_right>(this));
        assign_callback(sf::Keyboard::Scancode::Left, ks_Ctrl, CallMe::fromMethod<&line_edit_control::move_pos_left_word>(this));
        assign_callback(sf::Keyboard::Scancode::Right, ks_Ctrl, CallMe::fromMethod<&line_edit_control::move_pos_right_word>(this));

        assign_callback(sf::Keyboard::Scancode::Backspace, ks_free, CallMe::fromMethod<&line_edit_control::delete_selected_back>(this));
        assign_callback(sf::Keyboard::Scancode::Delete, ks_free, CallMe::fromMethod<&line_edit_control::delete_selected_forward>(this));
        assign_callback(sf::Keyboard::Scancode::Backspace, ks_Ctrl, CallMe::fromMethod<&line_edit_control::delete_word_back>(this));
        assign_callback(sf::Keyboard::Scancode::Delete, ks_Ctrl, CallMe::fromMethod<&line_edit_control::delete_word_forward>(this));

        assign_callback(sf::Keyboard::Scancode::LShift, ks_Ctrl, CallMe::fromMethod<&line_edit_control::SwitchKL>(this));
        assign_callback(sf::Keyboard::Scancode::LShift, ks_Alt, CallMe::fromMethod<&line_edit_control::SwitchKL>(this));
    } // if mode

    create_key_state(sf::Keyboard::Scancode::LShift, ks_LShift);
    create_key_state(sf::Keyboard::Scancode::RShift, ks_RShift);
    create_key_state(sf::Keyboard::Scancode::LControl, ks_LCtrl);
    create_key_state(sf::Keyboard::Scancode::RControl, ks_RCtrl);
    create_key_state(sf::Keyboard::Scancode::LAlt, ks_LAlt);
    create_key_state(sf::Keyboard::Scancode::RAlt, ks_RAlt);
}

void line_edit_control::assign_char_pairs(init_mode mode)
{
    create_char_pair(sf::Keyboard::Scancode::Numpad0, '0', '0');
    create_char_pair(sf::Keyboard::Scancode::Numpad1, '1', '1');
    create_char_pair(sf::Keyboard::Scancode::Numpad2, '2', '2');
    create_char_pair(sf::Keyboard::Scancode::Numpad3, '3', '3');
    create_char_pair(sf::Keyboard::Scancode::Numpad4, '4', '4');
    create_char_pair(sf::Keyboard::Scancode::Numpad5, '5', '5');
    create_char_pair(sf::Keyboard::Scancode::Numpad6, '6', '6');
    create_char_pair(sf::Keyboard::Scancode::Numpad7, '7', '7');
    create_char_pair(sf::Keyboard::Scancode::Numpad8, '8', '8');
    create_char_pair(sf::Keyboard::Scancode::Numpad9, '9', '9');

    if (mode == im_number_only)
    {
        create_char_pair(sf::Keyboard::Scancode::Num0, '0', '0');
        create_char_pair(sf::Keyboard::Scancode::Num1, '1', '1');
        create_char_pair(sf::Keyboard::Scancode::Num2, '2', '2');
        create_char_pair(sf::Keyboard::Scancode::Num3, '3', '3');
        create_char_pair(sf::Keyboard::Scancode::Num4, '4', '4');
        create_char_pair(sf::Keyboard::Scancode::Num5, '5', '5');
        create_char_pair(sf::Keyboard::Scancode::Num6, '6', '6');
        create_char_pair(sf::Keyboard::Scancode::Num7, '7', '7');
        create_char_pair(sf::Keyboard::Scancode::Num8, '8', '8');
        create_char_pair(sf::Keyboard::Scancode::Num9, '9', '9');
        create_char_pair(sf::Keyboard::Scancode::NumpadMinus, '-', '-');
        create_char_pair(sf::Keyboard::Scancode::Hyphen, '-', '-');
        create_char_pair(sf::Keyboard::Scancode::NumpadPlus, '+', '+');
        create_char_pair(sf::Keyboard::Scancode::Equal, '+', '+');
        return;
    }

    if (mode != im_file_name_mode)
    {
        create_char_pair(sf::Keyboard::Scancode::Num0, '0', ')', true);
        create_char_pair(sf::Keyboard::Scancode::Num1, '1', '!', true);
        create_char_pair(sf::Keyboard::Scancode::Num2, '2', '@', true);
        create_char_pair(sf::Keyboard::Scancode::Num3, '3', '#', true);
        create_char_pair(sf::Keyboard::Scancode::Num4, '4', '$', true);
        create_char_pair(sf::Keyboard::Scancode::Num5, '5', '%', true);
        create_char_pair(sf::Keyboard::Scancode::Num6, '6', '^', true);
        create_char_pair(sf::Keyboard::Scancode::Num7, '7', '&', true);
        create_char_pair(sf::Keyboard::Scancode::Num8, '8', '*', true);
        create_char_pair(sf::Keyboard::Scancode::Num9, '9', '(', true);

        create_char_pair(sf::Keyboard::Scancode::Backslash, '\\', '|', true);
        create_char_pair(sf::Keyboard::Scancode::LBracket, '[', '{', true);
        create_char_pair(sf::Keyboard::Scancode::RBracket, ']', '}', true);
        create_char_pair(sf::Keyboard::Scancode::Apostrophe, '\'', '\"', true);
        create_char_pair(sf::Keyboard::Scancode::Comma, ',', '<', true);
        create_char_pair(sf::Keyboard::Scancode::Period, '.', '>', true);
        create_char_pair(sf::Keyboard::Scancode::Equal, '=', '+', true);
        create_char_pair(sf::Keyboard::Scancode::Semicolon, ';', ':', true);
        create_char_pair(sf::Keyboard::Scancode::Slash, '/', '?', true);

        create_char_pair(sf::Keyboard::Scancode::NumpadMultiply, '*', '*');
        create_char_pair(sf::Keyboard::Scancode::NumpadDivide, '/', '/');
    }
    else
    {
        create_char_pair(sf::Keyboard::Scancode::Num0, '0', '0');
        create_char_pair(sf::Keyboard::Scancode::Num1, '1', '1');
        create_char_pair(sf::Keyboard::Scancode::Num2, '2', '2');
        create_char_pair(sf::Keyboard::Scancode::Num3, '3', '3');
        create_char_pair(sf::Keyboard::Scancode::Num4, '4', '4');
        create_char_pair(sf::Keyboard::Scancode::Num5, '5', '5');
        create_char_pair(sf::Keyboard::Scancode::Num6, '6', '6');
        create_char_pair(sf::Keyboard::Scancode::Num7, '7', '7');
        create_char_pair(sf::Keyboard::Scancode::Num8, '8', '8');
        create_char_pair(sf::Keyboard::Scancode::Num9, '9', '9');
    }

    create_char_pair(sf::Keyboard::Scancode::NumpadMinus, '-', '-');
    create_char_pair(sf::Keyboard::Scancode::NumpadPlus, '+', '+');
    create_char_pair(sf::Keyboard::Scancode::NumpadDecimal, '.', '.');

    create_char_pair(sf::Keyboard::Scancode::Hyphen, '-', '_', true);
    create_char_pair(sf::Keyboard::Scancode::Space, ' ', ' ');
    create_char_pair(sf::Keyboard::Scancode::Grave, '`', '~', true);

    create_char_pair(sf::Keyboard::Scancode::A, 'a', 'A', true);
    create_char_pair(sf::Keyboard::Scancode::B, 'b', 'B', true);
    create_char_pair(sf::Keyboard::Scancode::C, 'c', 'C', true);
    create_char_pair(sf::Keyboard::Scancode::D, 'd', 'D', true);
    create_char_pair(sf::Keyboard::Scancode::E, 'e', 'E', true);
    create_char_pair(sf::Keyboard::Scancode::F, 'f', 'F', true);
    create_char_pair(sf::Keyboard::Scancode::G, 'g', 'G', true);
    create_char_pair(sf::Keyboard::Scancode::H, 'h', 'H', true);
    create_char_pair(sf::Keyboard::Scancode::I, 'i', 'I', true);
    create_char_pair(sf::Keyboard::Scancode::J, 'j', 'J', true);
    create_char_pair(sf::Keyboard::Scancode::K, 'k', 'K', true);
    create_char_pair(sf::Keyboard::Scancode::L, 'l', 'L', true);
    create_char_pair(sf::Keyboard::Scancode::M, 'm', 'M', true);
    create_char_pair(sf::Keyboard::Scancode::N, 'n', 'N', true);
    create_char_pair(sf::Keyboard::Scancode::O, 'o', 'O', true);
    create_char_pair(sf::Keyboard::Scancode::P, 'p', 'P', true);
    create_char_pair(sf::Keyboard::Scancode::Q, 'q', 'Q', true);
    create_char_pair(sf::Keyboard::Scancode::R, 'r', 'R', true);
    create_char_pair(sf::Keyboard::Scancode::S, 's', 'S', true);
    create_char_pair(sf::Keyboard::Scancode::T, 't', 'T', true);
    create_char_pair(sf::Keyboard::Scancode::U, 'u', 'U', true);
    create_char_pair(sf::Keyboard::Scancode::V, 'v', 'V', true);
    create_char_pair(sf::Keyboard::Scancode::W, 'w', 'W', true);
    create_char_pair(sf::Keyboard::Scancode::X, 'x', 'X', true);
    create_char_pair(sf::Keyboard::Scancode::Y, 'y', 'Y', true);
    create_char_pair(sf::Keyboard::Scancode::Z, 'z', 'Z', true);
}

void line_edit_control::create_key_state(sf::Keyboard::Scancode dik, key_state state)
{
    Base* prev = m_actions[std::to_underlying(dik)];
    // if ( m_actions[dik] )
    //{
    //  xr_delete( m_actions[dik] );
    // }
    m_actions[std::to_underlying(dik)] = xr_new<text_editor::key_state_base>(state, prev);
}

void line_edit_control::create_char_pair(sf::Keyboard::Scancode dik, char c, char c_shift, bool translate)
{
    xr_delete(m_actions[std::to_underlying(dik)]);
    m_actions[std::to_underlying(dik)] = xr_new<text_editor::type_pair>(dik, c, c_shift, translate);
}

void line_edit_control::assign_callback(sf::Keyboard::Scancode dik, key_state state, Callback const& callback)
{
    VERIFY(std::to_underlying(dik) < sf::Keyboard::ScancodeCount);

    Base* prev_action = m_actions[std::to_underlying(dik)];
    m_actions[std::to_underlying(dik)] = xr_new<text_editor::callback_base>(callback, state);
    m_actions[std::to_underlying(dik)]->on_assign(prev_action);
}

void line_edit_control::insert_character(char c)
{
    VERIFY(m_inserted_pos < (m_buffer_size - 1 /*trailing zero*/));
    m_inserted[m_inserted_pos] = c;
    m_inserted[m_inserted_pos + 1] = 0;
    m_inserted_pos++;
}

void line_edit_control::clear_inserted()
{
    m_inserted[0] = m_inserted[1] = 0;
    m_inserted_pos = 0;
}

bool line_edit_control::empty_inserted() const { return m_inserted_pos == 0; }

void line_edit_control::set_edit(LPCSTR str)
{
    if (!str)
        str = "";

    gsl::index str_size = std::clamp(xr_strlen(str), 0z, gsl::index{m_buffer_size} - 1);
    strncpy_s(m_edit_str, m_buffer_size, str, str_size);
    m_edit_str[str_size] = 0;

    m_cur_pos = str_size;
    m_select_start = m_cur_pos;
    m_accel = 1.0f;
    update_bufs();
}

// ========================================================

tmc::task<void> line_edit_control::on_key_press(xr::key_id dik)
{
    if (!dik.is<sf::Keyboard::Scancode>())
        co_return;

    if (!m_hold_mode)
    {
        m_last_key_time = 0.0f;
        m_accel = 1.0f;
    }

    m_mark = true;

    clamp_cur_pos();
    clear_inserted();
    compute_positions();

    const auto key = dik.get<sf::Keyboard::Scancode>();

    if (m_actions[std::to_underlying(key)] != nullptr)
        co_await m_actions[std::to_underlying(key)]->on_key_press(this);

    // ===========
    if (key == sf::Keyboard::Scancode::LControl || key == sf::Keyboard::Scancode::RControl)
        m_mark = false;

    m_edit_str[m_buffer_size - 1] = 0;
    clamp_cur_pos();

    add_inserted_text();

    if (m_mark && (!get_key_state(ks_Shift) || !empty_inserted()))
        m_select_start = m_cur_pos;

    compute_positions();

    m_repeat_mode = false;
    m_rep_time = 0.0f;

    update_key_states();
    update_bufs();
}

tmc::task<void> line_edit_control::on_key_hold(xr::key_id dik)
{
    if (!dik.is<sf::Keyboard::Scancode>())
        co_return;

    update_key_states();
    update_bufs();

    switch (dik.get<sf::Keyboard::Scancode>())
    {
    case sf::Keyboard::Scancode::Tab:
    case sf::Keyboard::Scancode::LShift:
    case sf::Keyboard::Scancode::RShift:
    case sf::Keyboard::Scancode::LControl:
    case sf::Keyboard::Scancode::RControl:
    case sf::Keyboard::Scancode::LAlt:
    case sf::Keyboard::Scancode::RAlt: co_return;
    default: break;
    }

    if (m_repeat_mode && m_last_key_time > 3.0f * g_console_sensitive)
    {
        float buf_time = m_rep_time;
        m_hold_mode = true;

        co_await on_key_press(dik);

        m_hold_mode = false;
        m_rep_time = buf_time;
    }
}

void line_edit_control::on_key_release(xr::key_id dik)
{
    if (!dik.is<sf::Keyboard::Scancode>())
        return;

    m_accel = 1.0f;
    m_rep_time = 0.0f;
    m_last_key_time = 0.0f;

    update_key_states();
    update_bufs();
}

void line_edit_control::on_frame()
{
    update_key_states();

    u32 fr_time = Device.dwTimeContinual;
    float dt = (fr_time - m_last_frame_time) * 0.001f;
    if (dt > 0.06666f)
    {
        dt = 0.06666f;
    }
    m_last_frame_time = fr_time;
    m_cur_time += dt;

    m_cursor_view = true;
    if (m_cur_time > 0.3f)
    {
        m_cursor_view = false;
    }
    if (m_cur_time > 0.4f)
    {
        m_cur_time = 0.0f;
    }

    m_rep_time += dt * m_accel;
    if (m_rep_time > g_console_sensitive) // 0.2
    {
        m_rep_time = 0.0f;
        m_repeat_mode = true;
        m_accel += 0.2f;
    }
    m_last_key_time += dt;

    if (m_last_changed_frame + 1 < Device.dwFrame)
    {
        m_need_update = false;
    }

    /*if ( Device.dwFrame % 100 == 0 )
    {
    Msg( " cur_time=%.2f re=%d acc=%.2f rep_time=%.2f", cur_time, bRepeat, fAccel, rep_time );
    }*/
}

void line_edit_control::update_bufs()
{
    // separate_buffer
    m_buf0[0] = 0;
    m_buf1[0] = 0;
    m_buf2[0] = 0;
    m_buf3[0] = 0;

    auto edit_size = xr_strlen(m_edit_str);
    int ds = (m_cursor_view && m_insert_mode && m_p2 < edit_size) ? 1 : 0;
    strncpy_s(m_buf0, m_buffer_size, m_edit_str, m_cur_pos);
    strncpy_s(m_buf1, m_buffer_size, m_edit_str, m_p1);
    strncpy_s(m_buf2, m_buffer_size, m_edit_str + m_p1, m_p2 - m_p1 + ds);
    // xrSimpodin: m_p2 может быть больше чем edit_size, а потому что edit_size unsigned, вместо отрицательного числа получится огромное положительное число.
    strncpy_s(m_buf3, m_buffer_size, m_edit_str + m_p2 + ds, edit_size < m_p2 ? 0 : (edit_size - m_p2 - ds));

    m_need_update = true;
    m_last_changed_frame = Device.dwFrame;
    // if ( m_cursor_view ) {
    // Msg( " m_p1=%d m_p2=%d cur=%d sstart=%d", m_p1, m_p2, m_cur_pos, m_select_start ); }
}

void line_edit_control::add_inserted_text()
{
    if (empty_inserted())
        return;

    const auto old_edit_size = xr_strlen(m_edit_str);
    for (gsl::index i{}; i < old_edit_size; ++i)
    {
        if ((m_edit_str[i] == '\n') || (m_edit_str[i] == '\t'))
            m_edit_str[i] = ' ';
    }

    LPSTR buf = (LPSTR)_alloca((m_buffer_size + 1) * sizeof(char));

    strncpy_s(buf, m_buffer_size, m_edit_str, m_p1); // part 1
    strncpy_s(m_undo_buf, m_buffer_size, m_edit_str + m_p1, m_p2 - m_p1);

    auto new_size = xr_strlen(m_inserted);
    if (m_buffer_size - 1 < m_p1 + new_size)
    {
        m_inserted[m_buffer_size - 1 - m_p1] = 0;
        new_size = xr_strlen(m_inserted);
    }
    strncpy_s(buf + m_p1, m_buffer_size - m_p1, m_inserted, _min(new_size, gsl::index{m_buffer_size - m_p1})); // part 2

    const u8 ds = (m_insert_mode && m_p2 < old_edit_size) ? 1 : 0;
    strncpy_s(buf + m_p1 + new_size, m_buffer_size - (m_p1 + new_size), m_edit_str + m_p2 + ds, _min(old_edit_size - m_p2 - ds, m_buffer_size - m_p1 - new_size)); // part 3
    buf[m_buffer_size] = 0;

    const gsl::index szn = m_p1 + new_size + old_edit_size - m_p2 - ds;
    if (szn < m_buffer_size)
    {
        strncpy_s(m_edit_str, m_buffer_size, buf, szn); // part 1+2+3
        m_edit_str[m_buffer_size - 1] = 0;
        m_cur_pos = m_p1 + new_size;
    }

    clamp_cur_pos();
}

//------------------------------------------------

tmc::task<void> line_edit_control::copy_to_clipboard()
{
    if (m_p1 >= m_p2)
        co_return;

    auto edit_len = xr_strlen(m_edit_str);
    xr::inlined_vector<char, 128> buf(edit_len + 1);

    strncpy_s(buf.data(), edit_len + 1, m_edit_str + m_p1, m_p2 - m_p1);
    buf[edit_len] = '\0';

    os_clipboard::copy_to_clipboard(buf.data());
    m_mark = false;
}

tmc::task<void> line_edit_control::paste_from_clipboard()
{
    os_clipboard::paste_from_clipboard(m_inserted, m_buffer_size - 1);
    m_inserted_pos += xr_strlen(m_inserted);

    co_return;
}

tmc::task<void> line_edit_control::cut_to_clipboard()
{
    co_await copy_to_clipboard();
    co_await delete_selected_forward();
}

tmc::task<void> line_edit_control::undo_buf()
{
    xr_strcpy(m_inserted, m_buffer_size, m_undo_buf);
    m_undo_buf[0] = 0;

    co_return;
}

tmc::task<void> line_edit_control::select_all_buf()
{
    m_select_start = 0;
    m_cur_pos = xr_strlen(m_edit_str);
    m_mark = false;

    co_return;
}

tmc::task<void> line_edit_control::flip_insert_mode()
{
    m_insert_mode = !m_insert_mode;
    co_return;
}

tmc::task<void> line_edit_control::delete_selected_back()
{
    delete_selected(true);
    co_return;
}

tmc::task<void> line_edit_control::delete_selected_forward()
{
    delete_selected(false);
    co_return;
}

void line_edit_control::delete_selected(bool back)
{
    clamp_cur_pos();

    auto edit_len = xr_strlen(m_edit_str);
    if (edit_len > 0)
    {
        if (back)
        {
            u8 dp = ((m_p1 == m_p2) && m_p1 > 0) ? 1 : 0;
            strncpy_s(m_undo_buf, m_buffer_size, m_edit_str + m_p1 - dp, m_p2 - m_p1 + dp);
            strncpy_s(m_edit_str + m_p1 - dp, m_buffer_size - (m_p1 - dp), m_edit_str + m_p2, edit_len - m_p2);
            m_cur_pos = m_p1 - dp;
        }
        else
        {
            u8 dn = ((m_p1 == m_p2) && m_p2 < edit_len) ? 1 : 0;
            strncpy_s(m_undo_buf, m_buffer_size, m_edit_str + m_p1, m_p2 - m_p1 + dn);
            strncpy_s(m_edit_str + m_p1, m_buffer_size - m_p1, m_edit_str + m_p2 + dn, edit_len - m_p2 - dn);
            m_cur_pos = m_p1;
        }
        clamp_cur_pos();
    }
    m_select_start = m_cur_pos;
}

tmc::task<void> line_edit_control::delete_word_back()
{
    bool const left_shift = get_key_state(ks_LShift);
    bool const right_shift = get_key_state(ks_RShift);
    set_key_state(ks_Shift, true);

    co_await move_pos_left_word();
    compute_positions();
    delete_selected(true);

    set_key_state(ks_LShift, left_shift);
    set_key_state(ks_RShift, right_shift);
}

tmc::task<void> line_edit_control::delete_word_forward()
{
    set_key_state(ks_Shift, true);
    co_await move_pos_right_word();

    compute_positions();
    delete_selected(false);
    set_key_state(ks_Shift, false);
}

tmc::task<void> line_edit_control::move_pos_home()
{
    m_cur_pos = 0;
    co_return;
}

tmc::task<void> line_edit_control::move_pos_end()
{
    m_cur_pos = xr_strlen(m_edit_str);
    co_return;
}

tmc::task<void> line_edit_control::move_pos_left()
{
    --m_cur_pos;
    co_return;
}

tmc::task<void> line_edit_control::move_pos_right()
{
    ++m_cur_pos;
    co_return;
}

tmc::task<void> line_edit_control::move_pos_left_word()
{
    int i = m_cur_pos - 1;
    while (i >= 0 && m_edit_str[i] == ' ')
        --i;

    if (!terminate_char(m_edit_str[i]))
    {
        while (i >= 0 && !terminate_char(m_edit_str[i], true))
            --i;

        ++i;
    }

    m_cur_pos = i;
    co_return;
}

tmc::task<void> line_edit_control::move_pos_right_word()
{
    auto edit_len = xr_strlen(m_edit_str);
    int i = m_cur_pos + 1;

    while (i < edit_len && !terminate_char(m_edit_str[i], true))
        ++i;

    while (i < edit_len && m_edit_str[i] == ' ')
        ++i;

    m_cur_pos = i;
    co_return;
}

void line_edit_control::compute_positions()
{
    m_p1 = m_cur_pos;
    m_p2 = m_cur_pos;
    if (m_unselected_mode)
    {
        return;
    }

    if (m_cur_pos > m_select_start)
    {
        m_p1 = m_select_start;
    }
    else if (m_cur_pos < m_select_start)
    {
        m_p2 = m_select_start;
    }
}

void line_edit_control::clamp_cur_pos() { clamp(m_cur_pos, 0, gsl::narrow_cast<s32>(xr_strlen(m_edit_str))); }

tmc::task<void> line_edit_control::SwitchKL()
{
    // У нас переключение языка находится в xr_input, поэтому здесь не нужно.
    // ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
    co_return;
}
} // namespace text_editor
