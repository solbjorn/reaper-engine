//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.h: элемент окна списка в основном
// экране для сообщений PDA
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "..\InventoryOwner.h"

class CUIPdaMsgListItem : public CUIStatic
{
    RTTI_DECLARE_TYPEINFO(CUIPdaMsgListItem, CUIStatic);

public:
    ~CUIPdaMsgListItem() override = default;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    virtual void InitCharacter(CInventoryOwner* pInvOwner);
    void SetTextColor(u32 color) override;
    void SetFont(CGameFont* pFont) override;
    void SetColor(u32 color) override;

    // информация о персонаже
    CUIStatic UIIcon;
    CUIStatic UIName;
    CUIStatic UIMsgText;
};
