#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class IInputReceiver;

#ifndef MOUSE_1
#define MOUSE_1 MOUSE_1

// 0xed - max vavue in DIK* enum
constexpr inline gsl::index MOUSE_1{0xed + 100};
constexpr inline gsl::index MOUSE_2{0xed + 101};
constexpr inline gsl::index MOUSE_3{0xed + 102};

constexpr inline gsl::index MOUSE_4{0xed + 103};
constexpr inline gsl::index MOUSE_5{0xed + 104};
constexpr inline gsl::index MOUSE_6{0xed + 105};
constexpr inline gsl::index MOUSE_7{0xed + 106};
constexpr inline gsl::index MOUSE_8{0xed + 107};
#endif

constexpr inline std::array<gsl::index, 8> mouse_button_2_key{MOUSE_1, MOUSE_2, MOUSE_3, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7, MOUSE_8};

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
        COUNT_MOUSE_BUTTONS = 3,
        COUNT_MOUSE_AXIS = 3,
        COUNT_KB_BUTTONS = 256
    };
    struct sxr_mouse
    {
        DIDEVCAPS capabilities;
        DIDEVICEINSTANCE deviceInfo;
        DIDEVICEOBJECTINSTANCE objectInfo;
        u32 mouse_dt;
    };
    struct sxr_key
    {
        DIDEVCAPS capabilities;
        DIDEVICEINSTANCE deviceInfo;
        DIDEVICEOBJECTINSTANCE objectInfo;
    };

private:
    LPDIRECTINPUT8 pDI{}; // The DInput object
    LPDIRECTINPUTDEVICE8 pMouse{}; // The DIDevice7 interface
    LPDIRECTINPUTDEVICE8 pKeyboard{}; // The DIDevice7 interface

    u32 timeStamp[COUNT_MOUSE_AXIS]{};
    u32 timeSave[COUNT_MOUSE_AXIS]{};
    int offs[COUNT_MOUSE_AXIS]{};
    BOOL mouseState[COUNT_MOUSE_BUTTONS]{};
    uint8_t KBState[COUNT_KB_BUTTONS]{};

    HRESULT CreateInputDevice(LPDIRECTINPUTDEVICE8* device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, u32 buf_size);

    //	xr_stack<IInputReceiver*>	cbStack;
    xr_vector<IInputReceiver*> cbStack;

    void MouseUpdate();
    void KeyUpdate();
    bool is_exclusive_mode;

public:
    sxr_mouse mouse_property;
    sxr_key key_property;
    u32 dwCurTime;

    void Attach();
    void SetAllAcquire(BOOL bAcquire = TRUE);
    void SetMouseAcquire(BOOL bAcquire);
    void SetKBDAcquire(BOOL bAcquire);

    void iCapture(IInputReceiver* pc);
    void iRelease(IInputReceiver* pc);
    BOOL iGetAsyncKeyState(int dik);
    BOOL iGetAsyncBtnState(int btn);
    void iGetLastMouseDelta(Ivector2& p) { p.set(offs[0], offs[1]); }

    explicit CInput(bool bExclusive = true, int deviceForInit = default_key);
    ~CInput() override;

    [[nodiscard]] tmc::task<void> OnFrame() override;
    [[nodiscard]] tmc::task<void> OnAppActivate() override;
    [[nodiscard]] tmc::task<void> OnAppDeactivate() override;

    IInputReceiver* CurrentIR();

    void exclusive_mode(const bool exclusive);
    bool exclusive_mode() const { return is_exclusive_mode; }

public:
    bool get_dik_name(int dik, LPSTR dest, int dest_sz);

    // Возвращает символ по коду клавиши. Учитывается переключение языка, зажатый shift и caps lock
    // ( caps lock учитывается только в неэксклюзивном режиме, из-за его особенностей )
    // В случае неудачи функция возвращает 0.
    u16 DikToChar(const int dik, const bool utf);

    void clip_cursor(bool clip);
};

extern CInput* pInput;
