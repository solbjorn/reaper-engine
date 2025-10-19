#pragma once

#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;
class CUIXml;
class CSE_ALifeTraderAbstract;

class CUIStalkersRankingWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIStalkersRankingWnd, CUIWindow);

public:
    typedef CUIWindow inherited;

    void Init();
    virtual void Show(bool status);
    void ShowHumanDetails();

protected:
    CUIFrameWindow* UIInfoFrame;
    CUIFrameWindow* UICharIconFrame;
    CUIFrameLineWnd* UIInfoHeader;
    CUIFrameLineWnd* UICharIconHeader;
    CUIAnimatedStatic* UIAnimatedIcon;
    // информация о персонаже
    CUIWindow* UICharacterWindow;
    CUICharacterInfo* UICharacterInfo;
    void FillList();
    CUIScrollView* UIList;
    void AddStalkerItem(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);
    void AddActorItem(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);

public:
    CUIScrollView& GetTopList() { return *UIList; }
    void ShowHumanInfo(u16 id);
    virtual void Reset();
};

class CUIStalkerRankingInfoItem : public CUIWindow, public CUISelectable
{
    RTTI_DECLARE_TYPEINFO(CUIStalkerRankingInfoItem, CUIWindow, CUISelectable);

public:
    CUIStalkersRankingWnd* m_StalkersRankingWnd;
    u32 m_stored_alpha;

    u16 m_humanID;
    CUIStatic* m_text1;
    CUIStatic* m_text2;
    CUIStatic* m_text3;

    CUIStalkerRankingInfoItem(CUIStalkersRankingWnd*);

    void Init(CUIXml* xml, LPCSTR path, int idx);
    virtual void SetSelected(bool b);
    virtual bool OnMouseDown(int mouse_btn);
};

class CUIStalkerRankingElipsisItem : public CUIStalkerRankingInfoItem
{
    RTTI_DECLARE_TYPEINFO(CUIStalkerRankingElipsisItem, CUIStalkerRankingInfoItem);

public:
    typedef CUIStalkerRankingInfoItem inherited;

    CUIStalkerRankingElipsisItem(CUIStalkersRankingWnd*);

    virtual void SetSelected(bool);
    virtual bool OnMouseDown(int);
};

void add_human_to_top_list(u16 id);
int get_actor_ranking();
void remove_human_from_top_list(u16 id);
