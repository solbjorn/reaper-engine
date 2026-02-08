#include "stdafx.h"

#include "UICustomEdit.h"

#include "../../xr_3da/LightAnimLibrary.h"
#include "../../xr_3da/xr_input.h"

CUICustomEdit::CUICustomEdit()
{
    m_max_symb_count = u32(-1);

    m_bInputFocus = false;
    m_bHoldWaitMode = false;

    m_lines.SetVTextAlignment(valCenter);
    m_lines.SetColoringMode(false);
    m_lines.SetUseNewLineMode(false);
    SetText("");
    m_textPos.set(3, 0);
    m_bNumbersOnly = false;
    m_bFloatNumbers = false;
    m_bFocusByDbClick = false;

    m_textColor[0] = color_argb(255, 235, 219, 185);
    m_textColor[1] = color_argb(255, 100, 100, 100);
}

void CUICustomEdit::SetTextColor(u32 color) { m_textColor[0] = color; }
void CUICustomEdit::SetTextColorD(u32 color) { m_textColor[1] = color; }

void CUICustomEdit::Init(float x, float y, float width, float height)
{
    CUIWindow::Init(x, y, width, height);
    m_lines.SetWidth(width - m_textPos.x);
    m_lines.SetHeight(height - m_textPos.y);
}

void CUICustomEdit::SetLightAnim(LPCSTR lanim)
{
    if (lanim && xr_strlen(lanim))
        m_lanim = LALib.FindItem(lanim);
    else
        m_lanim = nullptr;
}

void CUICustomEdit::SetPasswordMode(bool mode) { m_lines.SetPasswordMode(mode); }
void CUICustomEdit::OnFocusLost() { CUIWindow::OnFocusLost(); }

void CUICustomEdit::SendMessage(CUIWindow*, s16 msg, void*)
{
    // кто-то другой захватил клавиатуру
    if (msg == WINDOW_KEYBOARD_CAPTURE_LOST)
    {
        m_bInputFocus = false;
        m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;
    }
}

bool CUICustomEdit::OnMouse(f32, f32, EUIMessages mouse_action)
{
    if (m_bFocusByDbClick)
    {
        if (mouse_action == WINDOW_LBUTTON_DB_CLICK && !m_bInputFocus)
        {
            GetParent()->SetKeyboardCapture(this, true);
            m_bInputFocus = true;
            m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;

            m_lines.MoveCursorToEnd();
        }
    }

    if (mouse_action == WINDOW_LBUTTON_DOWN && !m_bInputFocus)
    {
        GetParent()->SetKeyboardCapture(this, true);
        m_bInputFocus = true;
        m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;

        m_lines.MoveCursorToEnd();
    }

    return false;
}

bool CUICustomEdit::OnKeyboardHold(xr::key_id) { return true; }

bool CUICustomEdit::OnKeyboard(xr::key_id dik, EUIMessages keyboard_action)
{
    if (!m_bInputFocus)
        return false;

    if (!dik.is<sf::Keyboard::Scancode>())
        return false;

    if (keyboard_action == WINDOW_KEY_PRESSED)
    {
        m_iKeyPressAndHold = dik.get<sf::Keyboard::Scancode>();
        m_bHoldWaitMode = true;

        if (KeyPressed(m_iKeyPressAndHold))
            return true;
    }
    else if (keyboard_action == WINDOW_KEY_RELEASED)
    {
        if (m_iKeyPressAndHold == dik.get<sf::Keyboard::Scancode>())
        {
            m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;
            m_bHoldWaitMode = false;
        }

        return true;
    }

    return false;
}

