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
    using inherited = CUIWindow;

    ~CUIStalkersRankingWnd() override = default;

    void Init();
    void Show(bool status) override;
    void ShowHumanDetails();

protected:
    CUIFrameWindow* UIInfoFrame{};
    CUIFrameWindow* UICharIconFrame{};
    CUIFrameLineWnd* UIInfoHeader{};
    CUIFrameLineWnd* UICharIconHeader{};
    CUIAnimatedStatic* UIAnimatedIcon{};
    // информация о персонаже
    CUIWindow* UICharacterWindow{};
    CUICharacterInfo* UICharacterInfo{};
    CUIScrollView* UIList{};

    void FillList();
    void AddStalkerItem(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);
    void AddActorItem(CUIXml* xml, int num, CSE_ALifeTraderAbstract* t);

public:
    [[nodiscard]] CUIScrollView& GetTopList() { return *UIList; }
    void ShowHumanInfo(u16 id);
    void Reset() override;
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

    explicit CUIStalkerRankingInfoItem(CUIStalkersRankingWnd*);
    ~CUIStalkerRankingInfoItem() override = default;

    void Init(CUIXml* xml, LPCSTR path, int idx);
    void SetSelected(bool b) override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button mouse_btn) override;
};

class CUIStalkerRankingElipsisItem : public CUIStalkerRankingInfoItem
{
    RTTI_DECLARE_TYPEINFO(CUIStalkerRankingElipsisItem, CUIStalkerRankingInfoItem);

public:
    typedef CUIStalkerRankingInfoItem inherited;

    explicit CUIStalkerRankingElipsisItem(CUIStalkersRankingWnd*);
    ~CUIStalkerRankingElipsisItem() override = default;

    void SetSelected(bool) override;
    [[nodiscard]] bool OnMouseDown(sf::Mouse::Button) override;
};

void add_human_to_top_list(u16 id);
int get_actor_ranking();
void remove_human_from_top_list(u16 id);
