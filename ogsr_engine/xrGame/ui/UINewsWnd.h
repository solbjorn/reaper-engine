#pragma once

#include "UIWindow.h"
#include "xrUIXmlParser.h"

class CUIScrollView;
struct GAME_NEWS_DATA;

class CUINewsWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUINewsWnd, CUIWindow);

private:
    typedef CUIWindow inherited;
    enum eFlag
    {
        eNeedAdd = (1 << 0),
    };
    Flags16 m_flags;
    CUIXml uiXml;

public:
    CUINewsWnd();
    ~CUINewsWnd() override;

    void Init();
    void Init(LPCSTR xml_name, LPCSTR start_from);
    void AddNews();
    void LoadNews();
    virtual void Show(bool status);
    virtual void Update();
    virtual void Reset();

    CUIScrollView* UIScrollWnd;

private:
    void AddNewsItem(GAME_NEWS_DATA& news_data, bool top = false);
};
