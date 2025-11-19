////////////////////////////////////////////////////////////////////////////
// Module : edit_actions.h
// Created : 04.03.2008
// Author : Evgeniy Sokolov
// Description : edit actions class
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_ACTIONS_H_INCLUDED
#define EDIT_ACTIONS_H_INCLUDED

namespace text_editor
{
class line_edit_control;

class base : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(base);

public:
    base() = default;
    ~base() override;

    void on_assign(base* const prev_action);
    virtual void on_key_press(line_edit_control* const control);

protected:
    base* m_previous_action{};
};

// -------------------------------------------------------------------------------------------------

class callback_base : public base
{
    RTTI_DECLARE_TYPEINFO(callback_base, base);

private:
    using Callback = CallMe::Delegate<void()>;

public:
    explicit callback_base(Callback const& callback, key_state state);
    ~callback_base() override;

    virtual void on_key_press(line_edit_control* const control);

protected:
    key_state m_run_state;
    Callback m_callback;
};

// -------------------------------------------------------------------------------------------------

class type_pair : public base
{
    RTTI_DECLARE_TYPEINFO(type_pair, base);

public:
    explicit type_pair(u32 dik, char c, char c_shift, bool b_translate);
    ~type_pair() override;

    void init(u32 dik, char c, char c_shift, bool b_translate);
    virtual void on_key_press(line_edit_control* const control);

private:
    u32 m_dik;
    bool m_translate;
    char m_char;
    char m_char_shift;
};

// -------------------------------------------------------------------------------------------------

class key_state_base : public base
{
    RTTI_DECLARE_TYPEINFO(key_state_base, base);

public:
    explicit key_state_base(key_state state, base* type_pair);
    ~key_state_base() override;

    virtual void on_key_press(line_edit_control* const control);

private:
    key_state m_state;
    base* m_type_pair;
};
} // namespace text_editor

#endif // EDIT_ACTIONS_H_INCLUDED
