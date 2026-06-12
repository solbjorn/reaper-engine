//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIWindow.h"

class CUIFrameWindow;
class CUICharacterInfo;
class CInventoryOwner;

class CUIPdaListItem : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIPdaListItem, CUIWindow);

private:
    typedef CUIWindow inherited;

public:
    CUIPdaListItem();
    ~CUIPdaListItem() override;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    virtual void InitCharacter(CInventoryOwner* pInvOwner);

    void* m_data{};

protected:
    // информация о персонаже
    CUIFrameWindow* UIMask{};
    CUICharacterInfo* UIInfo{};
};
