#include "stdafx.h"

#include "xr_input.h"

#include "IGame_Persistent.h"
#include "IInputReceiver.h"

#include <SFML/Window/Joystick.hpp>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define MOUSEBUFFERSIZE 64
#define KEYBOARDBUFFERSIZE 64

CInput* pInput{};

namespace
{
IInputReceiver dummyController;
}

float psMouseSens = 1.f;
float psMouseSensScale = 1.f;
Flags32 psMouseInvert = {FALSE};

CInput::CInput(bool exclusive) : is_exclusive_mode{exclusive} { Log("Starting INPUT device..."); }

tmc::task<void> CInput::co_CInput(u32 device)
{
    //===================== Dummy pack
    co_await iCapture(&dummyController);

    if (!pDI)
        CHK_DX(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&pDI, nullptr));

    // KEYBOARD
    if (device & keyboard_device_key)
        CHK_DX(CreateInputDevice(&pKeyboard, GUID_SysKeyboard, &c_dfDIKeyboard, KEYBOARDBUFFERSIZE));

    // MOUSE
    if (device & mouse_device_key)
        CHK_DX(CreateInputDevice(&pMouse, GUID_SysMouse, &c_dfDIMouse2, MOUSEBUFFERSIZE));

    Device.seqAppActivate.Add(this);
    Device.seqAppDeactivate.Add(this);
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH);
}

tmc::task<void> CInput::co_create(bool exclusive, u32 device)
{
    pInput = new (xr_alloc<CInput>(1)) CInput{exclusive};
    co_await pInput->co_CInput(device);
}

CInput::~CInput(void)
{
    Device.seqFrame.Remove(this);
    Device.seqAppDeactivate.Remove(this);
    Device.seqAppActivate.Remove(this);

    // Unacquire and release the device's interfaces
    if (pMouse)
    {
        pMouse->Unacquire();
        _RELEASE(pMouse);
    }

    if (pKeyboard)
    {
        pKeyboard->Unacquire();
        _RELEASE(pKeyboard);
    }

    _SHOW_REF("Input: ", pDI);
    _RELEASE(pDI);
}

