#include "stdafx.h"

#include "level.h"

#include "HUDmanager.h"
#include "..\xr_3da\XR_IOConsole.h"
#include "entity_alive.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "alife_simulator_header.h"
#include "level_graph.h"
#include "../xr_3da/fdemorecord.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "stalker_movement_manager.h"
#include "Inventory.h"
#include "xrServer.h"

#include "actor.h"
#include "huditem.h"
#include "ui/UIDialogWnd.h"
#include "clsid_game.h"
#include "../xr_3da/xr_input.h"
#include "saved_game_wrapper.h"

#include "game_object_space.h"
#include "GamePersistent.h"
#include "MainMenu.h"
#include "UIGameSP.h"

#ifdef DEBUG
#include "ai/monsters/BaseMonster/base_monster.h"

extern void try_change_current_entity();
extern void restore_actor();
#endif

bool g_bDisableAllInput = false;

#define CURRENT_ENTITY() (game ? CurrentEntity() : nullptr)

tmc::task<void> CLevel::IR_OnMouseWheel(gsl::index direction)
{
    if (g_bDisableAllInput)
        co_return;

    if (co_await HUD().GetUI()->IR_OnMouseWheel(direction))
        co_return;

    if (Device.Paused())
        co_return;

    if (game != nullptr && co_await Game().IR_OnMouseWheel(direction))
        co_return;

    if (Actor())
        Actor()->callback(GameObject::eOnMouseWheel)(direction);

    if (HUD().GetUI()->MainInputReceiver())
        co_return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            co_await IR->IR_OnMouseWheel(direction);
    }
}

void CLevel::IR_OnMouseMove(int dx, int dy)
{
    if (g_bDisableAllInput)
        return;
    if (pHUD->GetUI()->IR_OnMouseMove(dx, dy))
        return;
    if (Device.Paused())
        return;

    if (Actor())
        Actor()->callback(GameObject::eOnMouseMove)(dx, dy);

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnMouseMove(dx, dy);
    }
}

// Обработка нажатия клавиш

tmc::task<void> CLevel::IR_OnKeyboardPress(xr::key_id key)
{
    if (GamePersistent().OnKeyboardPress())
        co_return;

    const auto _curr = get_binded_action(key);

    if (m_blocked_actions.find(_curr) != m_blocked_actions.end())
        co_return; // Real Wolf. 14.10.2014

    const bool b_ui_exist = pHUD != nullptr && pHUD->GetUI() != nullptr;

    switch (_curr)
    {
    case EGameActions::kSCREENSHOT: Render->Screenshot(); co_return;
    case EGameActions::kCONSOLE: co_await Console->Show(); co_return;
    case EGameActions::kQUIT:
        if (b_ui_exist && HUD().GetUI()->MainInputReceiver())
        {
            if (co_await HUD().GetUI()->MainInputReceiver()->IR_OnKeyboardPress(key))
                co_return; // special case for mp and main_menu

            if (MainMenu()->IsActive() || !Device.Paused())
                HUD().GetUI()->StartStopMenu(HUD().GetUI()->MainInputReceiver(), true);
        }
        else
        {
            co_await Device.execute_async("main_menu");
        }

        co_return;
    case EGameActions::kPAUSE:
        if (!g_block_pause)
            Device.Pause(!Device.Paused(), TRUE, TRUE, "li_pause_key");

        co_return;
    default: break;
    }

    if (g_bDisableAllInput)
        co_return;

    if (g_block_all_except_movement)
    {
        if (!(_curr < EGameActions::kCAM_1 || _curr == EGameActions::kPAUSE || _curr == EGameActions::kSCREENSHOT || _curr == EGameActions::kQUIT ||
              _curr == EGameActions::kCONSOLE))
            co_return;
    }

    if (!b_ui_exist)
        co_return;

    if (Actor())
    {
        CKeyBinding KB;

        Actor()->callback(GameObject::eOnKeyPress)(key, _curr, &KB);
        if (KB.ignore)
            co_return;

        if (g_bDisableAllInput)
            co_return;
    }

    if (b_ui_exist && co_await pHUD->GetUI()->IR_OnKeyboardPress(key))
        co_return;

    if (Device.Paused())
        co_return;

    if (game != nullptr && co_await Game().IR_OnKeyboardPress(key))
        co_return;

    if (_curr == EGameActions::kQUICK_SAVE)
    {
        co_await Device.execute_async("save");
        co_return;
    }
    else if (_curr == EGameActions::kQUICK_LOAD)
    {
        string_path saved_game, command;
        xr_strconcat(saved_game, Core.UserName, "_", "quicksave");

        if (!CSavedGameWrapper::valid_saved_game(saved_game))
            co_return;

        xr_strconcat(command, "load ", saved_game);

        co_await Device.execute_async(command);
        co_return;
    }

    if (bindConsoleCmds.execute(key))
        co_return;

    if (b_ui_exist && HUD().GetUI()->MainInputReceiver())
        co_return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            co_await IR->IR_OnKeyboardPress(key);
    }

