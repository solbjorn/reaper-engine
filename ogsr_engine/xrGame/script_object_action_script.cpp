////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_object_action.h"
#include "script_game_object.h"

void CScriptObjectAction::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptObjectAction>(
        "object", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptObjectAction(), CScriptObjectAction(CScriptGameObject*, MonsterSpace::EObjectAction),
                          CScriptObjectAction(CScriptGameObject*, MonsterSpace::EObjectAction, u32), CScriptObjectAction(MonsterSpace::EObjectAction),
                          CScriptObjectAction(LPCSTR, MonsterSpace::EObjectAction)>(),

        // state
        "idle", sol::var(MonsterSpace::eObjectActionIdle), "show", sol::var(MonsterSpace::eObjectActionShow), "hide", sol::var(MonsterSpace::eObjectActionHide), "take",
        sol::var(MonsterSpace::eObjectActionTake), "drop", sol::var(MonsterSpace::eObjectActionDrop), "strap", sol::var(MonsterSpace::eObjectActionStrapped), "aim1",
        sol::var(MonsterSpace::eObjectActionAim1), "aim2", sol::var(MonsterSpace::eObjectActionAim2), "reload", sol::var(MonsterSpace::eObjectActionReload1), "reload1",
        sol::var(MonsterSpace::eObjectActionReload1), "reload2", sol::var(MonsterSpace::eObjectActionReload2), "fire1", sol::var(MonsterSpace::eObjectActionFire1), "fire2",
        sol::var(MonsterSpace::eObjectActionFire2), "switch1", sol::var(MonsterSpace::eObjectActionSwitch1), "switch2", sol::var(MonsterSpace::eObjectActionSwitch2), "activate",
        sol::var(MonsterSpace::eObjectActionActivate), "deactivate", sol::var(MonsterSpace::eObjectActionDeactivate), "use", sol::var(MonsterSpace::eObjectActionUse), "turn_on",
        sol::var(MonsterSpace::eObjectActionTurnOn), "turn_off", sol::var(MonsterSpace::eObjectActionTurnOff), "dummy", sol::var(MonsterSpace::eObjectActionDummy),

        "action", &CScriptObjectAction::SetObjectAction, "object",
        sol::overload(sol::resolve<void(LPCSTR)>(&CScriptObjectAction::SetObject), sol::resolve<void(CScriptGameObject*)>(&CScriptObjectAction::SetObject)), "completed",
        sol::resolve<bool()>(&CScriptObjectAction::completed));
}
