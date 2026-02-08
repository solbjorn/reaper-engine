#include "stdafx.h"

#include "UIGameTutorial.h"

#include "UIWindow.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "..\..\xr_3da\xr_input.h"
#include "../xr_level_controller.h"
#include "../script_engine.h"
#include "../ai_space.h"

void CUISequenceItem::Load(CUIXml* xml, int idx)
{
    XML_NODE* _stored_root = xml->GetLocalRoot();
    xml->SetLocalRoot(xml->NavigateToNode("item", idx));
    int disabled_cnt = xml->GetNodesNum(xml->GetLocalRoot(), "disabled_key");
    for (int i = 0; i < disabled_cnt; ++i)
    {
        LPCSTR str = xml->Read("disabled_key", i, nullptr);
        m_disabled_actions.push_back(action_name_to_id(str));
    }

    LPCSTR str;
    bool function_exists;
    int j;
    int f_num = xml->GetNodesNum(xml->GetLocalRoot(), "function_on_start");
    m_start_lua_functions.resize(f_num);
    for (j = 0; j < f_num; ++j)
    {
        str = xml->Read(xml->GetLocalRoot(), "function_on_start", j, nullptr);
        function_exists = ai().script_engine().function(str, m_start_lua_functions[j]);
        THROW3(function_exists, "Cannot find script function described in tutorial item ", str);
    }

    f_num = xml->GetNodesNum(xml->GetLocalRoot(), "function_on_stop");
    m_stop_lua_functions.resize(f_num);
    for (j = 0; j < f_num; ++j)
    {
        str = xml->Read(xml->GetLocalRoot(), "function_on_stop", j, nullptr);
        function_exists = ai().script_engine().function(str, m_stop_lua_functions[j]);
        THROW3(function_exists, "Cannot find script function described in tutorial item ", str);
    }

    xml->SetLocalRoot(_stored_root);
}

bool CUISequenceItem::AllowKey(xr::key_id dik)
{
    const auto it = std::find(m_disabled_actions.begin(), m_disabled_actions.end(), get_binded_action(dik));
    if (it == m_disabled_actions.end())
        return true;

    return false;
}

namespace
{
void CallFunctions(xr_vector<sol::function>& v)
{
    for (auto& fn : v)
        fn();
}
} // namespace

void CUISequenceItem::Start() { CallFunctions(m_start_lua_functions); }

bool CUISequenceItem::Stop(bool)
{
    CallFunctions(m_stop_lua_functions);
    return true;
}

CUISequencer::CUISequencer()
{
    m_bActive = false;
    m_bPlayEachItem = false;
}

tmc::task<void> CUISequencer::Start(gsl::czstring tutor_name)
{
    VERIFY(m_items.size() == 0);
    Device.seqFrame.Add(this, REG_PRIORITY_LOW - 10000);
    Device.seqRender.Add(this, 3);

    m_UIWindow = xr_new<CUIWindow>();

    CUIXml uiXml;
    uiXml.Init(CONFIG_PATH, UI_PATH, "game_tutorials.xml");

    int items_count = uiXml.GetNodesNum(tutor_name, 0, "item");
    VERIFY(items_count > 0);
    uiXml.SetLocalRoot(uiXml.NavigateToNode(tutor_name, 0));

    m_bPlayEachItem = !!uiXml.ReadInt("play_each_item", 0, 0);

    CUIXmlInit xml_init;
    xml_init.InitWindow(uiXml, "global_wnd", 0, m_UIWindow);
    //.	xml_init.InitAutoStaticGroup(uiXml, "global_wnd",		m_UIWindow);

    for (int i = 0; i < items_count; ++i)
    {
        LPCSTR _tp = uiXml.ReadAttrib("item", i, "type", "");
        CUISequenceItem* pItem;

        if (std::is_eq(xr::strcasecmp(_tp, "video")))
            pItem = xr_new<CUISequenceVideoItem>(this);
        else
            pItem = xr_new<CUISequenceSimpleItem>(this);

        m_items.push_back(pItem);
        pItem->Load(&uiXml, i);
    }

    CUISequenceItem* pCurrItem = m_items.front();
    pCurrItem->Start();
    m_pStoredInputReceiver = pInput->CurrentIR();
    co_await IR_Capture();
    m_bActive = true;
}