#ifdef DEBUG
    CObject* obj = Level().Objects.FindObjectByName("monster");
    if (obj)
    {
        CBaseMonster* monster = smart_cast<CBaseMonster*>(obj);
        if (monster)
            monster->debug_on_key(key);
    }
#endif
}

void CLevel::IR_OnKeyboardRelease(xr::key_id key)
{
    if (g_bDisableAllInput)
        return;

    const auto _curr = get_binded_action(key);

    if (m_blocked_actions.find(_curr) != m_blocked_actions.end())
        return; // Real Wolf. 14.10.2014

    if (g_block_all_except_movement)
    {
        if (!(_curr < EGameActions::kCAM_1 || _curr == EGameActions::kPAUSE || _curr == EGameActions::kSCREENSHOT || _curr == EGameActions::kQUIT ||
              _curr == EGameActions::kCONSOLE))
            return;
    }

    const bool b_ui_exist = pHUD != nullptr && pHUD->GetUI() != nullptr;
    if (b_ui_exist && pHUD->GetUI()->IR_OnKeyboardRelease(key))
        return;

    if (Device.Paused())
        return;

    if (game && Game().OnKeyboardRelease(_curr))
        return;

    if (key != xr::key_id{sf::Keyboard::Scancode::LAlt} && key != xr::key_id{sf::Keyboard::Scancode::RAlt} && key != xr::key_id{sf::Keyboard::Scancode::F4} && Actor() != nullptr)
        Actor()->callback(GameObject::eOnKeyRelease)(key, _curr);

    if (b_ui_exist && HUD().GetUI()->MainInputReceiver())
        return;

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            IR->IR_OnKeyboardRelease(key);
    }
}

tmc::task<void> CLevel::IR_OnKeyboardHold(xr::key_id key)
{
    if (g_bDisableAllInput)
        co_return;

    const auto _curr = get_binded_action(key);

    if (m_blocked_actions.find(_curr) != m_blocked_actions.end())
        co_return; // Real Wolf. 14.10.2014

    if (g_block_all_except_movement)
    {
        if (!(_curr < EGameActions::kCAM_1 || _curr == EGameActions::kPAUSE || _curr == EGameActions::kSCREENSHOT || _curr == EGameActions::kQUIT ||
              _curr == EGameActions::kCONSOLE))
            co_return;
    }

    const bool b_ui_exist = pHUD != nullptr && pHUD->GetUI() != nullptr;
    if (b_ui_exist && co_await pHUD->GetUI()->IR_OnKeyboardHold(key))
        co_return;
    if (b_ui_exist && HUD().GetUI()->MainInputReceiver())
        co_return;

    if (Device.Paused())
        co_return;

    if (key != xr::key_id{sf::Keyboard::Scancode::LAlt} && key != xr::key_id{sf::Keyboard::Scancode::RAlt} && key != xr::key_id{sf::Keyboard::Scancode::F4} && Actor() != nullptr)
        Actor()->callback(GameObject::eOnKeyHold)(key, _curr);

    if (CURRENT_ENTITY())
    {
        IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY()));
        if (IR)
            co_await IR->IR_OnKeyboardHold(key);
    }
}

void CLevel::IR_OnMouseStop(int, int) {}

tmc::task<void> CLevel::IR_OnActivate()
{
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

void CLevel::block_action(EGameActions cmd) { m_blocked_actions.insert(cmd); }
void CLevel::unblock_action(EGameActions cmd) { m_blocked_actions.erase(cmd); }
