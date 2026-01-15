////////////////////////////////////////////////////////////////////////////
// Module : edit_actions.cpp
// Created : 04.03.2008
// Author : Evgeniy Sokolov
// Description : edit actions chars class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "line_edit_control.h"
#include "edit_actions.h"
#include "xr_input.h"

namespace text_editor
{
base::~base() { xr_delete(m_previous_action); }

void base::on_assign(base* const prev_action) { m_previous_action = prev_action; }

tmc::task<void> base::on_key_press(line_edit_control* const control)
{
    if (m_previous_action)
        co_await m_previous_action->on_key_press(control);
}

// -------------------------------------------------------------------------------------------------

callback_base::callback_base(Callback const& callback, key_state state)
{
    m_callback = callback;
    m_run_state = state;
}

callback_base::~callback_base() = default;

tmc::task<void> callback_base::on_key_press(line_edit_control* const control)
{
    if (control->get_key_state(m_run_state))
    {
        co_await m_callback();
        co_return;
    }

    co_await base::on_key_press(control);
}

// -------------------------------------------------------------------------------------------------

type_pair::type_pair(u32 dik, char c, char c_shift, bool b_translate) { init(dik, c, c_shift, b_translate); }
type_pair::~type_pair() = default;

void type_pair::init(u32 dik, char c, char c_shift, bool b_translate)
{
    m_translate = b_translate;
    m_dik = dik;
    m_char = c;
    m_char_shift = c_shift;
}

tmc::task<void> type_pair::on_key_press(line_edit_control* const control)
{
    char c = 0;
    if (m_translate)
    {
        c = static_cast<char>(pInput->DikToChar(m_dik, false));
    }
    else
    {
        c = m_char;
        if (control->get_key_state(ks_Shift) != control->get_key_state(ks_CapsLock))
        {
            c = m_char_shift;
        }
    }

    control->insert_character(c);
    co_return;
}

// -------------------------------------------------------------------------------------------------

key_state_base::key_state_base(key_state state, base* type_pair) : m_state{state}, m_type_pair{type_pair} {}
key_state_base::~key_state_base() { xr_delete(m_type_pair); }

tmc::task<void> key_state_base::on_key_press(line_edit_control* const control)
{
    control->set_key_state(m_state, true);

    if (m_type_pair)
        co_await m_type_pair->on_key_press(control);
}
} // namespace text_editor
