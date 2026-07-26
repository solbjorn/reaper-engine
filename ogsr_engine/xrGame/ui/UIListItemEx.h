// File:        UIListWndEx.cpp
// Description: Extended ListItem
//              Requiered to use feature "Selected Item"
// Created:
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#pragma once

#include "UIListItem.h"

class CUIListItemEx final : public CUIListItem
{
    RTTI_DECLARE_TYPEINFO(CUIListItemEx, CUIListItem);

private:
    typedef CUIListItem inherited;

public:
    CUIListItemEx();
    ~CUIListItemEx() override;

    void SendMessage(CUIWindow*, s16 msg, void*) override;
    void SetSelectionColor(u32 dwColor);
    void Draw() override;
    void dummy() {}

protected:
    u32 m_dwSelectionColor;
};
XR_SOL_BASE_CLASSES(CUIListItemEx);