tmc::task<void> CUISequencer::Destroy()
{
    m_bActive = false;
    Device.seqFrame.Remove(this);
    Device.seqRender.Remove(this);

    delete_data(m_items);
    delete_data(m_UIWindow);
    co_await IR_Release();
    m_pStoredInputReceiver = nullptr;
}

tmc::task<void> CUISequencer::Stop()
{
    if (m_items.size())
    {
        if (m_bPlayEachItem)
        {
            Next();
            co_return;
        }
        else
        {
            CUISequenceItem* pCurrItem = m_items.front();
            pCurrItem->Stop(true);
        }
    }

    co_await Destroy();
}

tmc::task<void> CUISequencer::OnFrame()
{
    if (!m_bActive)
        co_return;

    if (m_items.empty())
    {
        co_await Stop();
        co_return;
    }

    CUISequenceItem* pCurrItem = m_items.front();
    if (!pCurrItem->IsPlaying())
        Next();

    if (m_items.empty())
    {
        co_await Stop();
        co_return;
    }

    m_items.front()->Update();
    m_UIWindow->Update();
}

tmc::task<void> CUISequencer::OnRender()
{
    if (m_UIWindow->IsShown())
        m_UIWindow->Draw();

    VERIFY(m_items.size());
    m_items.front()->OnRender();

    co_return;
}

void CUISequencer::Next()
{
    CUISequenceItem* pCurrItem = m_items.front();
    bool can_stop = pCurrItem->Stop();
    if (!can_stop)
        return;

    m_items.pop_front();
    delete_data(pCurrItem);

    if (m_items.size())
    {
        pCurrItem = m_items.front();
        pCurrItem->Start();
    }
}

bool CUISequencer::GrabInput()
{
    if (!m_items.empty())
        return m_items.front()->GrabInput();

    return false;
}

void CUISequencer::IR_OnMouseMove(int x, int y)
{
    if (m_bActive && !GrabInput() && m_pStoredInputReceiver)
        m_pStoredInputReceiver->IR_OnMouseMove(x, y);
}

void CUISequencer::IR_OnMouseStop(int x, int y)
{
    if (m_bActive && !GrabInput() && m_pStoredInputReceiver)
        m_pStoredInputReceiver->IR_OnMouseStop(x, y);
}

void CUISequencer::IR_OnKeyboardRelease(xr::key_id dik)
{
    if (m_bActive && !GrabInput() && m_pStoredInputReceiver)
        m_pStoredInputReceiver->IR_OnKeyboardRelease(dik);
}

tmc::task<void> CUISequencer::IR_OnKeyboardHold(xr::key_id dik)
{
    if (m_bActive && !GrabInput() && m_pStoredInputReceiver)
        co_await m_pStoredInputReceiver->IR_OnKeyboardHold(dik);
}

tmc::task<void> CUISequencer::IR_OnMouseWheel(gsl::index direction)
{
    if (m_bActive && !GrabInput() && m_pStoredInputReceiver)
        co_await m_pStoredInputReceiver->IR_OnMouseWheel(direction);
}

tmc::task<void> CUISequencer::IR_OnKeyboardPress(xr::key_id dik)
{
    if (!m_bActive)
        co_return;

    if (m_items.size())
        m_items.front()->OnKeyboardPress(dik);

    bool b = true;
    if (m_items.size())
        b &= m_items.front()->AllowKey(dik);

    if (b && is_binded(EGameActions::kQUIT, dik))
    {
        co_await Stop();
        co_return;
    }

    if (b && !GrabInput() && m_pStoredInputReceiver)
        co_await m_pStoredInputReceiver->IR_OnKeyboardPress(dik);
}

tmc::task<void> CUISequencer::IR_OnActivate()
{
    if (!m_bActive)
        co_return;

    if (!pInput)
        co_return;

    for (auto& key : xr::key_ids())
    {
        if (IR_GetKeyState(key.dik))
        {
            switch (get_binded_action(key.dik))
            {
            case EGameActions::kFWD:
            case EGameActions::kBACK:
            case EGameActions::kL_STRAFE:
            case EGameActions::kR_STRAFE:
            case EGameActions::kLEFT:
            case EGameActions::kRIGHT:
            case EGameActions::kUP:
            case EGameActions::kDOWN:
            case EGameActions::kCROUCH:
            case EGameActions::kACCEL:
            case EGameActions::kL_LOOKOUT:
            case EGameActions::kR_LOOKOUT:
            case EGameActions::kWPN_FIRE: co_await IR_OnKeyboardPress(key.dik); break;
            default: break;
            }
        }
    }
}
