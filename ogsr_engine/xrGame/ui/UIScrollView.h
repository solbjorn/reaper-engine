#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUIScrollBar;

class CUIScrollView : public CUIWindow, public CUIWndCallback
{
    RTTI_DECLARE_TYPEINFO(CUIScrollView, CUIWindow, CUIWndCallback);

public:
    typedef CUIWindow inherited;
    friend class CUIXmlInit; // for init

protected:
    enum
    {
        eVertFlip = (1 << 0),
        eNeedRecalc = (1 << 1),
        eFixedScrollBar = (1 << 2),
        eItemsSelectabe = (1 << 3),
        eInverseDir = (1 << 4) /*,eMultiSelect=(1<<5)*/
    };
    CUIScrollBar* m_VScrollBar;
    CUIWindow* m_pad;

    float m_targetScrollPosition{};
    float m_rightIndent;
    float m_leftIndent;
    float m_upIndent;
    float m_downIndent;

    float m_vertInterval;
    u32 m_text_color;

    Flags16 m_flags;
    shared_str m_scrollbar_profile;

    virtual void RecalcSize();
    void UpdateScroll();
    void OnScrollV(CUIWindow*, void*);
    void SetRightIndention(float val);
    void SetLeftIndention(float val);
    void SetUpIndention(float val);
    void SetDownIndention(float val);
    void SetVertFlip(bool val) { m_flags.set(eVertFlip, val); }

public:
    using CUIWindow::Init;

    CUIScrollView();
    virtual ~CUIScrollView();
    void Init(); // need parent to be initialized
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
    virtual void Draw();
    virtual void Update();
    void AddWindow(CUIWindow* pWnd, bool auto_delete, bool top = false);
    void RemoveWindow(CUIWindow* pWnd);
    void Clear(bool scrollToTop = true);
    void ScrollToBegin();
    void ScrollToEnd();
    bool GetVertFlip() { return !!m_flags.test(eVertFlip); }
    u32 GetSize();
    CUIWindow* GetItem(u32 idx);
    void SetFixedScrollBar(bool b);
    float GetDesiredChildWidth();
    virtual void SetSelected(CUIWindow*);
    CUIWindow* GetSelected();
    void ForceUpdate();
    void ForceScrollPosition();
    int GetMinScrollPos();
    int GetMaxScrollPos();
    int GetCurrentScrollPos();
    virtual void SetWndSize(const Fvector2& size) override;
    virtual void SetHeight(float height) override;
    void SetScrollPos(int position);
    void SetScrollBarProfile(LPCSTR profile);
    IC bool NeedShowScrollBar(); // no comment
    float GetHorizIndent(); // left + right indent
    float GetVertIndent(); // top + bottom indent
    void UpdateChildrenLenght(); // set default width for all children
    float Scroll2ViewV(); // calculate scale for scroll position
    CUIScrollBar* ScrollBar() { return m_VScrollBar; }
    u32 GetTextColor() { return m_text_color; }
    virtual void SetTextColor(u32 color) { m_text_color = color; }
};
XR_SOL_BASE_CLASSES(CUIScrollView);
