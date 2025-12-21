#include "stdafx.h"

#include "UIDialogHolder.h"

#include "HUDManager.h"
#include "UICursor.h"
#include "level.h"
#include "actor.h"
#include "ui/UIScriptWnd.h"
#include "xr_level_controller.h"

dlgItem::dlgItem(CUIWindow* pWnd) : wnd{pWnd} {}

bool dlgItem::operator<(const dlgItem& itm) const { return (int)enabled > (int)itm.enabled; }
bool dlgItem::operator==(const dlgItem& that) const { return wnd == that.wnd; }

recvItem::recvItem(CUIDialogWnd* r) : m_item{r} {}

bool recvItem::operator==(const recvItem& that) const { return m_item == that.m_item; }

CDialogHolder::CDialogHolder()
{
    shedule.t_min = 5;
    shedule.t_max = 20;
    shedule_register();
    Device.seqFrame.Add(this, REG_PRIORITY_LOW - 1000);
}

CDialogHolder::~CDialogHolder()
{
    shedule_unregister();
    Device.seqFrame.Remove(this);
}

void CDialogHolder::StartMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
    R_ASSERT(!pDialog->IsShown());

    if (MainInputReceiver() && GetUICursor()->IsVisible())
    {
        // предыдущий диалог теряет фокус
        MainInputReceiver()->UpdateFocus(true);
        MainInputReceiver()->CommitFocus(true);
    }

    AddDialogToRender(pDialog);
    SetMainInputReceiver(pDialog, false);

    if (UseIndicators())
    {
        bool b = !!psHUD_Flags.test(HUD_CROSSHAIR_RT);
        m_input_receivers.back().m_flags.set(recvItem::eCrosshair, b);

        b = HUD().GetUI()->GameIndicatorsShown();
        m_input_receivers.back().m_flags.set(recvItem::eIndicators, b);

        if (bDoHideIndicators)
        {
            psHUD_Flags.set(HUD_CROSSHAIR_RT, FALSE);
            HUD().GetUI()->HideGameIndicators();
        }
    }
    pDialog->SetHolder(this);
    pDialog->Show();

    if (pDialog->NeedCursor())
        GetUICursor()->Show();

    if (g_pGameLevel)
    {
        CActor* A = smart_cast<CActor*>(Level().CurrentViewEntity());
        if (A && pDialog->StopAnyMove())
        {
            A->StopAnyMove();
        }

        if (A)
        {
            A->PickupModeOff();
            A->IR_OnKeyboardRelease(kWPN_ZOOM);
            A->IR_OnKeyboardRelease(kWPN_FIRE);
        }
    }
}

void CDialogHolder::StopMenu(CUIDialogWnd* pDialog)
{
    R_ASSERT(pDialog->IsShown());

    if (MainInputReceiver() == pDialog)
    {
        if (GetUICursor()->IsVisible())
        {
            // текущий диалог теряет фокус
            pDialog->UpdateFocus(true);
            pDialog->CommitFocus(true);
        }
        if (UseIndicators())
        {
            bool b = !!m_input_receivers.back().m_flags.test(recvItem::eCrosshair);
            psHUD_Flags.set(HUD_CROSSHAIR_RT, b);
            b = !!m_input_receivers.back().m_flags.test(recvItem::eIndicators);
            if (b)
                HUD().GetUI()->ShowGameIndicators();
            else
                HUD().GetUI()->HideGameIndicators();
        }

        RemoveDialogToRender(pDialog);
        SetMainInputReceiver(nullptr, false);
        pDialog->SetHolder(nullptr);
        pDialog->Hide();
    }
    else
    {
        RemoveDialogToRender(pDialog);
        SetMainInputReceiver(pDialog, true);
        pDialog->SetHolder(nullptr);
        pDialog->Hide();
    }

    if (!MainInputReceiver() || !MainInputReceiver()->NeedCursor())
        GetUICursor()->Hide();

    if (const auto menu = smart_cast<CUIDialogWndEx*>(pDialog); menu != nullptr)
        std::erase_if(script_menus, [menu](const auto& ptr) { return ptr.get() == menu; });
}

