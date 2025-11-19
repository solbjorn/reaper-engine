#pragma once

#include "UIWindow.h"

class CUIStatic;
class CUIXml;
struct GAME_NEWS_DATA;

class CUINewsItemWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUINewsItemWnd, CUIWindow);

private:
    typedef CUIWindow inherited;
    CUIStatic* m_UIText;
    CUIStatic* m_UITextDate;
    CUIStatic* m_UIImage;

public:
    CUINewsItemWnd();
    ~CUINewsItemWnd() override;

    void Init(CUIXml& uiXml, LPCSTR start_from);
    void Setup(GAME_NEWS_DATA& news_data);
};
