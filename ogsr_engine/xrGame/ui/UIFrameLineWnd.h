#pragma once

#include "UIFrameLine.h"
#include "UIWindow.h"
#include "UIStatic.h"

class CUIFrameLineWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIFrameLineWnd, CUIWindow);

public:
    typedef CUIWindow inherited;

    CUIFrameLineWnd();
    ~CUIFrameLineWnd() override = default;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    virtual void Init(gsl::czstring base_name, f32 x, f32 y, f32 width, f32 height, bool horizontal = true);
    virtual void InitTexture(gsl::czstring tex_name, bool horizontal = true);
    void Draw() override;
    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;
    virtual void SetOrientation(bool horizontal);
    float GetTextureHeight();
    void SetColor(u32 cl);

    // Also we can display textual caption on the frame
    CUIStatic UITitleText;
    CUIStatic* GetTitleStatic() { return &UITitleText; }

protected:
    bool bHorizontal;
    bool m_bTextureAvailable;
    CUIFrameLine UIFrameLine;
};
XR_SOL_BASE_CLASSES(CUIFrameLineWnd);
