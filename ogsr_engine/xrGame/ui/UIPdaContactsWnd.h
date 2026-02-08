#pragma once

#include "UIPdaListItem.h"
#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIStatic;
class CUIAnimatedStatic;
class CUIScrollView;
class CPda;

class CUIPdaContactsWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIPdaContactsWnd, CUIWindow);

private:
    typedef CUIWindow inherited;
    enum
    {
        flNeedUpdate = (1 << 0),
    };
    Flags8 m_flags;

public:
    CUIPdaContactsWnd();
    ~CUIPdaContactsWnd() override;

    void Init();

    virtual void Update();
    virtual void Reset();

    virtual void Show(bool status);

    void AddContact(CPda* pda, u16 owner_id);
    void RemoveContact(CPda* pda);
    void RemoveAll();
    void Reload();

    CUIScrollView* UIListWnd;
    CUIScrollView* UIDetailsWnd;

protected:
    CUIFrameWindow* UIFrameContacts;
    CUIFrameLineWnd* UIContactsHeader;
    CUIFrameWindow* UIRightFrame;
    CUIFrameLineWnd* UIRightFrameHeader;
    CUIAnimatedStatic* UIAnimation;
};

class CUIPdaContactItem : public CUIPdaListItem, public CUISelectable
{
    RTTI_DECLARE_TYPEINFO(CUIPdaContactItem, CUIPdaListItem, CUISelectable);

public:
    CUIPdaContactsWnd* m_cw;

public:
    explicit CUIPdaContactItem(CUIPdaContactsWnd* cw) : m_cw{cw} {}
    ~CUIPdaContactItem() override;

    virtual void SetSelected(bool b);
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
};
