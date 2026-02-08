// UIDialogWnd.cpp: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "uidialogwnd.h"

#include "../hudmanager.h"
#include "../xr_level_controller.h"
#include "..\..\xr_3da\xr_ioconsole.h"
#include "../level.h"
#include "../GameObject.h"

CUIDialogWnd::CUIDialogWnd() { Hide(); }
CUIDialogWnd::~CUIDialogWnd() = default;

void CUIDialogWnd::Show()
{
    Fvector2 pos = GetUICursor()->GetCursorPosition();
    GetUICursor()->SetUICursorPosition(pos);

    inherited::Enable(true);
    inherited::Show(true);

    ResetAll();
}

void CUIDialogWnd::Hide()
{
    inherited::Enable(false);
    inherited::Show(false);
}

tmc::task<bool> CUIDialogWnd::IR_OnKeyboardHold(xr::key_id dik)
{
    if (!IR_process())
        co_return false;

    if (OnKeyboardHold(dik))
        co_return true;

    if (!StopAnyMove() && g_pGameLevel)
    {
        CObject* O = Level().CurrentEntity();
        if (O)
        {
            IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
            if (!IR)
                co_return false;

            co_await IR->IR_OnKeyboardHold(dik);
        }
    }

    co_return false;
}

#define DOUBLE_CLICK_TIME 250

tmc::task<bool> CUIDialogWnd::IR_OnKeyboardPress(xr::key_id dik)
{
    if (!IR_process())
        co_return false;

    // mouse click
    if (dik.is<sf::Mouse::Button>())
    {
        Fvector2 cp = GetUICursor()->GetCursorPosition();
        EUIMessages action;

        switch (dik.get<sf::Mouse::Button>())
        {
        case sf::Mouse::Button::Left: action = WINDOW_LBUTTON_DOWN; break;
        case sf::Mouse::Button::Right: action = WINDOW_RBUTTON_DOWN; break;
        case sf::Mouse::Button::Middle: action = WINDOW_CBUTTON_DOWN; break;
        case sf::Mouse::Button::Extra1: action = WINDOW_EBUTTON_DOWN; break;
        case sf::Mouse::Button::Extra2: action = WINDOW_XBUTTON_DOWN; break;
        }

        if (action == WINDOW_LBUTTON_DOWN)
        {
            u32 dwCurTime = Device.dwTimeContinual;

            if (dwCurTime - m_dwLastClickTime < DOUBLE_CLICK_TIME)
                action = WINDOW_LBUTTON_DB_CLICK;

            m_dwLastClickTime = dwCurTime;
        }

        if (OnMouse(cp.x, cp.y, action))
            co_return true;
    }

    if (OnKeyboard(dik, WINDOW_KEY_PRESSED))
        co_return true;

    if (!StopAnyMove() && g_pGameLevel)
    {
        CObject* O = Level().CurrentEntity();
        if (O)
        {
            IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
            if (!IR)
                co_return false;

            co_await IR->IR_OnKeyboardPress(dik);
        }
    }

    co_return false;
}

bool CUIDialogWnd::IR_OnKeyboardRelease(xr::key_id dik)
{
    if (!IR_process())
        return false;

    // mouse click
    if (dik.is<sf::Mouse::Button>())
    {
        Fvector2 cp = GetUICursor()->GetCursorPosition();
        EUIMessages action;

        switch (dik.get<sf::Mouse::Button>())
        {
        case sf::Mouse::Button::Left: action = WINDOW_LBUTTON_UP; break;
        case sf::Mouse::Button::Right: action = WINDOW_RBUTTON_UP; break;
        case sf::Mouse::Button::Middle: action = WINDOW_CBUTTON_UP; break;
        case sf::Mouse::Button::Extra1: action = WINDOW_EBUTTON_UP; break;
        case sf::Mouse::Button::Extra2: action = WINDOW_XBUTTON_UP; break;
        }

        if (OnMouse(cp.x, cp.y, action))
            return true;
    }

    if (OnKeyboard(dik, WINDOW_KEY_RELEASED))
        return true;

    if (!StopAnyMove() && g_pGameLevel)
    {
        CObject* O = Level().CurrentEntity();
        if (O)
        {
            IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
            if (!IR)
                return (false);

            IR->IR_OnKeyboardRelease(dik);
        }
    }
    return false;
}

tmc::task<bool> CUIDialogWnd::IR_OnMouseWheel(gsl::index direction)
{
    if (!IR_process())
        co_return false;

    Fvector2 pos = GetUICursor()->GetCursorPosition();

    if (direction > 0)
        std::ignore = OnMouse(pos.x, pos.y, WINDOW_MOUSE_WHEEL_UP);
    else
        std::ignore = OnMouse(pos.x, pos.y, WINDOW_MOUSE_WHEEL_DOWN);

    co_return true;
}

bool CUIDialogWnd::IR_OnMouseMove(int dx, int dy)
{
    if (!IR_process())
        return false;

    if (GetUICursor()->IsVisible())
    {
        GetUICursor()->UpdateCursorPosition(dx, dy);
        Fvector2 cPos = GetUICursor()->GetCursorPosition();

        std::ignore = OnMouse(cPos.x, cPos.y, WINDOW_MOUSE_MOVE);
    }
    else if (!StopAnyMove() && g_pGameLevel)
    {
        CObject* O = Level().CurrentEntity();
        if (O)
        {
            IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(O));
            if (!IR)
                return (false);

            IR->IR_OnMouseMove(dx, dy);
        }
    }

    return true;
}

bool CUIDialogWnd::OnKeyboardHold(xr::key_id dik)
{
    if (!IR_process())
        return false;

    return inherited::OnKeyboardHold(dik);
}

bool CUIDialogWnd::OnKeyboard(xr::key_id dik, EUIMessages keyboard_action)
{
    if (!IR_process())
        return false;

    if (inherited::OnKeyboard(dik, keyboard_action))
        return true;

    return false;
}

bool CUIDialogWnd::IR_process()
{
    if (!IsEnabled())
        return false;

    if (Device.Paused() && !WorkInPause())
        return false;

    return true;
}

void CUIDialogWnd::Update() { CUIWindow::Update(); }
