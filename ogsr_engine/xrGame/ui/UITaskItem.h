#pragma once

#include "UIDialogWnd.h"
#include "UIListItem.h"
#include "UIWndCallback.h"

class CGameTask;
class CUI3tButton;
class CUIStatic;
class CUIButton;
class SGameTaskObjective;
class CUIEventsWnd;
class CUIEditBoxEx;
class CUIEditBox;

class CUITaskItem : public CUIListItem, public CUIWndCallback
{
    RTTI_DECLARE_TYPEINFO(CUITaskItem, CUIListItem, CUIWndCallback);

private:
    using inherited = CUIListItem;

protected:
    CGameTask* m_GameTask{};
    u16 m_TaskObjectiveIdx{std::numeric_limits<u16>::max()};

    void OnItemClicked(CUIWindow*, void*);
    void Init();

public:
    explicit CUITaskItem(CUIEventsWnd* w);
    ~CUITaskItem() override;

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

    virtual void SetGameTask(CGameTask* gt, u16 obj_idx);

    CGameTask* GameTask() { return m_GameTask; }
    u16 ObjectiveIdx() { return m_TaskObjectiveIdx; }
    SGameTaskObjective* Objective();

    CUIEventsWnd* m_EventsWnd;
};

class CUITaskRootItem : public CUITaskItem
{
    RTTI_DECLARE_TYPEINFO(CUITaskRootItem, CUITaskItem);

private:
    using inherited = CUITaskItem;

protected:
    CUIStatic* m_taskImage;
    CUIStatic* m_captionStatic;
    CUIStatic* m_captionTime;
    CUIStatic* m_remTimeStatic;
    CUI3tButton* m_switchDescriptionBtn;
    bool m_curr_descr_mode;
    void Init();

public:
    explicit CUITaskRootItem(CUIEventsWnd* w);
    ~CUITaskRootItem() override;

    virtual void Update();
    virtual void SetGameTask(CGameTask* gt, u16 obj_idx);
    void OnSwitchDescriptionClicked(CUIWindow*, void*);

    virtual void MarkSelected(bool);
    virtual bool OnDbClick();
};

class CUITaskSubItem : public CUITaskItem
{
    RTTI_DECLARE_TYPEINFO(CUITaskSubItem, CUITaskItem);

private:
    using inherited = CUITaskItem;

    u32 m_active_color;
    u32 m_failed_color;
    u32 m_accomplished_color;
    u32 m_skipped_color;

protected:
    CUIStatic* m_ActiveObjectiveStatic;
    CUI3tButton* m_showDescriptionBtn;
    CUIStatic* m_descriptionStatic;
    CUIStatic* m_stateStatic;

    void Init();

public:
    explicit CUITaskSubItem(CUIEventsWnd* w);
    ~CUITaskSubItem() override;

    virtual void Update();
    virtual void SetGameTask(CGameTask* gt, u16 obj_idx);
    void OnActiveObjectiveClicked();
    void OnShowDescriptionClicked(CUIWindow*, void*);
    virtual void MarkSelected(bool b);
    virtual bool OnDbClick();
};
