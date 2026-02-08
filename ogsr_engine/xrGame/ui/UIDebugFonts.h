// File:		UIDebugFonts.h
// Description:	Output list of all fonts
// Created:		22.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#ifdef DEBUG
#include "UIListWnd.h"
#include "UIDialogWnd.h"

class CUIDebugFonts : public CUIDialogWnd
{
    RTTI_DECLARE_TYPEINFO(CUIDebugFonts, CUIDialogWnd);

public:
    CUIDebugFonts();
    ~CUIDebugFonts() override;

    virtual void Init(float x, float y, float width, float height);
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    void FillUpList();

protected:
    CUIStatic m_background;
};
#endif // DEBUG
