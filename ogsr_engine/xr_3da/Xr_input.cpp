#include "stdafx.h"

#include "xr_input.h"

#include "IGame_Persistent.h"
#include "IInputReceiver.h"

#include <SFML/Window/Joystick.hpp>

#include <hidusage.h>

CInput* pInput{nullptr};

namespace
{
IInputReceiver dummyController;
}

float psMouseSens = 1.f;
float psMouseSensScale = 1.f;
Flags32 psMouseInvert = {FALSE};

CInput::CInput()
{
    keyboard_events.reserve(16);
    mouse_events.reserve(16);

    mouse_buffer.reserve(4096);
    mouse_buffer.resize(mouse_buffer.capacity());
}

tmc::task<void> CInput::co_CInput()
{
    //===================== Dummy pack
    co_await iCapture(&dummyController);

    Device.seqAppActivate.Add(this);
    Device.seqAppDeactivate.Add(this);
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH);
}

tmc::task<void> CInput::co_create()
{
    pInput = new (xr_alloc<CInput>(1)) CInput{};
    co_await pInput->co_CInput();
}

CInput::~CInput()
{
    Device.seqFrame.Remove(this);
    Device.seqAppDeactivate.Remove(this);
    Device.seqAppActivate.Remove(this);
}

tmc::task<void> CInput::Attach()
{
    co_await tmc::resume_on(xr::tmc_cpu_st_executor());

    const ::RAWINPUTDEVICE rid{.usUsagePage = HID_USAGE_PAGE_GENERIC, .usUsage = HID_USAGE_GENERIC_MOUSE, .dwFlags = 0, .hwndTarget = Device.m_hWnd};
    XR_ASSERT(::RegisterRawInputDevices(&rid, 1, sizeof(rid)), "", xr::GetLastError());

    tid = ::GetWindowThreadProcessId(Device.m_hWnd, nullptr);
    caps = ::GetKeyState(VK_CAPITAL) & 0x1;
}

//-----------------------------------------------------------------------

namespace xr
{
namespace
{
class keyconv final
{
private:
    std::array<sf::Keyboard::Scancode, 0x90> to_scan;
    std::array<sf::Keyboard::Scancode, 0x90> to_ext;
    std::array<s32, sf::Keyboard::ScancodeCount> to_code{0};

