#pragma once

#include "UIWindow.h"

class CUIStatic;
class CUIFrameLineWnd;

class CUIButtonHint : public CUIWindow, public pureRender
{
    RTTI_DECLARE_TYPEINFO(CUIButtonHint, CUIWindow, pureRender);

public:
    CUIWindow* m_ownerWnd{};

    CUIStatic* m_text;
    CUIFrameLineWnd* m_border;
    bool m_enabledOnFrame{};

    CUIButtonHint();
    ~CUIButtonHint() override;

    CUIWindow* Owner() { return m_ownerWnd; }
    void Discard() { m_ownerWnd = nullptr; }
    tmc::task<void> OnRender() override;
    void Draw_() { m_enabledOnFrame = true; }
    void SetHintText(CUIWindow* w, LPCSTR text);
};

extern CUIButtonHint* g_btnHint;
