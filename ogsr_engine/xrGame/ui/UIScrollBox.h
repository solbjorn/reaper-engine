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

    [[nodiscard]] bool OnMouse(f32 x, f32 y, EUIMessages mouse_action) override;
    virtual void Draw();

protected:
    bool m_bIsHorizontal;
};