//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CInput::CreateInputDevice(IDirectInputDevice8W** device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, u32 buf_size)
{
    // Obtain an interface to the input device
    //.	CHK_DX( pDI->CreateDeviceEx( guidDevice, IID_IDirectInputDevice8, (void**)device, NULL ) );
    CHK_DX(pDI->CreateDevice(guidDevice, /*IID_IDirectInputDevice8,*/ device, NULL));

    // Set the device data format. Note: a data format specifies which
    // controls on a device we are interested in, and how they should be
    // reported.
    CHK_DX((*device)->SetDataFormat(pdidDataFormat));

    // setup the buffer size for the keyboard data
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = buf_size;

    CHK_DX((*device)->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

    return S_OK;
}

void CInput::Attach()
{
    // Set the cooperativity level to let DirectInput know how this device
    // should interact with the system and with other DirectInput applications.
    if (pKeyboard)
        R_CHK(pKeyboard->SetCooperativeLevel(Device.m_hWnd, (is_exclusive_mode ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND));
    if (pMouse)
        R_CHK(pMouse->SetCooperativeLevel(Device.m_hWnd, (is_exclusive_mode ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY));
}

//-----------------------------------------------------------------------

void CInput::SetAllAcquire(BOOL bAcquire)
{
    if (pMouse)
        bAcquire ? pMouse->Acquire() : pMouse->Unacquire();
    if (pKeyboard)
        bAcquire ? pKeyboard->Acquire() : pKeyboard->Unacquire();
}

void CInput::SetMouseAcquire(BOOL bAcquire)
{
    if (pMouse)
        bAcquire ? pMouse->Acquire() : pMouse->Unacquire();
}
void CInput::SetKBDAcquire(BOOL bAcquire)
{
    if (pKeyboard)
        bAcquire ? pKeyboard->Acquire() : pKeyboard->Unacquire();
}

void CInput::exclusive_mode(const bool exclusive)
{
    is_exclusive_mode = exclusive;

    pKeyboard->Unacquire();
    pMouse->Unacquire();

    Attach();

    pKeyboard->Acquire();
    pMouse->Acquire();
}

//-----------------------------------------------------------------------

namespace xr
{
namespace
{
class keyconv
{
private:
    std::array<s32, sf::Keyboard::ScancodeCount> to_dik{};
    std::array<sf::Keyboard::Scancode, 0xf0> to_scan;

    [[nodiscard]] constexpr sf::Keyboard::Scancode init(s32 dik)
    {
        switch (dik)
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
        case 0x10: return sf::Keyboard::Scancode::Q;
        case 0x11: return sf::Keyboard::Scancode::W;
        case 0x12: return sf::Keyboard::Scancode::E;
        case 0x13: return sf::Keyboard::Scancode::R;
        case 0x14: return sf::Keyboard::Scancode::T;
        case 0x15: return sf::Keyboard::Scancode::Y;
        case 0x16: return sf::Keyboard::Scancode::U;
        case 0x17: return sf::Keyboard::Scancode::I;
        case 0x18: return sf::Keyboard::Scancode::O;
        case 0x19: return sf::Keyboard::Scancode::P;
        case 0x1A: return sf::Keyboard::Scancode::LBracket;
        case 0x1B: return sf::Keyboard::Scancode::RBracket;
        case 0x1C: return sf::Keyboard::Scancode::Enter;
        case 0x1D: return sf::Keyboard::Scancode::LControl;
        case 0x1E: return sf::Keyboard::Scancode::A;
        case 0x1F: return sf::Keyboard::Scancode::S;
        case 0x20: return sf::Keyboard::Scancode::D;
        case 0x21: return sf::Keyboard::Scancode::F;
        case 0x22: return sf::Keyboard::Scancode::G;
        case 0x23: return sf::Keyboard::Scancode::H;
        case 0x24: return sf::Keyboard::Scancode::J;
        case 0x25: return sf::Keyboard::Scancode::K;
        case 0x26: return sf::Keyboard::Scancode::L;
        case 0x27: return sf::Keyboard::Scancode::Semicolon;
        case 0x28: return sf::Keyboard::Scancode::Apostrophe;
        case 0x29: return sf::Keyboard::Scancode::Grave;
        case 0x2A: return sf::Keyboard::Scancode::LShift;
        case 0x2B: return sf::Keyboard::Scancode::Backslash;
        case 0x2C: return sf::Keyboard::Scancode::Z;
        case 0x2D: return sf::Keyboard::Scancode::X;
        case 0x2E: return sf::Keyboard::Scancode::C;
        case 0x2F: return sf::Keyboard::Scancode::V;
        case 0x30: return sf::Keyboard::Scancode::B;
        case 0x31: return sf::Keyboard::Scancode::N;
        case 0x32: return sf::Keyboard::Scancode::M;
        case 0x33: return sf::Keyboard::Scancode::Comma;
        case 0x34: return sf::Keyboard::Scancode::Period;
        case 0x35: return sf::Keyboard::Scancode::Slash;
        case 0x36: return sf::Keyboard::Scancode::RShift;
        case 0x37: return sf::Keyboard::Scancode::NumpadMultiply;
        case 0x38: return sf::Keyboard::Scancode::LAlt;
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
        case 0x45: return sf::Keyboard::Scancode::NumLock;
        case 0x46: return sf::Keyboard::Scancode::ScrollLock;
        case 0x47: return sf::Keyboard::Scancode::Numpad7;
        case 0x48: return sf::Keyboard::Scancode::Numpad8;
        case 0x49: return sf::Keyboard::Scancode::Numpad9;
        case 0x4A: return sf::Keyboard::Scancode::NumpadMinus;
        case 0x4B: return sf::Keyboard::Scancode::Numpad4;
        case 0x4C: return sf::Keyboard::Scancode::Numpad5;
        case 0x4D: return sf::Keyboard::Scancode::Numpad6;
        case 0x4E: return sf::Keyboard::Scancode::NumpadPlus;
        case 0x4F: return sf::Keyboard::Scancode::Numpad1;
        case 0x50: return sf::Keyboard::Scancode::Numpad2;
        case 0x51: return sf::Keyboard::Scancode::Numpad3;
        case 0x52: return sf::Keyboard::Scancode::Numpad0;
        case 0x53: return sf::Keyboard::Scancode::NumpadDecimal;
        case 0x56: return sf::Keyboard::Scancode::NonUsBackslash;
        case 0x57: return sf::Keyboard::Scancode::F11;
        case 0x58: return sf::Keyboard::Scancode::F12;
        case 0x64: return sf::Keyboard::Scancode::F13;
        case 0x65: return sf::Keyboard::Scancode::F14;
        case 0x66: return sf::Keyboard::Scancode::F15;
        case 0x67: return sf::Keyboard::Scancode::F16;
        case 0x68: return sf::Keyboard::Scancode::F17;
        case 0x69: return sf::Keyboard::Scancode::F18;
        case 0x6A: return sf::Keyboard::Scancode::F19;
        case 0x6B: return sf::Keyboard::Scancode::F20;
        case 0x6C: return sf::Keyboard::Scancode::F21;
        case 0x6D: return sf::Keyboard::Scancode::F22;
        case 0x6E: return sf::Keyboard::Scancode::F23;
        case 0x76: return sf::Keyboard::Scancode::F24;
        case 0x7E: return sf::Keyboard::Scancode::NumpadEqual;
        case 0x8D: return sf::Keyboard::Scancode::NumpadEqual;
        case 0x90: return sf::Keyboard::Scancode::MediaPreviousTrack;
        case 0x99: return sf::Keyboard::Scancode::MediaNextTrack;
        case 0x9C: return sf::Keyboard::Scancode::NumpadEnter;
        case 0x9D: return sf::Keyboard::Scancode::RControl;
        case 0x9E: return sf::Keyboard::Scancode::Select;
        case 0xA0: return sf::Keyboard::Scancode::VolumeMute;
        case 0xA1: return sf::Keyboard::Scancode::LaunchApplication2;
        case 0xA2: return sf::Keyboard::Scancode::MediaPlayPause;
        case 0xA4: return sf::Keyboard::Scancode::MediaStop;
        case 0xAE: return sf::Keyboard::Scancode::VolumeDown;
        case 0xB0: return sf::Keyboard::Scancode::VolumeUp;
        case 0xB2: return sf::Keyboard::Scancode::HomePage;
        case 0xB5: return sf::Keyboard::Scancode::NumpadDivide;
        case 0xB7: return sf::Keyboard::Scancode::PrintScreen;
        case 0xB8: return sf::Keyboard::Scancode::RAlt;
        case 0xC5: return sf::Keyboard::Scancode::Pause;
        case 0xC7: return sf::Keyboard::Scancode::Home;
        case 0xC8: return sf::Keyboard::Scancode::Up;
        case 0xC9: return sf::Keyboard::Scancode::PageUp;
        case 0xCB: return sf::Keyboard::Scancode::Left;
        case 0xCD: return sf::Keyboard::Scancode::Right;
        case 0xCF: return sf::Keyboard::Scancode::End;
        case 0xD0: return sf::Keyboard::Scancode::Down;
        case 0xD1: return sf::Keyboard::Scancode::PageDown;
        case 0xD2: return sf::Keyboard::Scancode::Insert;
        case 0xD3: return sf::Keyboard::Scancode::Delete;
        case 0xDB: return sf::Keyboard::Scancode::LSystem;
        case 0xDC: return sf::Keyboard::Scancode::RSystem;
        case 0xDD: return sf::Keyboard::Scancode::Menu;
        case 0xE1: return sf::Keyboard::Scancode::Help;
        case 0xE5: return sf::Keyboard::Scancode::Search;
        case 0xE6: return sf::Keyboard::Scancode::Favorites;
        case 0xE7: return sf::Keyboard::Scancode::Refresh;
        case 0xE8: return sf::Keyboard::Scancode::Stop;
        case 0xE9: return sf::Keyboard::Scancode::Forward;
        case 0xEA: return sf::Keyboard::Scancode::Back;
        case 0xEB: return sf::Keyboard::Scancode::LaunchApplication1;
        case 0xEC: return sf::Keyboard::Scancode::LaunchMail;
        case 0xED: return sf::Keyboard::Scancode::LaunchMediaSelect;
        default: return sf::Keyboard::Scancode::Unknown;
        }
    }

public:
    constexpr keyconv()
    {
        for (auto [dik, scan] : xr::views_enumerate(to_scan))
        {
            scan = init(dik);

            if (scan != sf::Keyboard::Scancode::Unknown)
                to_dik[std::to_underlying(scan)] = dik;
        }
    }

    [[nodiscard]] constexpr sf::Keyboard::Scancode scan(s32 dik) const { return dik < std::ssize(to_scan) ? to_scan[dik] : sf::Keyboard::Scancode::Unknown; }
    [[nodiscard]] constexpr s32 dik(sf::Keyboard::Scancode scan) const
    {
        if (scan == sf::Keyboard::Scancode::Unknown || std::to_underlying(scan) >= std::ssize(to_dik))
            return 0;

        return to_dik[std::to_underlying(scan)];
    }
};

constexpr keyconv keyconv;
} // namespace
} // namespace xr

namespace
{
BOOL b_altF4 = FALSE;
}

tmc::task<void> CInput::KeyUpdate()
{
    HRESULT hr;
    DWORD dwElements = KEYBOARDBUFFERSIZE;
    DIDEVICEOBJECTDATA od[KEYBOARDBUFFERSIZE];

    VERIFY(pKeyboard);

    hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
    if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    {
        hr = pKeyboard->Acquire();
        if (hr != S_OK)
            co_return;

        hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
        if (hr != S_OK)
            co_return;
    }

    const bool editor = xr::editor() != nullptr;

    for (u32 i = 0; i < dwElements; ++i)
    {
        const auto key = xr::keyconv.scan(od[i].dwOfs);

        KBState.set(std::to_underlying(key), !!(od[i].dwData & 0x80));
        if (KBState[std::to_underlying(key)])
        {
            if (this->is_exclusive_mode && (key == sf::Keyboard::Scancode::LShift || key == sf::Keyboard::Scancode::RShift) &&
                (this->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LAlt}) || this->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RAlt})))
            {
                // Переключили язык. В эксклюзивном режиме это обязательно для правильной работы функции DikToChar
                auto scope = co_await tmc::enter(xr::tmc_cpu_st_executor());
                PostMessage(gGameWindow, WM_INPUTLANGCHANGEREQUEST, 2, 0);
                co_await scope.exit();
            }

            if (!editor || !xr::editor()->key_press(xr::key_id{key}))
                co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{key});
        }
        else
        {
            if (!editor || !xr::editor()->key_release(xr::key_id{key}))
                cbStack.back()->IR_OnKeyboardRelease(xr::key_id{key});
        }
    }

    for (s32 key{0}; key < s32{sf::Keyboard::ScancodeCount}; ++key)
    {
        if (!KBState[key])
            continue;

        if (!editor || !xr::editor()->key_hold(xr::key_id{sf::Keyboard::Scancode{key}}))
            co_await cbStack.back()->IR_OnKeyboardHold(xr::key_id{sf::Keyboard::Scancode{key}});
    }

    if (!b_altF4 && iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::F4}) &&
        (iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LAlt}) || iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RAlt})))
    {
        b_altF4 = TRUE;

        co_await Engine.Event.Defer("KERNEL:disconnect");
        co_await Engine.Event.Defer("KERNEL:quit");
    }
}

