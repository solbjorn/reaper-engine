////////////////////////////////////////////////////////////////////////////
// Module : line_editor.cpp
// Created : 22.02.2008
// Author : Evgeniy Sokolov
// Description : line editor class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "line_editor.h"

namespace text_editor
{
line_editor::line_editor(u32 str_buffer_size) : m_control{str_buffer_size} {}
line_editor::~line_editor() = default;

void line_editor::on_frame() { m_control.on_frame(); }

tmc::task<void> line_editor::IR_OnKeyboardPress(gsl::index dik) { co_await m_control.on_key_press(dik); }
tmc::task<void> line_editor::IR_OnKeyboardHold(gsl::index dik) { co_await m_control.on_key_hold(dik); }
void line_editor::IR_OnKeyboardRelease(int) { m_control.on_key_release(); }
} // namespace text_editor
