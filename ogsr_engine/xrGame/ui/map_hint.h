#pragma once

#include "UIWindow.h"

class CUIStatic;
class CUIFrameWindow;

class CUIMapHint : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIMapHint, CUIWindow);

private:
    typedef CUIWindow inherited;
    CUIStatic* m_text;
    CUIFrameWindow* m_border;
    CUIWindow* m_owner{};

public:
    CUIMapHint();
    ~CUIMapHint() override;

    void Init();
    void SetText(LPCSTR text);
    virtual void Draw() {}
    void Draw_();
    void SetOwner(CUIWindow* w) { m_owner = w; }
    CUIWindow* GetOwner() { return m_owner; }
};