    [[nodiscard]] constexpr sf::Keyboard::Scancode init(s32 code, bool ext)
    {
        switch (code)
        {
        case 0x01: return sf::Keyboard::Scancode::Escape;
        case 0x02: return sf::Keyboard::Scancode::Num1;
        case 0x03: return sf::Keyboard::Scancode::Num2;
        case 0x04: return sf::Keyboard::Scancode::Num3;
        case 0x05: return sf::Keyboard::Scancode::Num4;
        case 0x06: return sf::Keyboard::Scancode::Num5;
        case 0x07: return sf::Keyboard::Scancode::Num6;
        case 0x08: return sf::Keyboard::Scancode::Num7;
        case 0x09: return sf::Keyboard::Scancode::Num8;
        case 0x0A: return sf::Keyboard::Scancode::Num9;
        case 0x0B: return sf::Keyboard::Scancode::Num0;
        case 0x0C: return sf::Keyboard::Scancode::Hyphen;
        case 0x0D: return sf::Keyboard::Scancode::Equal;
        case 0x0E: return sf::Keyboard::Scancode::Backspace;
        case 0x0F: return sf::Keyboard::Scancode::Tab;
        case 0x10: return ext ? sf::Keyboard::Scancode::MediaPreviousTrack : sf::Keyboard::Scancode::Q;
        case 0x11: return sf::Keyboard::Scancode::W;
        case 0x12: return sf::Keyboard::Scancode::E;
        case 0x13: return sf::Keyboard::Scancode::R;
        case 0x14: return sf::Keyboard::Scancode::T;
        case 0x15: return sf::Keyboard::Scancode::Y;
        case 0x16: return sf::Keyboard::Scancode::U;
        case 0x17: return sf::Keyboard::Scancode::I;
        case 0x18: return sf::Keyboard::Scancode::O;
        case 0x19: return ext ? sf::Keyboard::Scancode::MediaNextTrack : sf::Keyboard::Scancode::P;
        case 0x1A: return sf::Keyboard::Scancode::LBracket;
        case 0x1B: return sf::Keyboard::Scancode::RBracket;
        case 0x1C: return ext ? sf::Keyboard::Scancode::NumpadEnter : sf::Keyboard::Scancode::Enter;
        case 0x1D: return ext ? sf::Keyboard::Scancode::RControl : sf::Keyboard::Scancode::LControl;
        case 0x1E: return ext ? sf::Keyboard::Scancode::Select : sf::Keyboard::Scancode::A;
        case 0x1F: return sf::Keyboard::Scancode::S;
        case 0x20: return ext ? sf::Keyboard::Scancode::VolumeMute : sf::Keyboard::Scancode::D;
        case 0x21: return ext ? sf::Keyboard::Scancode::LaunchApplication1 : sf::Keyboard::Scancode::F;
        case 0x22: return ext ? sf::Keyboard::Scancode::MediaPlayPause : sf::Keyboard::Scancode::G;
        case 0x23: return sf::Keyboard::Scancode::H;
        case 0x24: return ext ? sf::Keyboard::Scancode::MediaStop : sf::Keyboard::Scancode::J;
        case 0x25: return sf::Keyboard::Scancode::K;
        case 0x26: return sf::Keyboard::Scancode::L;
        case 0x27: return sf::Keyboard::Scancode::Semicolon;
        case 0x28: return sf::Keyboard::Scancode::Apostrophe;
        case 0x29: return sf::Keyboard::Scancode::Grave;
        case 0x2A: return sf::Keyboard::Scancode::LShift;
        case 0x2B: return sf::Keyboard::Scancode::Backslash;
        case 0x2C: return sf::Keyboard::Scancode::Z;
        case 0x2D: return sf::Keyboard::Scancode::X;
        case 0x2E: return ext ? sf::Keyboard::Scancode::VolumeDown : sf::Keyboard::Scancode::C;
        case 0x2F: return sf::Keyboard::Scancode::V;
        case 0x30: return ext ? sf::Keyboard::Scancode::VolumeUp : sf::Keyboard::Scancode::B;
        case 0x31: return sf::Keyboard::Scancode::N;
        case 0x32: return ext ? sf::Keyboard::Scancode::HomePage : sf::Keyboard::Scancode::M;
        case 0x33: return sf::Keyboard::Scancode::Comma;
        case 0x34: return sf::Keyboard::Scancode::Period;
        case 0x35: return ext ? sf::Keyboard::Scancode::NumpadDivide : sf::Keyboard::Scancode::Slash;
        case 0x36: return sf::Keyboard::Scancode::RShift;
        case 0x37: return ext ? sf::Keyboard::Scancode::PrintScreen : sf::Keyboard::Scancode::NumpadMultiply;
        case 0x38: return ext ? sf::Keyboard::Scancode::RAlt : sf::Keyboard::Scancode::LAlt;
        case 0x39: return sf::Keyboard::Scancode::Space;
        case 0x3A: return sf::Keyboard::Scancode::CapsLock;
        case 0x3B: return sf::Keyboard::Scancode::F1;
        case 0x3C: return sf::Keyboard::Scancode::F2;
        case 0x3D: return sf::Keyboard::Scancode::F3;
        case 0x3E: return sf::Keyboard::Scancode::F4;
        case 0x3F: return sf::Keyboard::Scancode::F5;
        case 0x40: return sf::Keyboard::Scancode::F6;
        case 0x41: return sf::Keyboard::Scancode::F7;
        case 0x42: return sf::Keyboard::Scancode::F8;
        case 0x43: return sf::Keyboard::Scancode::F9;
        case 0x44: return sf::Keyboard::Scancode::F10;
        case 0x45: return ext ? sf::Keyboard::Scancode::NumLock : sf::Keyboard::Scancode::Pause;
        case 0x46: return sf::Keyboard::Scancode::ScrollLock;
        case 0x47: return ext ? sf::Keyboard::Scancode::Home : sf::Keyboard::Scancode::Numpad7;
        case 0x48: return ext ? sf::Keyboard::Scancode::Up : sf::Keyboard::Scancode::Numpad8;
        case 0x49: return ext ? sf::Keyboard::Scancode::PageUp : sf::Keyboard::Scancode::Numpad9;
        case 0x4A: return sf::Keyboard::Scancode::NumpadMinus;
        case 0x4B: return ext ? sf::Keyboard::Scancode::Left : sf::Keyboard::Scancode::Numpad4;
        case 0x4C: return sf::Keyboard::Scancode::Numpad5;
        case 0x4D: return ext ? sf::Keyboard::Scancode::Right : sf::Keyboard::Scancode::Numpad6;
        case 0x4E: return sf::Keyboard::Scancode::NumpadPlus;
        case 0x4F: return ext ? sf::Keyboard::Scancode::End : sf::Keyboard::Scancode::Numpad1;
        case 0x50: return ext ? sf::Keyboard::Scancode::Down : sf::Keyboard::Scancode::Numpad2;
        case 0x51: return ext ? sf::Keyboard::Scancode::PageDown : sf::Keyboard::Scancode::Numpad3;
        case 0x52: return ext ? sf::Keyboard::Scancode::Insert : sf::Keyboard::Scancode::Numpad0;
        case 0x53: return ext ? sf::Keyboard::Scancode::Delete : sf::Keyboard::Scancode::NumpadDecimal;
        case 0x56: return sf::Keyboard::Scancode::NonUsBackslash;
        case 0x57: return sf::Keyboard::Scancode::F11;
        case 0x58: return sf::Keyboard::Scancode::F12;
        case 0x5B: return ext ? sf::Keyboard::Scancode::LSystem : sf::Keyboard::Scancode::Unknown;
        case 0x5C: return ext ? sf::Keyboard::Scancode::RSystem : sf::Keyboard::Scancode::Unknown;
        case 0x5D: return ext ? sf::Keyboard::Scancode::Menu : sf::Keyboard::Scancode::Unknown;
        case 0x63: return ext ? sf::Keyboard::Scancode::Help : sf::Keyboard::Scancode::Unknown;
        case 0x64: return sf::Keyboard::Scancode::F13;
        case 0x65: return ext ? sf::Keyboard::Scancode::Search : sf::Keyboard::Scancode::F14;
        case 0x66: return ext ? sf::Keyboard::Scancode::Favorites : sf::Keyboard::Scancode::F15;
        case 0x67: return ext ? sf::Keyboard::Scancode::Refresh : sf::Keyboard::Scancode::F16;
        case 0x68: return ext ? sf::Keyboard::Scancode::Stop : sf::Keyboard::Scancode::F17;
        case 0x69: return ext ? sf::Keyboard::Scancode::Forward : sf::Keyboard::Scancode::F18;
        case 0x6A: return ext ? sf::Keyboard::Scancode::Back : sf::Keyboard::Scancode::F19;
        case 0x6B: return ext ? sf::Keyboard::Scancode::LaunchApplication1 : sf::Keyboard::Scancode::F20;
        case 0x6C: return ext ? sf::Keyboard::Scancode::LaunchMail : sf::Keyboard::Scancode::F21;
        case 0x6D: return ext ? sf::Keyboard::Scancode::LaunchMediaSelect : sf::Keyboard::Scancode::F22;
        case 0x6E: return sf::Keyboard::Scancode::F23;
        case 0x76: return sf::Keyboard::Scancode::F24;
        case 0x7E: return sf::Keyboard::Scancode::NumpadEqual;
        case 0x8D: return sf::Keyboard::Scancode::NumpadEqual;
        default: return sf::Keyboard::Scancode::Unknown;
        }
    }

public:
    constexpr keyconv()
    {
        for (auto [code, scan, ext] : std::views::zip(std::views::indices(std::ssize(to_scan)), to_scan, to_ext))
        {
            scan = init(code, false);
            ext = init(code, true);

            if (scan != sf::Keyboard::Scancode::Unknown)
                to_code[std::to_underlying(scan)] = code;
        }
    }