bool CInput::iGetAsyncKeyState(xr::key_id dik) const
{
    if (dik.is<sf::Keyboard::Scancode>())
    {
        // KRodin: да-да, я знаю, что этот код ужасен.
        const auto key = dik.get<sf::Keyboard::Scancode>();
        switch (key)
        {
        case sf::Keyboard::Scancode::Unknown: return false;
        case sf::Keyboard::Scancode::LAlt:
        case sf::Keyboard::Scancode::RAlt:
        case sf::Keyboard::Scancode::Tab:
        case sf::Keyboard::Scancode::LControl:
        case sf::Keyboard::Scancode::RControl:
        case sf::Keyboard::Scancode::Delete: return sf::Keyboard::isKeyPressed(key);
        default: return KBState[std::to_underlying(key)];
        }
    }

    if (dik.is<sf::Mouse::Button>())
        return mouseState[std::to_underlying(dik.get<sf::Mouse::Button>())];

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

tmc::task<void> CInput::MouseUpdate()
{
#pragma push_macro("FIELD_OFFSET")
#undef FIELD_OFFSET
#define FIELD_OFFSET offsetof // Фиксим warning C4644 - просто переводим макрос из винсдк на использование стандартного оффсетофа.

    HRESULT hr;
    DWORD dwElements = MOUSEBUFFERSIZE;
    DIDEVICEOBJECTDATA od[MOUSEBUFFERSIZE];

    VERIFY(pMouse);

    hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
    if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    {
        hr = pMouse->Acquire();
        if (hr != S_OK)
            co_return;

        hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
        if (hr != S_OK)
            co_return;
    }

    const auto mouse_prev = mouseState;
    const bool editor = xr::editor() != nullptr;

    offs[0] = offs[1] = offs[2] = 0;

    for (u32 i = 0; i < dwElements; i++)
    {
        switch (od[i].dwOfs)
        {
        case DIMOFS_X:
            offs[0] += od[i].dwData;
            timeStamp[0] = od[i].dwTimeStamp;
            break;
        case DIMOFS_Y:
            offs[1] += od[i].dwData;
            timeStamp[1] = od[i].dwTimeStamp;
            break;
        case DIMOFS_Z:
            offs[2] += od[i].dwData;
            timeStamp[2] = od[i].dwTimeStamp;
            break;
        case DIMOFS_BUTTON0:
            if (od[i].dwData & 0x80)
            {
                mouseState.set(std::to_underlying(sf::Mouse::Button::Left));

                if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button::Left}))
                    co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button::Left});
            }

            if (!(od[i].dwData & 0x80))
            {
                mouseState.reset(std::to_underlying(sf::Mouse::Button::Left));

                if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button::Left}))
                    cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button::Left});
            }

            break;
        case DIMOFS_BUTTON1:
            if (od[i].dwData & 0x80)
            {
                mouseState.set(std::to_underlying(sf::Mouse::Button::Right));

                if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button::Right}))
                    co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button::Right});
            }

            if (!(od[i].dwData & 0x80))
            {
                mouseState.reset(std::to_underlying(sf::Mouse::Button::Right));

                if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button::Right}))
                    cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button::Right});
            }

            break;
        case DIMOFS_BUTTON2:
            if (od[i].dwData & 0x80)
            {
                mouseState.set(std::to_underlying(sf::Mouse::Button::Middle));

                if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button::Middle}))
                    co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button::Middle});
            }

            if (!(od[i].dwData & 0x80))
            {
                mouseState.reset(std::to_underlying(sf::Mouse::Button::Middle));

                if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button::Middle}))
                    cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button::Middle});
            }

            break;
        case DIMOFS_BUTTON3:
            if (od[i].dwData & 0x80)
            {
                mouseState.set(std::to_underlying(sf::Mouse::Button::Extra1));

                if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button::Extra1}))
                    co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button::Extra1});
            }

            if (!(od[i].dwData & 0x80))
            {
                mouseState.reset(std::to_underlying(sf::Mouse::Button::Extra1));

                if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button::Extra1}))
                    cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button::Extra1});
            }

            break;
        case DIMOFS_BUTTON4:
            if (od[i].dwData & 0x80)
            {
                mouseState.set(std::to_underlying(sf::Mouse::Button::Extra2));

                if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button::Extra2}))
                    co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button::Extra2});
            }

            if (!(od[i].dwData & 0x80))
            {
                mouseState.reset(std::to_underlying(sf::Mouse::Button::Extra2));

                if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button::Extra2}))
                    cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button::Extra2});
            }

            break;
        }
    }

    // Giperion: double check mouse buttons state
    DIMOUSESTATE2 MouseState;
    hr = pMouse->GetDeviceState(sizeof(MouseState), &MouseState);

    if (hr == S_OK)
    {
        auto& state = *reinterpret_cast<const std::array<u8, 8>*>(&MouseState.rgbButtons);
        co_await RecheckMouseButtons(state, editor);
    }
    //-Giperion

    co_await isButtonsOnHold(mouse_prev, editor);

    if (dwElements)
    {
        if (offs[0] || offs[1])
        {
            if (!editor || !xr::editor()->mouse_move(offs[0], offs[1]))
                cbStack.back()->IR_OnMouseMove(offs[0], offs[1]);
        }

        if (offs[2])
        {
            if (!editor || !xr::editor()->mouse_wheel(offs[2]))
                co_await cbStack.back()->IR_OnMouseWheel(offs[2]);
        }
    }
    else
    {
        if (timeStamp[1] && ((dwCurTime - timeStamp[1]) >= mouse_dt))
            cbStack.back()->IR_OnMouseStop(DIMOFS_Y, timeStamp[1] = 0);
        if (timeStamp[0] && ((dwCurTime - timeStamp[0]) >= mouse_dt))
            cbStack.back()->IR_OnMouseStop(DIMOFS_X, timeStamp[0] = 0);
    }

