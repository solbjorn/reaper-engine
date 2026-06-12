#pragma once

#include "uiwindow.h"

class CUI3tButton;
class CUIScrollBox;
class CUIStaticItem;

class CUIScrollBar : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIScrollBar, CUIWindow);

private:
    typedef CUIWindow inherited;

protected:
    CUI3tButton* m_DecButton;
    CUI3tButton* m_IncButton;

    CUIScrollBox* m_ScrollBox;

    CUIStaticItem* m_StaticBackground;

    int m_iScrollPos;

    int m_iStepSize;

    int m_iMinPos;
    int m_iMaxPos;

    int m_iPageSize;

    int m_ScrollWorkArea{};
    bool m_b_enabled;
    bool m_bIsHorizontal{};

    bool ScrollInc();
    bool ScrollDec();
    void UpdateScrollBar();

    u32 ScrollSize() { return _max(1, m_iMaxPos - m_iMinPos - m_iPageSize + 1); }
    void ClampByViewRect();
    void SetPosScrollFromView(float view_pos, int view_width, int view_offs);
    int PosViewFromScroll(int view_size, int view_offs);
    void SetScrollPosClamped(int iPos)
    {
        m_iScrollPos = iPos;
        clamp(m_iScrollPos, m_iMinPos, m_iMaxPos - m_iPageSize + 1);
    }

public:
    CUIScrollBar();
    ~CUIScrollBar() override;

    void SetEnabled(bool b)
    {
        m_b_enabled = b;
        if (!m_b_enabled)
            Show(m_b_enabled);
    }

    [[nodiscard]] bool GetEnabled() { return m_b_enabled; }

    void Show(bool b) override;
    void Enable(bool b) override;
    virtual void Init(f32 x, f32 y, f32 length, bool bIsHorizontal, gsl::czstring profile = "default");

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    [[nodiscard]] bool OnKeyboardHold(xr::key_id dik) override;

    void Draw() override;

    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;

    void Reset() override;
    void Refresh();
    void SetStepSize(int step);
    void SetRange(int iMin, int iMax);

    void GetRange(int& iMin, int& iMax)
    {
        iMin = m_iMinPos;
        iMax = m_iMaxPos;
    }

    int GetMaxRange() { return m_iMaxPos; }
    int GetMinRange() { return m_iMinPos; }

    void SetPageSize(int iPage)
    {
        m_iPageSize = _max(0, iPage);
        UpdateScrollBar();
    }

    int GetPageSize() { return m_iPageSize; }

    void SetScrollPos(int iPos)
    {
        SetScrollPosClamped(iPos);
        UpdateScrollBar();
    }

    int GetScrollPos() { return _max(m_iMinPos, m_iScrollPos); }

    void TryScrollInc();
    void TryScrollDec();
};
