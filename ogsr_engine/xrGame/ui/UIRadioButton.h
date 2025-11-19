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

    virtual void Init(float x, float y, float width, float height);
    virtual void InitTexture(LPCSTR tex_name);
    void SetTextX(float) override {}
};

#endif