#pragma pop_macro("FIELD_OFFSET")
}

tmc::task<void> CInput::RecheckMouseButtons(std::array<u8, 8> state, bool editor)
{
    for (s32 i{0}; i < s32{sf::Mouse::ButtonCount}; ++i)
    {
        if ((state[i] & 0x80) && !mouseState[i])
        {
            mouseState.set(i);

            if (!editor || !xr::editor()->key_press(xr::key_id{sf::Mouse::Button{i}}))
                co_await cbStack.back()->IR_OnKeyboardPress(xr::key_id{sf::Mouse::Button{i}});
        }
        else if (!(state[i] & 0x80) && mouseState[i])
        {
            mouseState.reset(i);

            if (!editor || !xr::editor()->key_release(xr::key_id{sf::Mouse::Button{i}}))
                cbStack.back()->IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button{i}});
        }
    }
}

tmc::task<void> CInput::isButtonsOnHold(xr::bitset<sf::Mouse::ButtonCount> mouse_prev, bool editor)
{
    for (s32 i{0}; i < s32{sf::Mouse::ButtonCount}; ++i)
    {
        if (!mouseState[i] || !mouse_prev[i])
            continue;

        if (!editor || !xr::editor()->key_hold(xr::key_id{sf::Mouse::Button{i}}))
            co_await cbStack.back()->IR_OnKeyboardHold(xr::key_id{sf::Mouse::Button{i}});
    }
}

