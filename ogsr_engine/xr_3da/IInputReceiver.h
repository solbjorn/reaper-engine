// IInputReceiver.h: interface for the IInputReceiver class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <SFML/Window/Keyboard.hpp>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wfloat-equal");

#include <SFML/Window/Mouse.hpp>

XR_DIAG_POP();

namespace xr
{
class key_id
{
public:
    enum class joystick : s32
    {
        button1 = 0,
        button2,
        button3,
        button4,
        button5,
        button6,
        button7,
        button8,
        button9,
        button10,
        button11,
        button12,
        button13,
        button14,
        button15,
        button16,
        button17,
        button18,
        button19,
        button20,
        button21,
        button22,
        button23,
        button24,
        button25,
        button26,
        button27,
        button28,
        button29,
        button30,
        button31,
        button32,
    };

private:
    std::variant<sf::Keyboard::Scancode, sf::Mouse::Button, joystick> val{sf::Keyboard::Scancode::Unknown};

public:
    constexpr key_id() = default;
    constexpr explicit key_id(sf::Keyboard::Scancode key) : val{key} {}
    constexpr explicit key_id(sf::Mouse::Button btn) : val{btn} {}
    constexpr explicit key_id(joystick btn) : val{btn} {}

    template <typename T>
    [[nodiscard]] constexpr auto is() const
    {
        return std::holds_alternative<T>(val);
    }

    template <typename T>
    [[nodiscard]] constexpr auto get() const
    {
        return std::get<T>(val);
    }

    [[nodiscard]] constexpr auto operator==(const key_id& that) const { return val == that.val; }
    [[nodiscard]] constexpr auto operator<=>(const key_id& that) const { return val <=> that.val; }

    template <typename H>
    [[nodiscard]] friend constexpr H AbslHashValue(H h, const key_id& id)
    {
        return H::combine(std::move(h), id.val);
    }
};
static_assert(xr::key_id{sf::Keyboard::Scancode::A} != xr::key_id{sf::Mouse::Button::Left});
static_assert(xr::key_id{sf::Keyboard::Scancode::A} != xr::key_id{xr::key_id::joystick::button1});
} // namespace xr

class IInputReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IInputReceiver);

public:
    ~IInputReceiver() override = default;

    static void IR_GetLastMouseDelta(Ivector2& p);
    static void IR_GetMousePosScreen(Ivector2& p);
    static void IR_GetMousePosReal(HWND hwnd, Ivector2& p);
    static void IR_GetMousePosReal(Ivector2& p);
    static void IR_GetMousePosIndependent(Fvector2& f);
    static void IR_GetMousePosIndependentCrop(Fvector2& f);

    [[nodiscard]] bool IR_GetKeyState(xr::key_id dik) const;
    tmc::task<void> IR_Capture();
    tmc::task<void> IR_Release();

    virtual tmc::task<void> IR_OnActivate() { co_return; }
    virtual void IR_OnDeactivate();

    virtual tmc::task<void> IR_OnMouseWheel(gsl::index) { co_return; }
    virtual void IR_OnMouseMove(int, int) {}
    virtual void IR_OnMouseStop(int, int) {}

    virtual tmc::task<void> IR_OnKeyboardPress(xr::key_id) { co_return; }
    virtual void IR_OnKeyboardRelease(xr::key_id) {}
    virtual tmc::task<void> IR_OnKeyboardHold(xr::key_id) { co_return; }
};

extern float psMouseSens;
extern float psMouseSensScale;
extern Flags32 psMouseInvert;
