
#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"
#include "../encyclopedia_article_defs.h"
class CUINewsWnd;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUIAnimatedStatic;
class CUIStatic;
class CUITabControl;
class CUIScrollView;
class CUIListWnd;
class CEncyclopediaArticle;
// class CUIVideoPlayerWnd;

class CUIDiaryWnd : public CUIWindow, public CUIWndCallback
{
    RTTI_DECLARE_TYPEINFO(CUIDiaryWnd, CUIWindow, CUIWndCallback);

public:
    typedef CUIWindow inherited;
    enum EDiaryFilter
    {
        eJournal = 0,
        eNews,
        eNone
    };

protected:
    EDiaryFilter m_currFilter;
    u32 prevArticlesCount;

    CUINewsWnd* m_UINewsWnd;

    CUIWindow* m_UILeftWnd;
    CUIWindow* m_UIRightWnd;
    CUIFrameWindow* m_UILeftFrame;
    CUIFrameLineWnd* m_UILeftHeader;
    CUIFrameWindow* m_UIRightFrame;
    CUIFrameLineWnd* m_UIRightHeader;
    CUIAnimatedStatic* m_UIAnimation;
    CUITabControl* m_FilterTab;
    CUIListWnd* m_SrcListWnd;
    CUIScrollView* m_DescrView;
    CGameFont* m_pTreeRootFont;
    u32 m_uTreeRootColor;
    CGameFont* m_pTreeItemFont;
    u32 m_uTreeItemColor;

    xr_vector<Fvector2> m_sign_places;
    CUIStatic* m_updatedSectionImage;
    CUIStatic* m_oldSectionImage;

    xr_vector<CEncyclopediaArticle> m_ArticlesDB;

    void OnFilterChanged(CUIWindow*, void*);
    void OnSrcListItemClicked(CUIWindow*, void*);
    void UnloadJournalTab();
    void LoadJournalTab();
    void UnloadNewsTab();
    void LoadNewsTab();
    void Reload(EDiaryFilter new_filter);

public:
    CUIDiaryWnd();
    virtual ~CUIDiaryWnd();

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
    virtual void Draw();
    virtual void Reset();

    void Init();
    void AddNews();
    void MarkNewsAsRead(bool status);
    virtual void Show(bool status);

    void FillNews();
    void ReloadJournal();
    void ResetJournal();
    void UpdateJournal();
};
