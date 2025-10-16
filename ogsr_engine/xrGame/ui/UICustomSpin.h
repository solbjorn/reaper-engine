// file:		CCustomSpin.h
// description:	base class for CSpinNum & CSpinText
// created:		15.06.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once

#include "UIWindow.h"
#include "UIOptionsItem.h"

class CUI3tButton;
class CUIFrameLineWnd;
class CUILines;

class XR_NOVTABLE CUICustomSpin : public CUIWindow, public CUIOptionsItem
{
    RTTI_DECLARE_TYPEINFO(CUICustomSpin, CUIWindow, CUIOptionsItem);

public:
    CUICustomSpin();
    virtual ~CUICustomSpin();

    // CUIWindow
    void Init(float x, float y, float width, float) override;
    void SendMessage(CUIWindow* pWnd, s16 msg, void* = nullptr) override;
    virtual void Draw();
    virtual void Update();
    virtual void Enable(bool status);

    // own
    virtual void OnBtnUpClick();
    virtual void OnBtnDownClick();
    LPCSTR GetText();

    void SetTextColor(u32 color);
    void SetTextColorD(u32 color);

protected:
    virtual bool CanPressUp() = 0;
    virtual bool CanPressDown() = 0;
    virtual void IncVal() = 0;
    virtual void DecVal() = 0;

    CUIFrameLineWnd* m_pFrameLine;
    CUI3tButton* m_pBtnUp;
    CUI3tButton* m_pBtnDown;
    CUILines* m_pLines;

    u32 m_time_begin;
    u32 m_p_delay;
    u32 m_u_delay;

    u32 m_textColor[2];
};
XR_SOL_BASE_CLASSES(CUICustomSpin);