bool CUICustomEdit::KeyPressed(sf::Keyboard::Scancode dik)
{
    bool bChanged = false;

    switch (dik)
    {
    case sf::Keyboard::Scancode::Tab:
        // Табы нельзя добавлять здесь, такого символа в шрифтах нет, он не отображается в игре.
        // А проблемы доставить может при получении введенного текста через GetText() в скрипте,
        // когда полученный текст будет отличаться от отображаемого из-за наличия в нём табов.
        return true;
    case sf::Keyboard::Scancode::Left: m_lines.DecCursorPos(); break;
    case sf::Keyboard::Scancode::Right: m_lines.IncCursorPos(); break;
    case sf::Keyboard::Scancode::Escape:
        if (xr_strlen(GetText()) > 0)
        {
            SetText("");
            bChanged = true;
        }
        else
        {
            GetParent()->SetKeyboardCapture(this, false);

            m_bInputFocus = false;
            m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;
        }

        break;
    case sf::Keyboard::Scancode::Enter:
    case sf::Keyboard::Scancode::NumpadEnter:
        GetParent()->SetKeyboardCapture(this, false);

        m_bInputFocus = false;
        m_iKeyPressAndHold = sf::Keyboard::Scancode::Unknown;

        GetMessageTarget()->SendMessage(this, EDIT_TEXT_COMMIT, nullptr);
        break;
    case sf::Keyboard::Scancode::Backspace:
        m_lines.DelLeftChar();

        bChanged = true;
        break;
    case sf::Keyboard::Scancode::Delete:
        m_lines.DelCurrentChar();

        bChanged = true;
        break;
    default:
        const u16 out_me = pInput->DikToChar(dik, m_lines.GetFont()->IsMultibyte());
        if (out_me)
        {
            if (!m_bNumbersOnly || (out_me >= '0' && out_me <= '9') || (m_bFloatNumbers && out_me == '.' && !strchr(m_lines.GetText(), '.')))
            {
                AddChar(out_me);
                bChanged = true;
            }
        }
    }

    if (bChanged)
        GetMessageTarget()->SendMessage(this, EDIT_TEXT_CHANGED, nullptr);

    return true;
}

void CUICustomEdit::AddChar(const u16 c)
{
    if (xr_strlen(m_lines.GetText()) >= m_max_symb_count)
        return;

    float text_width = m_lines.GetFont()->SizeOf_(m_lines.GetText()) + m_lines.GetFont()->SizeOf_(c);

    UI()->ClientToScreenScaledWidth(text_width);

    if (!m_lines.GetTextComplexMode() && text_width > m_lines.GetWidth() - m_textPos.x - 0.5f) // учитываем смещения текста по ширине
        return;

    m_lines.AddCharAtCursor(c);
    m_lines.ParseText();
    if (m_lines.GetTextComplexMode())
        if (m_lines.GetVisibleHeight() > GetHeight())
            m_lines.DelLeftChar();
}

// время для обеспечивания печатания
// символа при удерживаемой кнопке
#define HOLD_WAIT_TIME 300
#define HOLD_REPEAT_TIME 50

void CUICustomEdit::Update()
{
    if (m_bInputFocus)
    {
        static u32 last_time;

        u32 cur_time = GetTickCount();

        if (m_iKeyPressAndHold != sf::Keyboard::Scancode::Unknown)
        {
            if (m_bHoldWaitMode)
            {
                if (cur_time - last_time > HOLD_WAIT_TIME)
                {
                    m_bHoldWaitMode = false;
                    last_time = cur_time;
                }
            }
            else
            {
                if (cur_time - last_time > HOLD_REPEAT_TIME)
                {
                    last_time = cur_time;
                    std::ignore = KeyPressed(m_iKeyPressAndHold);
                }
            }
        }
        else
        {
            last_time = cur_time;
        }
    }

    m_lines.SetTextColor(m_textColor[IsEnabled() ? 0 : 1]);

    CUIWindow::Update();
}

void CUICustomEdit::Draw()
{
    CUIWindow::Draw();
    Fvector2 pos;
    GetAbsolutePos(pos);

    m_lines.m_bDrawCursor = m_bInputFocus;
    m_lines.Draw(pos.x + m_textPos.x, pos.y + m_textPos.y);
}

void CUICustomEdit::SetText(const char* str) { CUILinesOwner::SetText(str); }

const char* CUICustomEdit::GetText() { return CUILinesOwner::GetText(); }

void CUICustomEdit::Enable(bool status)
{
    CUIWindow::Enable(status);
    if (!status)
        SendMessage(this, WINDOW_KEYBOARD_CAPTURE_LOST);
}

void CUICustomEdit::SetNumbersOnly(bool status) { m_bNumbersOnly = status; }

void CUICustomEdit::SetFloatNumbers(bool status) { m_bFloatNumbers = status; }

void CUICustomEdit::SetTextPosX(float x)
{
    CUILinesOwner::SetTextPosX(x);
    m_lines.SetWidth(GetWidth() - m_textPos.x);
}

void CUICustomEdit::SetTextPosY(float y)
{
    CUILinesOwner::SetTextPosY(y);
    m_lines.SetHeight(GetHeight() - m_textPos.y);
}
