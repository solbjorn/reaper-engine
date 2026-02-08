#include "stdafx.h"

#include "uigamesp.h"

#include "actor.h"
#include "level.h"

#include "game_cl_Single.h"
#include "ui/UIPdaAux.h"
#include "xr_level_controller.h"
#include "actorcondition.h"
#include "inventory.h"
#include "object_broker.h"
#include "GameTaskManager.h"
#include "GameTask.h"
#include "PDA.h"
#include "player_hud.h"

#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UIPdaWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"
#include "ui/UIMessageBox.h"

CUIGameSP::CUIGameSP()
{
    InventoryMenu = xr_new<CUIInventoryWnd>();
    PdaMenu = xr_new<CUIPdaWnd>();
    TalkMenu = xr_new<CUITalkWnd>();
    UICarBodyMenu = xr_new<CUICarBodyWnd>();
    UIChangeLevelWnd = xr_new<CChangeLevelWnd>();
}

CUIGameSP::~CUIGameSP()
{
    delete_data(InventoryMenu);
    delete_data(PdaMenu);
    delete_data(TalkMenu);
    delete_data(UICarBodyMenu);
    delete_data(UIChangeLevelWnd);
}

tmc::task<void> CUIGameSP::shedule_Update(u32 dt)
{
    co_await inherited::shedule_Update(dt);

    CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
    if (pActor == nullptr || pActor->g_Alive())
        co_return;

    HideShownDialogs();
}

void CUIGameSP::HideShownDialogs()
{
    CUIDialogWnd* mir = MainInputReceiver();
    if (mir && (mir == InventoryMenu || mir == PdaMenu || mir == TalkMenu || mir == UICarBodyMenu))
        mir->GetHolder()->StartStopMenu(mir, true);
}

void CUIGameSP::SetClGame(game_cl_GameState* g)
{
    inherited::SetClGame(g);
    m_game = smart_cast<game_cl_Single*>(g);
    R_ASSERT(m_game);
}