tmc::task<void> CInput::iCapture(IInputReceiver* p)
{
    VERIFY(p);

    if (pMouse)
        co_await MouseUpdate();
    if (pKeyboard)
        co_await KeyUpdate();

    // change focus
    if (!cbStack.empty())
        cbStack.back()->IR_OnDeactivate();

    cbStack.push_back(p);
    co_await cbStack.back()->IR_OnActivate();

    // prepare for _new_ controller
    std::memset(timeStamp, 0, sizeof(timeStamp));
    std::memset(timeSave, 0, sizeof(timeSave));
    std::memset(offs, 0, sizeof(offs));
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

    SetAllAcquire(true);

    mouseState.reset();
    KBState.reset();
    std::memset(timeStamp, 0, sizeof(timeStamp));
    std::memset(timeSave, 0, sizeof(timeSave));
    std::memset(offs, 0, sizeof(offs));
}

tmc::task<void> CInput::OnAppDeactivate()
{
    if (CurrentIR())
        CurrentIR()->IR_OnDeactivate();

    SetAllAcquire(false);

    mouseState.reset();
    KBState.reset();
    std::memset(timeStamp, 0, sizeof(timeStamp));
    std::memset(timeSave, 0, sizeof(timeSave));
    std::memset(offs, 0, sizeof(offs));

    co_return;
}

