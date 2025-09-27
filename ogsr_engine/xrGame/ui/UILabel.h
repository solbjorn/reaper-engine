#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"
class CLAItem;

class CUILabel : public CUIFrameLineWnd, public CUILinesOwner // IUITextControl
{
    RTTI_DECLARE_TYPEINFO(CUILabel, CUIFrameLineWnd, CUILinesOwner);

public:
    CLAItem* m_lanim{};
    float m_lainm_start_time{-1.f};

    // IUISimpleWindow
    virtual void SetWidth(float width);
    virtual void SetHeight(float height);
    virtual void SetFont(CGameFont* pFont);
    // CUIFrameLineWnd
    virtual void Init(float x, float y, float width, float height);
    virtual void Draw();
    virtual void Update();

    // own
    CUILabel();
    void SetLightAnim(LPCSTR lanim);
};
XR_SOL_BASE_CLASSES(CUILabel);
