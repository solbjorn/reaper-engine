#pragma once

#include "uiwindow.h"

class CUICarPanel : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUICarPanel, CUIWindow);

private:
    typedef CUIWindow inherited;

    CUIStatic UIStaticCarHealth;
    CUIProgressBar UICarHealthBar;

public:
    ~CUICarPanel() override = default;

    // Установить
    void SetCarHealth(f32 value);
    void SetSpeed(f32) {}
    void SetRPM(f32) {}
    void Init(f32 x, f32 y, f32 width, f32 height) override;
};
