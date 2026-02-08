////////////////////////////////////////////////////////////////////////////
// Module : line_edit_control.h
// Created : 21.02.2008
// Author : Evgeniy Sokolov
// Description : line edit control class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IInputReceiver.h"

namespace text_editor
{
class base;

enum key_state : u32
{
    ks_free = u32(0),
    ks_LShift = u32(1) << 0,
    ks_RShift = u32(1) << 1,
    ks_LCtrl = u32(1) << 2,
    ks_RCtrl = u32(1) << 3,
    ks_LAlt = u32(1) << 4,
    ks_RAlt = u32(1) << 5,
    ks_CapsLock = u32(1) << 6,

    ks_Shift = u32(ks_LShift | ks_RShift),
    ks_Ctrl = u32(ks_LCtrl | ks_RCtrl),
    ks_Alt = u32(ks_LAlt | ks_RAlt),
}; // enum key_state

enum init_mode
{
    im_standart = 0,
    im_number_only,
    im_read_only,
    im_file_name_mode,
    // not "/\\:*?\"<>|^()[]%"

    im_count
}; // init_mode

class line_edit_control
{
private:
    typedef text_editor::base Base;
    using Callback = CallMe::Delegate<tmc::task<void>()>;

public:
    explicit line_edit_control(u32 str_buffer_size);
    ~line_edit_control();

    void init(u32 str_buffer_size, init_mode mode = im_standart);
    void clear_states();
    tmc::task<void> on_key_press(xr::key_id dik);
    tmc::task<void> on_key_hold(xr::key_id dik);
    void on_key_release(xr::key_id dik);
    void on_frame();

    void assign_callback(sf::Keyboard::Scancode, key_state state, Callback const& callback);

    void insert_character(char c);

    IC bool get_key_state(key_state mask) const { return (mask) ? !!(m_key_state.test(mask)) : true; }
    IC void set_key_state(key_state mask, bool value) { m_key_state.set(mask, value); }

    IC bool cursor_view() const { return m_cursor_view; }
    IC bool need_update() const { return m_need_update; }

    IC LPCSTR str_edit() const { return m_edit_str; }
    IC LPCSTR str_before_cursor() const { return m_buf0; }
    IC LPCSTR str_before_mark() const { return m_buf1; }
    IC LPCSTR str_mark() const { return m_buf2; }
    IC LPCSTR str_after_mark() const { return m_buf3; }

    void set_edit(LPCSTR str);
    void set_selected_mode(bool status) { m_unselected_mode = !status; }
    bool get_selected_mode() const { return !m_unselected_mode; }

private:
    line_edit_control(line_edit_control const&);
    line_edit_control const& operator=(line_edit_control const&) = delete;

    void update_key_states();
    void update_bufs();

    tmc::task<void> undo_buf();
    tmc::task<void> select_all_buf();
    tmc::task<void> flip_insert_mode();

    tmc::task<void> copy_to_clipboard();
    tmc::task<void> paste_from_clipboard();
    tmc::task<void> cut_to_clipboard();

    tmc::task<void> move_pos_home();
    tmc::task<void> move_pos_end();
    tmc::task<void> move_pos_left();
    tmc::task<void> move_pos_right();
    tmc::task<void> move_pos_left_word();
    tmc::task<void> move_pos_right_word();

    tmc::task<void> delete_selected_back();
    tmc::task<void> delete_selected_forward();
    tmc::task<void> delete_word_back();
    tmc::task<void> delete_word_forward();
    tmc::task<void> SwitchKL();

    void assign_char_pairs(init_mode mode);
    void create_key_state(sf::Keyboard::Scancode, key_state state);
    void create_char_pair(sf::Keyboard::Scancode, char c, char c_shift, bool translate = false);

    void clear_inserted();
    bool empty_inserted() const;

    void add_inserted_text();

    void delete_selected(bool back);
    void compute_positions();
    void clamp_cur_pos();

private:
    Base* m_actions[sf::Keyboard::ScancodeCount]{};

    char* m_edit_str{};
    char* m_undo_buf{};
    char* m_inserted{};
    char* m_buf0{};
    char* m_buf1{};
    char* m_buf2{};
    char* m_buf3{};

    enum
    {
        MIN_BUF_SIZE = 8,
        MAX_BUF_SIZE = 4096
    };

    int m_buffer_size{};

    int m_cur_pos{};
    int m_inserted_pos{};
    int m_select_start{};
    int m_p1{};
    int m_p2{};

    float m_accel{};
    float m_cur_time{};
    float m_rep_time{};
    float m_last_key_time{};
    u32 m_last_frame_time{};
    u32 m_last_changed_frame{};

    Flags32 m_key_state{};

    bool m_hold_mode{};
    bool m_insert_mode{};
    bool m_repeat_mode{};
    bool m_mark{};
    bool m_cursor_view{};
    bool m_need_update{};
    bool m_unselected_mode{};
};
} // namespace text_editor
