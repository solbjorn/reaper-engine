#pragma once

#include "UIWindow.h"

class CUIStatic;
class CEncyclopediaArticle;

class CUIEncyclopediaArticleWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIEncyclopediaArticleWnd, CUIWindow);

private:
    typedef CUIWindow inherited;

    CUIStatic* m_UIImage;
    CUIStatic* m_UIText;

protected:
    void AdjustLauout();

public:
    CUIEncyclopediaArticleWnd();
    ~CUIEncyclopediaArticleWnd() override;

    void Init(LPCSTR xml_name, LPCSTR start_from);
    void SetArticle(CEncyclopediaArticle*);
    void SetArticle(LPCSTR);
};
