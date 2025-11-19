#pragma once

#include "UIWindow.h"

class CUIStatic;

class CUIProgressShape : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIProgressShape, CUIWindow);

private:
    friend class CUIXmlInit;

public:
    CUIProgressShape();
    ~CUIProgressShape() override;

    void SetPos(int pos, int max);
    void SetPos(float pos);
    void SetTextVisible(bool b);

    virtual void Draw();

protected:
    bool m_bClockwise{};
    u32 m_sectorCount{};
    float m_stage{};
    CUIStatic* m_pTexture;
    CUIStatic* m_pBackground;
    bool m_bText{};
    bool m_blend{};

    float m_angle_begin{};
    float m_angle_end{PI_MUL_2};
};