tmc::task<void> CInput::OnFrame()
{
    Device.Statistic->Input.Begin();
    dwCurTime = Device.TimerAsync_MMT();

    if (pKeyboard)
        co_await KeyUpdate();
    if (pMouse)
        co_await MouseUpdate();

    Device.Statistic->Input.End();
}

IInputReceiver* CInput::CurrentIR() const { return !cbStack.empty() ? cbStack.back() : nullptr; }

u16 CInput::DikToChar(sf::Keyboard::Scancode dik, bool utf) const
{
    switch (dik)
    {
    // Эти клавиши через ToAscii не обработать, поэтому пропишем явно
    case sf::Keyboard::Scancode::Numpad0: return '0';
    case sf::Keyboard::Scancode::Numpad1: return '1';
    case sf::Keyboard::Scancode::Numpad2: return '2';
    case sf::Keyboard::Scancode::Numpad3: return '3';
    case sf::Keyboard::Scancode::Numpad4: return '4';
    case sf::Keyboard::Scancode::Numpad5: return '5';
    case sf::Keyboard::Scancode::Numpad6: return '6';
    case sf::Keyboard::Scancode::Numpad7: return '7';
    case sf::Keyboard::Scancode::Numpad8: return '8';
    case sf::Keyboard::Scancode::Numpad9: return '9';
    case sf::Keyboard::Scancode::NumpadDivide: return '/';
    case sf::Keyboard::Scancode::NumpadDecimal: return '.';
    //
    default:
        u8 State[256]{};
        if (this->is_exclusive_mode)
        {
            // GetKeyboardState в данном случае не используем, потому что оно очень глючно работает в эксклюзивном режиме
            if (this->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::LShift}) || this->iGetAsyncKeyState(xr::key_id{sf::Keyboard::Scancode::RShift}))
                State[VK_SHIFT] = 0x80; // Для получения правильных символов при зажатом shift
        }
        else
        {
            if (!GetKeyboardState(State))
                return 0;
        }

        const auto win = xr::keyconv.dik(dik);
        u16 output{};

        if (utf)
        {
            WCHAR symbol{};
            if (this->is_exclusive_mode)
            {
                auto layout = GetKeyboardLayout(GetWindowThreadProcessId(gGameWindow, nullptr));
                if (ToUnicodeEx(MapVirtualKeyEx(win, MAPVK_VSC_TO_VK, layout), win, State, &symbol, 1, 0, layout) != 1)
                    return 0;
            }
            else
            {
                if (ToUnicode(MapVirtualKey(win, MAPVK_VSC_TO_VK), win, State, &symbol, 1, 0) != 1)
                    return 0;
            }
            WideCharToMultiByte(CP_UTF8, 0, &symbol, 1, reinterpret_cast<char*>(&output), sizeof output, nullptr, nullptr);
            return output;
        }
        else
        {
            if (this->is_exclusive_mode)
            {
                auto layout = GetKeyboardLayout(GetWindowThreadProcessId(gGameWindow, nullptr));
                if (ToAsciiEx(MapVirtualKeyEx(win, MAPVK_VSC_TO_VK, layout), win, State, &output, 0, layout) == 1)
                    return output;
            }
            else
            {
                if (ToAscii(MapVirtualKey(win, MAPVK_VSC_TO_VK), win, State, &output, 0) == 1)
                    return output;
            }
        }
    }

    return 0;
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