    [[nodiscard]] constexpr sf::Keyboard::Scancode scan(s32 code, bool ext) const
    {
        return code < std::ssize(to_scan) ? (ext ? to_ext : to_scan)[code] : sf::Keyboard::Scancode::Unknown;
    }

    [[nodiscard]] constexpr s32 code(sf::Keyboard::Scancode scan) const
    {
        if (scan == sf::Keyboard::Scancode::Unknown || std::to_underlying(scan) >= std::ssize(to_code))
            return 0;

        return to_code[std::to_underlying(scan)];
    }
};

constexpr keyconv keyconv;
} // namespace
} // namespace xr

tmc::task<void> CInput::KeyUpdate()
{
    const bool editor = !!xr::editor();
    auto new_state = keyboard_state;

    for (auto [msg, wp, lp] : keyboard_events)
    {
        bool pressed = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
        if (pressed && (HIWORD(lp) & KF_REPEAT))
            continue;

        s32 code;
        bool ext;

        // Num Lock and Pause are broken beyond belief
        if (wp == VK_PAUSE || wp == VK_NUMLOCK)
        {
            code = 0x45;
            ext = wp == VK_NUMLOCK;
        }
        else
        {
            code = LOBYTE(HIWORD(lp));
            ext = !!(HIWORD(lp) & KF_EXTENDED);
        }

        // Rare system events
        if (code == 0)
            code = gsl::narrow_cast<s32>(::MapVirtualKeyW(wp, MAPVK_VK_TO_VSC));

        const auto key = xr::keyconv.scan(code, ext);
        if (key == sf::Keyboard::Scancode::Unknown)
            continue;

        if (pressed)
        {
            if (key == sf::Keyboard::Scancode::CapsLock)
                caps = !caps;

            if (!editor || !xr::editor()->key_press(xr::key_id{key}))
                co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{key});

            // Pause never generates a WM_KEYUP, release it immediately
            if (key == sf::Keyboard::Scancode::Pause)
                pressed = false;
        }

        if (!pressed)
        {
            if (!editor || !xr::editor()->key_release(xr::key_id{key}))
                cbStack.back()->IR_OnKeyboardRelease(xr::key_id{key});
        }

        new_state.set(std::to_underlying(key), pressed);
    }

    keyboard_events.clear();

    const auto hold = keyboard_state & new_state;
    auto idx = hold.first_one();

    while (idx < hold.size())
    {
        const auto key = xr::key_id{sf::Keyboard::Scancode{gsl::narrow_cast<s32>(idx)}};

        if (!editor || !xr::editor()->key_hold(key))
            co_await cbStack.back()->IR_OnKeyboardHold(key);

        idx = hold.next_one(idx + 1);
    }

    keyboard_state = new_state;
}

