#pragma once

#include "UILines.h"
#include "UIWindow.h"

class CUICustomEdit : public CUIWindow, public CUILinesOwner
{
    RTTI_DECLARE_TYPEINFO(CUICustomEdit, CUIWindow, CUILinesOwner);

public:
    u32 m_max_symb_count;

    CUICustomEdit();
    ~CUICustomEdit() override = default;

    // CUILinesOwner
    virtual void SetFont(CGameFont* pFont) { CUILinesOwner::SetFont(pFont); }
    virtual CGameFont* GetFont() { return CUILinesOwner::GetFont(); }
    virtual void SetTextColor(u32 color);
    virtual void SetTextColorD(u32 color);

    virtual void Init(float x, float y, float width, float height);
    void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr) override;

    [[nodiscard]] bool OnMouse(f32, f32, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    [[nodiscard]] bool OnKeyboardHold(xr::key_id) override;
    virtual void OnFocusLost();

    virtual void Update();
    virtual void Draw();

    void CaptureFocus(bool bCapture) { m_bInputFocus = bCapture; }
    virtual void SetText(const char* str);
    virtual const char* GetText();
    void SetMaxCharCount(u32 cnt) { m_max_symb_count = cnt; }
    virtual void Enable(bool status);
    void SetNumbersOnly(bool status);
    void SetFloatNumbers(bool status);
    void SetPasswordMode(bool mode = true);
    void SetDbClickMode(bool mode = true) { m_bFocusByDbClick = mode; }
    void SetCursorColor(u32 color) { m_lines.SetCursorColor(color); }

    // Какой-то недодел
    void SetLightAnim(LPCSTR lanim);

    virtual void SetTextPosX(float x);
    virtual void SetTextPosY(float y);

protected:
    [[nodiscard]] bool KeyPressed(sf::Keyboard::Scancode dik);

    virtual void AddChar(const u16 c);

    bool m_bInputFocus;

    bool m_bNumbersOnly;
    bool m_bFloatNumbers;
    bool m_bFocusByDbClick;

    u32 m_textColor[2];

    // DIK клавиши, кот. нажата и удерживается, 0 если такой нет
    sf::Keyboard::Scancode m_iKeyPressAndHold{sf::Keyboard::Scancode::Unknown};
    bool m_bHoldWaitMode;

    //	u32	m_cursorColor;

    CLAItem* m_lanim;
};
XR_SOL_BASE_CLASSES(CUICustomEdit);
