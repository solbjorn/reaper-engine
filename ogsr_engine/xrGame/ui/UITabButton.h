// File:        UITabButton.cpp
// Description:
// Created:     19.11.2004
// Last Change: 19.11.2004
// Author:      Serhiy Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright: 2004 GSC Game World

#pragma once

#include "UI3tButton.h"
//.#include "UIFrameWindow.h"

class CUITabButton : public CUI3tButton
{
    RTTI_DECLARE_TYPEINFO(CUITabButton, CUI3tButton);

public:
    typedef CUI3tButton inherited;

    CUITabButton();
    ~CUITabButton() override;

    // behavior
    //.	virtual void AssociateWindow(CUIFrameWindow* pWindow);
    //.	virtual CUIFrameWindow* GetAssociatedWindow();
    //.	virtual void ShowAssociatedWindow(bool bShow = true);
    void SendMessage(CUIWindow* pWnd, s16 msg, void* = nullptr) override;
    [[nodiscard]] bool OnMouse(float x, float y, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnMouseDown(int mouse_btn) override;
    void Update() override;

    //.protected:
    // associated window
    //.	CUIFrameWindow* m_pAssociatedWindow;
};
XR_SOL_BASE_CLASSES(CUITabButton);
