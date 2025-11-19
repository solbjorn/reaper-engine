#pragma once

#include "uibutton.h"

class CUIScrollBox : public CUIStatic
{
    RTTI_DECLARE_TYPEINFO(CUIScrollBox, CUIStatic);

private:
    typedef CUIStatic inherited;

public:
    CUIScrollBox();
    ~CUIScrollBox() override = default;

    void SetHorizontal();
    void SetVertical();

    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
    virtual void Draw();

protected:
    bool m_bIsHorizontal;
};
