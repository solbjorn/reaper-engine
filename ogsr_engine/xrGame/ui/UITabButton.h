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

class CUITabButton : public CUI3tButton
{
    RTTI_DECLARE_TYPEINFO(CUITabButton, CUI3tButton);

public:
    typedef CUI3tButton inherited;

    CUITabButton();
    ~CUITabButton() override;

    // behavior
    void SendMessage(CUIWindow* pWnd, s16 msg, void* = nullptr) override;
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
    void Update() override;
};
XR_SOL_BASE_CLASSES(CUITabButton);
