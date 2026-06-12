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
    void SetFont(CGameFont* pFont) override { CUILinesOwner::SetFont(pFont); }
    [[nodiscard]] CGameFont* GetFont() override { return CUILinesOwner::GetFont(); }
    void SetTextColor(u32 color) override;
    virtual void SetTextColorD(u32 color);

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr) override;

    [[nodiscard]] bool OnMouse(f32, f32, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    [[nodiscard]] bool OnKeyboardHold(xr::key_id) override;
    void OnFocusLost() override;

    void Update() override;
    void Draw() override;

    void CaptureFocus(bool bCapture) { m_bInputFocus = bCapture; }
    void SetText(gsl::czstring str) override;
    [[nodiscard]] gsl::czstring GetText() override;
    void SetMaxCharCount(u32 cnt) { m_max_symb_count = cnt; }
    void Enable(bool status) override;
    void SetNumbersOnly(bool status);
    void SetFloatNumbers(bool status);
    void SetPasswordMode(bool mode = true);
    void SetDbClickMode(bool mode = true) { m_bFocusByDbClick = mode; }
    void SetCursorColor(u32 color) { m_lines.SetCursorColor(color); }

    // Какой-то недодел
    void SetLightAnim(gsl::czstring lanim);

    void SetTextPosX(f32 x) override;
    void SetTextPosY(f32 y) override;

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
