#pragma once

#include "uiwindow.h"

#include "../uiframerect.h"

class CUIStatic;

class CUIFrameWindow : public CUIWindow, public CUIMultiTextureOwner
{
    RTTI_DECLARE_TYPEINFO(CUIFrameWindow, CUIWindow, CUIMultiTextureOwner);

private:
    typedef CUIWindow inherited;

public:
    using CUIWindow::Draw;

    CUIFrameWindow();
    ~CUIFrameWindow() override = default;

    virtual void Init(gsl::czstring base_name, f32 x, f32 y, f32 width, f32 height);
    void Init(f32 x, f32 y, f32 width, f32 height) override;
    virtual void Init(gsl::czstring base_name, Frect* pRect);

    void InitTexture(gsl::czstring texture) override;
    void SetTextureColor(u32 color) { m_UIWndFrame.SetTextureColor(color); }

    void SetWidth(f32 width) override;
    void SetHeight(f32 height) override;

    void SetColor(u32 cl);

    void Draw() override;
    void Update() override;

    // текст заголовка
    CUIStatic* UITitleText;
    CUIStatic* GetTitleStatic() { return UITitleText; }
    void SetVisiblePart(CUIFrameRect::EFramePart p, BOOL b) { m_UIWndFrame.SetVisiblePart(p, b); }

protected:
    CUIFrameRect m_UIWndFrame;

    void FrameClip(const Frect parentAbsR);

private:
    inline void ClampMax_Zero(Frect& r);
};
XR_SOL_BASE_CLASSES(CUIFrameWindow);
