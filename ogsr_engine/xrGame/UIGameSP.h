#pragma once

#include "uigamecustom.h"
#include "ui/UIDialogWnd.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "game_graph_space.h"

class CUIInventoryWnd;
class CUITradeWnd;
class CUIPdaWnd;
class CUITalkWnd;
class CUICarBodyWnd;
class CInventory;

class game_cl_Single;
class CChangeLevelWnd;
class CUIMessageBox;
class IInventoryBox;
class CInventoryOwner;

class CUIGameSP : public CUIGameCustom
{
    RTTI_DECLARE_TYPEINFO(CUIGameSP, CUIGameCustom);

private:
    game_cl_Single* m_game{};
    typedef CUIGameCustom inherited;

public:
    CUIGameSP();
    ~CUIGameSP() override;

    tmc::task<void> reset_ui() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    virtual void Render();
    virtual void SetClGame(game_cl_GameState* g);
    tmc::task<bool> IR_OnKeyboardPress(xr::key_id dik) override;
    [[nodiscard]] bool IR_OnKeyboardRelease(xr::key_id dik) override;

    void StartTalk();
    void StartCarBody(CInventoryOwner* pOurInv, CInventoryOwner* pOthers);
    void StartCarBody(CInventoryOwner* pOurInv, IInventoryBox* pBox);
    virtual void ReInitShownUI();
    void ChangeLevel(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang, Fvector pos2, Fvector ang2, bool b);

    virtual void HideShownDialogs();

    CUIInventoryWnd* InventoryMenu;
    CUIPdaWnd* PdaMenu;
    CUITalkWnd* TalkMenu;
    CUICarBodyWnd* UICarBodyMenu;
    CChangeLevelWnd* UIChangeLevelWnd;

    void ShowHidePda(const bool show);
};

class CChangeLevelWnd : public CUIDialogWnd
{
    RTTI_DECLARE_TYPEINFO(CChangeLevelWnd, CUIDialogWnd);

private:
    CUIMessageBox* m_messageBox;
    typedef CUIDialogWnd inherited;

    void OnCancel();
    void OnOk();

public:
    GameGraph::_GRAPH_ID m_game_vertex_id;
    u32 m_level_vertex_id;
    Fvector m_position;
    Fvector m_angles;
    Fvector m_position_cancel;
    Fvector m_angles_cancel;
    bool m_b_position_cancel;

    CChangeLevelWnd();
    ~CChangeLevelWnd() override = default;

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
    virtual bool WorkInPause() const { return true; }
    virtual void Show();
    virtual void Hide();
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
};

extern bool g_block_pause;