void CDialogHolder::AddDialogToRender(CUIWindow* pDialog)
{
    dlgItem itm(pDialog);
    xr_vector<dlgItem>::iterator it = std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), itm);
    if ((it == m_dialogsToRender.end()) || (it != m_dialogsToRender.end() && (*it).enabled == false))
    {
        m_dialogsToRender.push_back(itm);
        pDialog->Show(true);
    }
}

void CDialogHolder::RemoveDialogToRender(CUIWindow* pDialog)
{
    dlgItem itm(pDialog);
    xr_vector<dlgItem>::iterator it = std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), itm);
    if (it != m_dialogsToRender.end())
    {
        (*it).wnd->Show(false);
        (*it).wnd->Enable(false);
        (*it).enabled = false;
        m_dialogsToRender.erase(it);
    }
}

void CDialogHolder::DoRenderDialogs()
{
    xr_vector<dlgItem>::iterator it = m_dialogsToRender.begin();
    for (; it != m_dialogsToRender.end(); ++it)
    {
        if ((*it).enabled && (*it).wnd->IsShown())
            (*it).wnd->Draw();
    }
}

CUIDialogWnd* CDialogHolder::MainInputReceiver()
{
    if (m_input_receivers.empty())
        return nullptr;

    return m_input_receivers.back().m_item;
}

void CDialogHolder::SetMainInputReceiver(CUIDialogWnd* ir, bool _find_remove, const Flags8 flags)
{
    if (MainInputReceiver() == ir)
        return;

    if (!ir || _find_remove)
    {
        if (m_input_receivers.empty())
            return;

        if (!ir)
            m_input_receivers.pop_back();
        else
        {
            VERIFY(ir && _find_remove);

            u32 cnt = m_input_receivers.size();
            for (; cnt > 0; --cnt)
                if (m_input_receivers[cnt - 1].m_item == ir)
                {
                    m_input_receivers[cnt].m_flags.set(recvItem::eCrosshair, m_input_receivers[cnt - 1].m_flags.test(recvItem::eCrosshair));
                    m_input_receivers[cnt].m_flags.set(recvItem::eIndicators, m_input_receivers[cnt - 1].m_flags.test(recvItem::eIndicators));
                    xr_vector<recvItem>::iterator it = m_input_receivers.begin();
                    std::advance(it, cnt - 1);
                    m_input_receivers.erase(it);
                    break;
                }
        }
    }
    else
    {
        m_input_receivers.emplace_back(ir);
        if (!flags.equal(Flags8{}))
            m_input_receivers.back().m_flags = flags;
    }
}

void CDialogHolder::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
    //.	ai().script_engine().script_log	(eLuaMessageTypeError,"foo");
    if (pDialog->IsShown())
        StopMenu(pDialog);
    else
        StartMenu(pDialog, bDoHideIndicators);
}

void CDialogHolder::StartStopMenu_script(std::unique_ptr<CUIDialogWndEx>& pDialog, bool bDoHideIndicators)
{
    const bool start = !pDialog->IsShown();
    StartStopMenu(pDialog.get(), bDoHideIndicators);

    if (start)
        script_menus.emplace_back(pDialog.release());
}

void CDialogHolder::OnFrame()
{
    if (MainInputReceiver() && GetUICursor()->IsVisible())
    {
        MainInputReceiver()->UpdateFocus();
        // Сначала генерируем события потери фокуса
        MainInputReceiver()->CommitFocus(true);
        // и только потом генерируем события получения фокуса, что бы всегда
        // потеря фокуса обрабатывалась первой, а не как получится.
        MainInputReceiver()->CommitFocus(false);
    }

    for (auto& it : m_dialogsToRender)
        if (it.enabled && it.wnd->IsEnabled())
            it.wnd->Update();
}

void CDialogHolder::shedule_Update(u32 dt)
{
    ISheduled::shedule_Update(dt);

    if (m_dialogsToRender.empty())
        return;

    m_dialogsToRender.erase(std::remove_if(m_dialogsToRender.begin(), m_dialogsToRender.end(), [](const auto& it) { return !it.enabled; }), m_dialogsToRender.end());
}

float CDialogHolder::shedule_Scale() const { return 0.5f; }

void CDialogHolder::CleanInternals()
{
    while (!m_input_receivers.empty())
        m_input_receivers.pop_back();

    m_dialogsToRender.clear();
    GetUICursor()->Hide();
}
