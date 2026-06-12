#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"

class CLAItem;

class CUILabel : public CUIFrameLineWnd, public CUILinesOwner // IUITextControl
{
    RTTI_DECLARE_TYPEINFO(CUILabel, CUIFrameLineWnd, CUILinesOwner);

public:
    CLAItem* m_lanim{nullptr};
    float m_lainm_start_time{-1.0f};

    // IUISimpleWindow
    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;
    void SetFont(CGameFont* pFont) override;
    // CUIFrameLineWnd
    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void Draw() override;
    void Update() override;

    // own
    CUILabel();
    ~CUILabel() override = default;

    void SetLightAnim(gsl::czstring lanim);
};
XR_SOL_BASE_CLASSES(CUILabel);
