#pragma once

#include "IInputReceiver.h"

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// описание класса

class CInput final : public pureFrame, public pureAppActivate, public pureAppDeactivate
{
    RTTI_DECLARE_TYPEINFO(CInput, pureFrame, pureAppActivate, pureAppDeactivate);

private:
    xr_vector<std::tuple<u32, std::size_t, std::ptrdiff_t>> keyboard_events;
    xr::bitset<sf::Keyboard::ScancodeCount> keyboard_state;

    xr_vector<std::pair<u32, std::size_t>> mouse_events;
    xr::bitset<sf::Mouse::ButtonCount> mouse_state;

    xr_vector<IInputReceiver*> cbStack;

    xr_vector<std::byte> mouse_buffer;
    gsl::index mouse_dx{0};
    gsl::index mouse_dy{0};

    unsigned long tid{0};
    bool caps{false};

    tmc::task<void> KeyUpdate();
    tmc::task<void> MouseUpdate();

    CInput();
    tmc::task<void> co_CInput();

public:
    static tmc::task<void> co_create();
    ~CInput() override;

    tmc::task<void> Attach();

    tmc::task<void> iCapture(IInputReceiver* pc);
    tmc::task<void> iRelease(IInputReceiver* pc);
    [[nodiscard]] bool iGetAsyncKeyState(xr::key_id dik) const;

    tmc::task<void> OnFrame() override;
    tmc::task<void> OnAppActivate() override;
    tmc::task<void> OnAppDeactivate() override;

    [[nodiscard]] IInputReceiver* CurrentIR() const;

    void keyboard_event(u32 msg, std::size_t wp, std::ptrdiff_t lp)
    {
        // Prevent Win keys from showing Start Menu: inject a fake key event which makes
        // Windows think it was a [non-existent] hotkey combination.
        if ((wp == VK_LWIN || wp == VK_RWIN) && (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN))
        {
            std::array<::INPUT, 2> fake{};

            fake[0].type = INPUT_KEYBOARD;
            fake[0].ki.wVk = 0xe8;

            fake[1].type = INPUT_KEYBOARD;
            fake[1].ki.wVk = 0xe8;
            fake[1].ki.dwFlags = KEYEVENTF_KEYUP;

            ::SendInput(fake.size(), fake.data(), sizeof(::INPUT));
        }

        // Filter-out the fake key code used above
        if (wp != 0xe8)
            keyboard_events.emplace_back(msg, wp, lp);
    }

    void mouse_event(u32 msg, std::size_t wp) { mouse_events.emplace_back(msg, wp); }
    void mouse_move();

    // Возвращает символ по коду клавиши. Учитывается переключение языка, зажатый shift и caps lock
    // В случае неудачи функция возвращает '\0'.
    [[nodiscard]] char32_t DikToChar(sf::Keyboard::Scancode dik) const;

    void clip_cursor(bool clip);
};

extern CInput* pInput;
