#pragma once

#include "IInputReceiver.h"

struct _DIDATAFORMAT;
struct IDirectInput8W;
struct IDirectInputDevice8W;

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//описание класса

constexpr inline int mouse_device_key{1};
constexpr inline int keyboard_device_key{2};
constexpr inline int all_device_key{mouse_device_key | keyboard_device_key};
constexpr inline int default_key{mouse_device_key | keyboard_device_key};

class CInput : public pureFrame, public pureAppActivate, public pureAppDeactivate
{
    RTTI_DECLARE_TYPEINFO(CInput, pureFrame, pureAppActivate, pureAppDeactivate);

public:
    enum
    {
        COUNT_MOUSE_AXIS = 3,
    };

private:
    IDirectInput8W* pDI{}; // The DInput object
    IDirectInputDevice8W* pMouse{}; // The DIDevice7 interface
    IDirectInputDevice8W* pKeyboard{}; // The DIDevice7 interface

    u32 timeStamp[COUNT_MOUSE_AXIS]{};
    u32 timeSave[COUNT_MOUSE_AXIS]{};
    int offs[COUNT_MOUSE_AXIS]{};
    xr::bitset<sf::Mouse::ButtonCount> mouseState;
    xr::bitset<sf::Keyboard::ScancodeCount> KBState;

    HRESULT CreateInputDevice(IDirectInputDevice8W** device, GUID guidDevice, const _DIDATAFORMAT* pdidDataFormat, u32 buf_size);

    //	xr_stack<IInputReceiver*>	cbStack;
    xr_vector<IInputReceiver*> cbStack;

    tmc::task<void> MouseUpdate();
    tmc::task<void> RecheckMouseButtons(std::array<u8, 8> state, bool editor);
    tmc::task<void> isButtonsOnHold(xr::bitset<sf::Mouse::ButtonCount> mouse_prev, bool editor);
    tmc::task<void> KeyUpdate();
    bool is_exclusive_mode;

    u32 mouse_dt{25};

public:
    u32 dwCurTime;

private:
    explicit CInput(bool exclusive);
    tmc::task<void> co_CInput(u32 device);

public:
    static tmc::task<void> co_create(bool exclusive = true, u32 device = default_key);
    ~CInput() override;

    void Attach();
    void SetAllAcquire(BOOL bAcquire = TRUE);
    void SetMouseAcquire(BOOL bAcquire);
    void SetKBDAcquire(BOOL bAcquire);

    tmc::task<void> iCapture(IInputReceiver* pc);
    tmc::task<void> iRelease(IInputReceiver* pc);
    [[nodiscard]] bool iGetAsyncKeyState(xr::key_id dik) const;
    void iGetLastMouseDelta(Ivector2& p) { p.set(offs[0], offs[1]); }

    tmc::task<void> OnFrame() override;
    tmc::task<void> OnAppActivate() override;
    tmc::task<void> OnAppDeactivate() override;

    [[nodiscard]] IInputReceiver* CurrentIR() const;

    void exclusive_mode(const bool exclusive);
    bool exclusive_mode() const { return is_exclusive_mode; }

    // Возвращает символ по коду клавиши. Учитывается переключение языка, зажатый shift и caps lock
    // ( caps lock учитывается только в неэксклюзивном режиме, из-за его особенностей )
    // В случае неудачи функция возвращает 0.
    [[nodiscard]] u16 DikToChar(sf::Keyboard::Scancode dik, bool utf) const;

    void clip_cursor(bool clip);
};

extern CInput* pInput;
