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
    // Установить
    void SetCarHealth(float value);
    void SetSpeed(float) {}
    void SetRPM(float) {}
    void Init(float x, float y, float width, float height);
};