void CInput::mouse_move()
{
    while (true)
    {
        auto size = gsl::narrow_cast<u32>(mouse_buffer.size());
        auto ret = ::GetRawInputBuffer(reinterpret_cast<::RAWINPUT*>(mouse_buffer.data()), &size, sizeof(::RAWINPUTHEADER));

        if (ret == 0)
            return;

        if (ret == std::numeric_limits<u32>::max())
        {
            XR_ASSERT(xr::GetLastError() == xr::last_error{ERROR_INSUFFICIENT_BUFFER});

            mouse_buffer.reserve(size);
            mouse_buffer.resize(mouse_buffer.capacity());

            continue;
        }

        auto block = reinterpret_cast<const ::RAWINPUT*>(mouse_buffer.data());

#define QWORD u64
        while (ret > 0)
        {
            if (block->header.dwType == RIM_TYPEMOUSE && !(block->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
            {
                mouse_dx += block->data.mouse.lLastX;
                mouse_dy += block->data.mouse.lLastY;
            }

            block = NEXTRAWINPUTBLOCK(const_cast<::RAWINPUT*>(block));
            --ret;
        }
#undef QWORD
    }
}

tmc::task<void> CInput::MouseUpdate()
{
    const bool editor = !!xr::editor();
    auto new_state = mouse_state;
    gsl::index z{0};

    for (auto [msg, wp] : mouse_events)
    {
        if (msg == WM_MOUSEWHEEL)
        {
            z += GET_WHEEL_DELTA_WPARAM(wp);
            continue;
        }

        sf::Mouse::Button btn;
        bool pressed{false};

        switch (msg)
        {
        case WM_LBUTTONDOWN: pressed = true; [[fallthrough]];
        case WM_LBUTTONUP: btn = sf::Mouse::Button::Left; break;
        case WM_RBUTTONDOWN: pressed = true; [[fallthrough]];
        case WM_RBUTTONUP: btn = sf::Mouse::Button::Right; break;
        case WM_MBUTTONDOWN: pressed = true; [[fallthrough]];
        case WM_MBUTTONUP: btn = sf::Mouse::Button::Middle; break;
        case WM_XBUTTONDOWN: pressed = true; [[fallthrough]];
        case WM_XBUTTONUP: btn = GET_XBUTTON_WPARAM(wp) == XBUTTON1 ? sf::Mouse::Button::Extra1 : sf::Mouse::Button::Extra2; break;
        default: continue;
        }

        if (pressed)
        {
            if (!editor || !xr::editor()->key_press(xr::key_id{btn}))
                co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{btn});
        }
        else
        {
            if (!editor || !xr::editor()->key_release(xr::key_id{btn}))
                cbStack.back()->IR_OnKeyboardRelease(xr::key_id{btn});
        }

        new_state.set(std::to_underlying(btn), pressed);
    }

    mouse_events.clear();

    const auto hold = mouse_state & new_state;
    auto idx = hold.first_one();

    while (idx < hold.size())
    {
        const auto btn = xr::key_id{sf::Mouse::Button{gsl::narrow_cast<s32>(idx)}};

        if (!editor || !xr::editor()->key_hold(btn))
            co_await cbStack.back()->IR_OnKeyboardHold(btn);

        idx = hold.next_one(idx + 1);
    }

    if (mouse_dx != 0 || mouse_dy != 0)
    {
        if (!editor || !xr::editor()->mouse_move(mouse_dx, mouse_dy))
            cbStack.back()->IR_OnMouseMove(mouse_dx, mouse_dy);

        mouse_dx = 0;
        mouse_dy = 0;
    }

    if (z != 0)
    {
        if (!editor || !xr::editor()->mouse_wheel(z))
            co_await cbStack.back()->IR_OnMouseWheel(z);
    }

    mouse_state = new_state;
}

bool CInput::iGetAsyncKeyState(xr::key_id dik) const
{
    if (dik.is<sf::Keyboard::Scancode>())
        return keyboard_state[std::to_underlying(dik.get<sf::Keyboard::Scancode>())];

    if (dik.is<sf::Mouse::Button>())
        return mouse_state[std::to_underlying(dik.get<sf::Mouse::Button>())];

    if (dik.is<xr::key_id::joystick>())
    {
        for (u32 joy{0}; joy < sf::Joystick::Count; ++joy)
        {
            if (sf::Joystick::isButtonPressed(joy, gsl::narrow_cast<u32>(dik.get<xr::key_id::joystick>())))
                return true;
        }
    }

    return false;
}

tmc::task<void> CInput::iCapture(IInputReceiver* p)
{
    XR_ASSERT(p != nullptr);

    co_await MouseUpdate();
    co_await KeyUpdate();

    // change focus
    if (!cbStack.empty())
        cbStack.back()->IR_OnDeactivate();

    cbStack.push_back(p);
    co_await cbStack.back()->IR_OnActivate();
}

tmc::task<void> CInput::iRelease(IInputReceiver* p)
{
    if (p == cbStack.back())
    {
        cbStack.back()->IR_OnDeactivate();
        cbStack.pop_back();

        co_await cbStack.back()->IR_OnActivate();
        co_return;
    }

    // we are not topmost receiver, so remove the nearest one
    u32 cnt = cbStack.size();
    for (; cnt > 0; --cnt)
    {
        if (cbStack[cnt - 1] == p)
        {
            xr_vector<IInputReceiver*>::iterator it = cbStack.begin();
            std::advance(it, cnt - 1);
            cbStack.erase(it);
            break;
        }
    }
}

tmc::task<void> CInput::OnAppActivate()
{
    if (CurrentIR() != nullptr)
        co_await CurrentIR()->IR_OnActivate();

    keyboard_state.reset();
    mouse_state.reset();

    caps = false;
    mouse_dx = 0;
    mouse_dy = 0;
}

tmc::task<void> CInput::OnAppDeactivate()
{
    if (CurrentIR())
        CurrentIR()->IR_OnDeactivate();

    keyboard_state.reset();
    mouse_state.reset();

    caps = false;
    mouse_dx = 0;
    mouse_dy = 0;

    co_return;
}

tmc::task<void> CInput::OnFrame()
{
    Device.Statistic->Input.Begin();

    co_await KeyUpdate();
    co_await MouseUpdate();

    Device.Statistic->Input.End();
}

IInputReceiver* CInput::CurrentIR() const { return !cbStack.empty() ? cbStack.back() : nullptr; }

char32_t CInput::DikToChar(sf::Keyboard::Scancode dik) const
{
    const auto win = xr::keyconv.code(dik);
    if (win == 0)
        return '\0';

    const auto layout = ::GetKeyboardLayout(tid);
    if (layout == nullptr)
        return '\0';

    const auto virt = ::MapVirtualKeyExW(win, MAPVK_VSC_TO_VK_EX, layout);
    if (virt == 0)
        return '\0';

    std::array<u8, 256> ks{0};

    if (const auto lc = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::LControl)),
        rc = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::RControl));
        lc || rc)
    {
        ks[VK_CONTROL] = 0x80;
        ks[VK_LCONTROL] = lc ? 0x80 : 0;
        ks[VK_RCONTROL] = rc ? 0x80 : 0;
    }

    if (const auto ls = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::LShift)),
        rs = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::RShift));
        ls || rs)
    {
        ks[VK_SHIFT] = 0x80;
        ks[VK_LSHIFT] = ls ? 0x80 : 0;
        ks[VK_RSHIFT] = rs ? 0x80 : 0;
    }

    if (const auto la = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::LAlt)),
        ra = keyboard_state.test(std::to_underlying(sf::Keyboard::Scancode::RAlt));
        la || ra)
    {
        ks[VK_MENU] = 0x80;
        ks[VK_LMENU] = la ? 0x80 : 0;
        ks[VK_RMENU] = ra ? 0x80 : 0;
    }

    ks[VK_CAPITAL] = caps ? 0x1 : 0;

    std::array<wchar_t, 16> cb{'\0'};
    const auto result = ::ToUnicodeEx(virt, win, ks.data(), cb.data(), cb.size(), 0x4, layout);

    if (result <= 0 || result > 2)
        return '\0';
    if (result == 1)
        return cb[0];

    // UTF-32 character: a surrogate pair of UTF-16
    if (cb[0] < 0xd800 || cb[0] > 0xdbff || cb[1] < 0xdc00 || cb[1] > 0xdfff)
        return '\0';

    char32_t res;
    sf::Utf16::toUtf32(cb.begin(), cb.begin() + 2, &res);

    return res;
}

// https://stackoverflow.com/a/36827574
void CInput::clip_cursor(bool clip)
{
    if (clip)
    {
        ShowCursor(FALSE);
        if (Device.m_hWnd && g_screenmode != 2)
        {
            RECT rect;
            GetClientRect(Device.m_hWnd, &rect);

            POINT ul;
            ul.x = rect.left;
            ul.y = rect.top;

            POINT lr;
            lr.x = rect.right;
            lr.y = rect.bottom;

            MapWindowPoints(Device.m_hWnd, nullptr, &ul, 1);
            MapWindowPoints(Device.m_hWnd, nullptr, &lr, 1);

            rect.left = ul.x;
            rect.top = ul.y;

            rect.right = lr.x;
            rect.bottom = lr.y;

            ClipCursor(&rect);
        }
    }
    else
    {
        while (ShowCursor(TRUE) < 0)
            ;
        ClipCursor(nullptr);
    }
}