tmc::task<bool> CUIGameSP::IR_OnKeyboardPress(xr::key_id dik)
{
    if (co_await inherited::IR_OnKeyboardPress(dik))
        co_return true;

    if (Device.Paused())
        co_return false;

    CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
    if (!pActor)
        co_return false;
    if (pActor && !pActor->g_Alive())
        co_return false;

    hud_adjust_mode_keyb(dik);
    if (attach_adjust_mode_keyb(dik))
        co_return true;

    if (Core.Features.test(xrCore::Feature::busy_actor_restrictions))
    {
        const auto active_hud = smart_cast<CHudItem*>(pActor->inventory().ActiveItem());
        if (active_hud != nullptr && active_hud->GetState() != CHudItem::eIdle)
            co_return false;
    }

    const auto bind = get_binded_action(dik);
    switch (bind)
    {
    case EGameActions::kINVENTORY:
        if (!MainInputReceiver() || MainInputReceiver() == InventoryMenu)
        {
            auto Pda = pActor->GetPDA();
            if (!Pda || !Pda->Is3DPDA() || !psActorFlags.test(AF_3D_PDA) || !PdaMenu->IsShown())
            {
                m_game->StartStopMenu(InventoryMenu, true);
                co_return true;
            }
        }

        break;
    case EGameActions::kACTIVE_JOBS:
    case EGameActions::kMAP:
    case EGameActions::kCONTACTS: {
        auto Pda = pActor->GetPDA();
        if ((!Pda || !Pda->Is3DPDA() || !psActorFlags.test(AF_3D_PDA)) && (!MainInputReceiver() || MainInputReceiver() == PdaMenu))
        {
            if (g_actor_allow_pda)
            {
                PdaMenu->SetActiveSubdialog(bind == EGameActions::kACTIVE_JOBS ? eptQuests : (bind == EGameActions::kMAP ? eptMap : eptContacts));
                m_game->StartStopMenu(PdaMenu, true);

                co_return true;
            }
        }

        break;
    }
    case EGameActions::kWPN_FIRE:
    case EGameActions::kWPN_ZOOM: {
        auto Pda = pActor->GetPDA();
        if (Pda && Pda->Is3DPDA() && psActorFlags.test(AF_3D_PDA) && PdaMenu->IsShown() && (!MainInputReceiver() || MainInputReceiver() != PdaMenu))
        {
            Flags8 IRFlags{};
            IRFlags.set(recvItem::eCrosshair, psHUD_Flags.test(HUD_CROSSHAIR_RT));
            IRFlags.set(recvItem::eIndicators, HUD().GetUI()->GameIndicatorsShown());

            HUD().GetUI()->SetMainInputReceiver(PdaMenu, false, IRFlags);
            Pda->m_bZoomed = true;

            co_return true;
        }

        break;
    }
    case EGameActions::kSCORES: {
        SDrawStaticStruct* ss = AddCustomStatic("main_task", true);
        SGameTaskObjective* o = pActor->GameTaskManager().ActiveObjective();
        if (!o)
            ss->m_static->SetTextST("st_no_active_task");
        else
            ss->m_static->SetTextST(*(o->description));

        break;
    }
    default: break;
    }

    co_return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(xr::key_id dik)
{
    if (inherited::IR_OnKeyboardRelease(dik))
        return true;

    if (is_binded(EGameActions::kSCORES, dik))
        RemoveCustomStatic("main_task");

    return false;
}

void CUIGameSP::Render()
{
    inherited::Render();

    hud_draw_adjust_mode();
    attach_draw_adjust_mode();
}

void CUIGameSP::StartTalk() { m_game->StartStopMenu(TalkMenu, true); }

void CUIGameSP::StartCarBody(CInventoryOwner* pOurInv, CInventoryOwner* pOthers)
{
    if (MainInputReceiver())
        return;
    UICarBodyMenu->InitCarBody(pOurInv, pOthers);
    m_game->StartStopMenu(UICarBodyMenu, true);
}
void CUIGameSP::StartCarBody(CInventoryOwner* pOurInv, IInventoryBox* pBox)
{
    if (MainInputReceiver())
        return;
    UICarBodyMenu->InitCarBody(pOurInv, pBox);
    m_game->StartStopMenu(UICarBodyMenu, true);
}

void CUIGameSP::ReInitShownUI()
{
    if (InventoryMenu->IsShown())
        InventoryMenu->InitInventory_delayed();
    else if (UICarBodyMenu->IsShown())
        UICarBodyMenu->UpdateLists_delayed();
}

void CUIGameSP::ChangeLevel(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang, Fvector pos2, Fvector ang2, bool b)
{
    if (!MainInputReceiver() || MainInputReceiver() != UIChangeLevelWnd)
    {
        UIChangeLevelWnd->m_game_vertex_id = game_vert_id;
        UIChangeLevelWnd->m_level_vertex_id = level_vert_id;
        UIChangeLevelWnd->m_position = pos;
        UIChangeLevelWnd->m_angles = ang;
        UIChangeLevelWnd->m_position_cancel = pos2;
        UIChangeLevelWnd->m_angles_cancel = ang2;
        UIChangeLevelWnd->m_b_position_cancel = b;
        m_game->StartStopMenu(UIChangeLevelWnd, true);
    }
}

tmc::task<void> CUIGameSP::reset_ui()
{
    co_await inherited::reset_ui();

    InventoryMenu->Reset();
    PdaMenu->Reset();
    TalkMenu->Reset();
    UICarBodyMenu->Reset();
    UIChangeLevelWnd->Reset();
}

void CUIGameSP::ShowHidePda(const bool show)
{
    if ((PdaMenu->IsShown() && !show) || (!PdaMenu->IsShown() && show))
    {
        m_game->StartStopMenu(PdaMenu, false);
        if (show || MainInputReceiver() == PdaMenu)
            HUD().GetUI()->SetMainInputReceiver(nullptr, false);
    }
}

CChangeLevelWnd::CChangeLevelWnd()
{
    m_messageBox = xr_new<CUIMessageBox>();
    m_messageBox->SetAutoDelete(true);
    AttachChild(m_messageBox);
    m_messageBox->Init("message_box_change_level");
    SetWndPos(m_messageBox->GetWndPos());
    m_messageBox->SetWndPos(0.0f, 0.0f);
    SetWndSize(m_messageBox->GetWndSize());
}

void CChangeLevelWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (pWnd == m_messageBox)
    {
        if (msg == MESSAGE_BOX_YES_CLICKED)
        {
            OnOk();
        }
        else if (msg == MESSAGE_BOX_NO_CLICKED)
        {
            OnCancel();
        }
    }
    else
        inherited::SendMessage(pWnd, msg, pData);
}

void CChangeLevelWnd::OnOk()
{
    Game().StartStopMenu(this, true);
    NET_Packet p;
    p.w_begin(M_CHANGE_LEVEL);
    p.w(&m_game_vertex_id, sizeof(m_game_vertex_id));
    p.w(&m_level_vertex_id, sizeof(m_level_vertex_id));
    p.w_vec3(m_position);
    p.w_vec3(m_angles);

    Level().Send(p, net_flags(TRUE));
}

void CChangeLevelWnd::OnCancel()
{
    Game().StartStopMenu(this, true);
    if (m_b_position_cancel)
    {
        Actor()->MoveActor(m_position_cancel, m_angles_cancel);
    }
}

bool CChangeLevelWnd::OnKeyboard(xr::key_id dik, EUIMessages keyboard_action)
{
    if (keyboard_action == WINDOW_KEY_PRESSED)
    {
        if (dik == xr::key_id{sf::Keyboard::Scancode::Enter} || dik == xr::key_id{sf::Keyboard::Scancode::NumpadEnter})
        {
            OnOk();
            return true;
        }
        else if (is_binded(EGameActions::kQUIT, dik))
        {
            OnCancel();
            return true;
        }
    }

    return inherited::OnKeyboard(dik, keyboard_action);
}

bool g_block_pause = false;

void CChangeLevelWnd::Show()
{
    g_block_pause = true;
    Device.Pause(TRUE, TRUE, TRUE, "CChangeLevelWnd_show");
    bShowPauseString = FALSE;
}

void CChangeLevelWnd::Hide()
{
    g_block_pause = false;
    Device.Pause(FALSE, TRUE, TRUE, "CChangeLevelWnd_hide");
}
