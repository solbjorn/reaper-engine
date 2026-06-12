//////////////////////////////////////////////////////////////////////
// UIRadioButton.h: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#ifndef _UI_RADIO_BUTTON_H_
#define _UI_RADIO_BUTTON_H_

#include "UITabButton.h"

class CUIRadioButton : public CUITabButton
{
    RTTI_DECLARE_TYPEINFO(CUIRadioButton, CUITabButton);

public:
    CUIRadioButton();
    ~CUIRadioButton() override;

    void Init(f32 x, f32 y, f32 width, f32 height) override;
    void InitTexture(gsl::czstring tex_name) override;
    void SetTextX(f32) override {}
};

#endif
